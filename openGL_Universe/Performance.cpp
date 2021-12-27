

#include "MyGlWindow.h"
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/dual_quaternion.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



MyGlWindow* win;

bool lbutton_down;
bool rbutton_down;
bool mbutton_down;
double m_lastMouseX;
double m_lastMouseY;
double cx, cy;



vector<glm::mat4> Transforms;
vector<glm::fdualquat> dualQuaternions;
vector<glm::mat2x4> DQs;

float deltaTime = 0.0f;   // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float animationTime = 0.0f;

double _frame_start_time;
double _delta_time;


template <class T>
class PingPongId
{
public:
    void swap() { _ping = pong(); }
    T ping() { return _ping; };
    T pong() { return (_ping + 1) % 2; }

private:
    T _ping;
};

int _frame_count{};
double _time_acc{};
double _cpu_time{};
double _cpu_time_min{ DBL_MAX };
double _cpu_time_max{};
double _cpu_time_avg{};
double _cpu_time_acc{};

GLuint64 _gpu_time{};
GLuint64 _gpu_time_min{ UINT_MAX };
GLuint64 _gpu_time_max{};
GLuint64 _gpu_time_avg{};
GLuint64 _gpu_time_acc{};

GLuint _gpu_timer_query[2]{};
PingPongId <size_t> _swap_id;


void mouseDragging(double width, double height);

void window_size_callback(GLFWwindow* window, int width, int height)
{
    win->setSize(width, height);
    win->setAspect(width / (float)height);

}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}


static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    cx = xpos;
    cy = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ||
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS ||
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS
        )
    {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        mouseDragging(w, h);
        win->changeCamera();
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        m_lastMouseX = xpos;
        m_lastMouseY = ypos;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (GLFW_PRESS == action)
            lbutton_down = true;
        else if (GLFW_RELEASE == action)
            lbutton_down = false;
    }

    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (GLFW_PRESS == action)
            rbutton_down = true;
        else if (GLFW_RELEASE == action)
            rbutton_down = false;
    }

    else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (GLFW_PRESS == action)
            mbutton_down = true;
        else if (GLFW_RELEASE == action)
            mbutton_down = false;
    }
}


void mouseDragging(double width, double height)
{


    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {



        if (lbutton_down) {
            float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
            float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
            win->m_viewer->rotate(fractionChangeX, fractionChangeY);
        }
        else if (mbutton_down) {
            float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
            float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
            win->m_viewer->zoom(fractionChangeY);
        }
        else if (rbutton_down) {
            float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
            float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
            win->m_viewer->translate(-fractionChangeX, -fractionChangeY, 1);
        }
    }

    m_lastMouseX = cx;
    m_lastMouseY = cy;

}





void drawStatsImGui(MyGlWindow* win, const double& time, const double& delta_time)
{
    constexpr size_t res = 32;

    const double refresh_interval_min = 0., refresh_interval_max = 5;

    static bool draw_time_plot = true;
    static double refresh_interval = 4.0;
    static bool show_gpu_ms = true;

    static bool freeze = false;

    static std::string cpu_histogram_text;
    static std::string gpu_histogram_text;
    static float cpu_time_data[res] = { 0.0f };
    static float gpu_time_data[res] = { 0.0f };
    static float cpu_histogram_max = FLT_MAX;
    static float gpu_histogram_max = FLT_MAX;

    double gpu_time_ms = _gpu_time * 1e-6;

    _cpu_time_min = min(_cpu_time_min, _cpu_time);
    _cpu_time_max = max(_cpu_time_max, _cpu_time);

    _gpu_time_min = min(_gpu_time_min, _gpu_time);
    _gpu_time_max = max(_gpu_time_max, _gpu_time);

    _frame_count++;
    _time_acc += delta_time;

    _cpu_time_acc += _cpu_time;
    _gpu_time_acc += _gpu_time;

    if (_time_acc > refresh_interval)
    {
        // Update CPU Timing Data
        _cpu_time_avg = _cpu_time_acc / _frame_count;
        if (draw_time_plot)
        {
            memmove(&cpu_time_data[0], &cpu_time_data[1], (res - 1) * sizeof(float));
            cpu_time_data[res - 1] = static_cast<float>(_cpu_time_avg);
        }
        _cpu_time_acc = 0;


        // Update GPU Timing Data
        _gpu_time_avg = _gpu_time_acc / _frame_count;
        if (draw_time_plot)
        {
            memmove(&gpu_time_data[0], &gpu_time_data[1], (res - 1) * sizeof(float));
            gpu_time_data[res - 1] = static_cast<float>(_gpu_time_avg * 1e-6);
        }
        _gpu_time_acc = 0;



        _time_acc = 0;
        _frame_count = 0;
    }

    //   ImGui::InputInt("Rendered Objects:", &_rendered_objects, 0, 0,
    //      ImGuiInputTextFlags_ReadOnly);
    ImGui::Checkbox("Draw Time plots", &draw_time_plot);
    ImGui::SliderScalar("Refresh Interval", ImGuiDataType_Double,
        &refresh_interval, &refresh_interval_min,
        &refresh_interval_max, "%.1f s");

    ImGui::InputDouble("CPU Time (ms)", &_cpu_time, 0, 0, "%.2f",
        ImGuiInputTextFlags_ReadOnly);
    ImGui::InputDouble("CPU Time Min (ms)", &_cpu_time_min, 0, 0, "%.2f",
        ImGuiInputTextFlags_ReadOnly);
    ImGui::InputDouble("CPU Time Max (ms)", &_cpu_time_max, 0, 0, "%.2f",
        ImGuiInputTextFlags_ReadOnly);
    ImGui::InputDouble("CPU Time Avg (ms)", &_cpu_time_avg, 0, 0, "%.2f",
        ImGuiInputTextFlags_ReadOnly);
    if (draw_time_plot)
    {
        ImGui::InputFloat("CPU Histogram Max", &cpu_histogram_max);


        if (cpu_histogram_max < 0) cpu_histogram_max = FLT_MAX;
        ImGui::Text("Avg CPU Time %.4f within %.1fs", _cpu_time_avg,
            refresh_interval);
        ImGui::PlotHistogram("CPU Times", cpu_time_data,
            IM_ARRAYSIZE(cpu_time_data), 0, nullptr, 0,
            cpu_histogram_max,
            ImVec2(0, 80));
    }

    ImGui::Checkbox("Show GPU Time in ms", &show_gpu_ms);
    if (show_gpu_ms)
    {
        double gpu_time_min_ms = _gpu_time_min * 1e-6;
        double gpu_time_max_ms = _gpu_time_max * 1e-6;
        double gpu_time_avg_ms = _gpu_time_avg * 1e-6;
        ImGui::InputDouble("GPU Time (ms)", &gpu_time_ms, 0, 0, "%.2f",
            ImGuiInputTextFlags_ReadOnly);
        ImGui::InputDouble("GPU Time Min (ms)", &gpu_time_min_ms, 0, 0, "%.2f",
            ImGuiInputTextFlags_ReadOnly);
        ImGui::InputDouble("GPU Time Max (ms)", &gpu_time_max_ms, 0, 0, "%.2f",
            ImGuiInputTextFlags_ReadOnly);
        ImGui::InputDouble("GPU Time Avg (ms)", &gpu_time_avg_ms, 0, 0, "%.2f",
            ImGuiInputTextFlags_ReadOnly);
    }
    else
    {
        ImGui::InputScalar("GPU Time (ns)", ImGuiDataType_U64, &_gpu_time, 0, 0, 0,
            ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("GPU Time Min (ns)", ImGuiDataType_U64, &_gpu_time_min,
            0, 0, 0, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("GPU Time Max (ns)", ImGuiDataType_U64, &_gpu_time_max,
            0, 0, 0, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputScalar("GPU Time Avg (ns)", ImGuiDataType_U64, &_gpu_time_avg,
            0, 0, 0, ImGuiInputTextFlags_ReadOnly);
    }

    if (draw_time_plot)
    {
        ImGui::InputFloat("GPU Histogram Max", &gpu_histogram_max);
        if (gpu_histogram_max < 0) gpu_histogram_max = FLT_MAX;
        ImGui::Text("Avg GPU Time %.4f within %.1fs", _gpu_time_avg * 1e-6,
            refresh_interval);
        ImGui::PlotHistogram("GPU Times", gpu_time_data,
            IM_ARRAYSIZE(gpu_time_data), 0, nullptr, 0,
            gpu_histogram_max,
            ImVec2(0, 80));
    }


}


void statUI(MyGlWindow* win)
{
    bool gl_stats_window = true;
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (gl_stats_window)
    {
        static int frame_count = 0;
        static float dt = 0.0f;
        static float fps = 0.0f;
        frame_count++;
        dt += _delta_time;

        if (dt > 1.0f)
        {
            fps = frame_count / dt;
            frame_count = 0;
            dt -= 1.0f;

        }


        float frame_time_ms = 1000.0f * static_cast<float>(_delta_time);

        ImGui::Begin("Stats", &gl_stats_window);
        ImGui::Text("General Stats:");
        ImGui::InputFloat("Frame Time (ms)", &frame_time_ms, 0, 0, "%.2f",
            ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("FPS", &fps, 0, 0, "%.1f",
            ImGuiInputTextFlags_ReadOnly);
        ImGui::Separator();
        ImGui::Text("Scene Stats:");

        drawStatsImGui(win, _frame_start_time, _delta_time);



        ImGui::End();
    }
}



int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
    {

    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    int width = 800;
    int height = 800;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "OpenGL FrameWork", NULL, NULL);


    if (!window)
    {
        glfwTerminate();
        return -1;
    }



    glfwMakeContextCurrent(window);


    /* Make the window's context current */

    if (gl3wInit()) {
        fprintf(stderr, "failed to initialize OpenGL\n");
        return -1;
    }


    if (!gl3wIsSupported(4, 3)) {
        fprintf(stderr, "OpenGL 4.5 not supported\n");
        return -1;
    }

    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
        glGetString(GL_SHADING_LANGUAGE_VERSION));


    glfwSwapInterval(0);  //enable vsync


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

     // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    win = new MyGlWindow(width, height);


    // Timer Queries
    glGenQueries(2, _gpu_timer_query);
    // Dummy query
    glBeginQuery(GL_TIME_ELAPSED, _gpu_timer_query[_swap_id.pong()]);
    glEndQuery(GL_TIME_ELAPSED);




    bool show_test_window = true;
    bool show_another_window = false;


    double previousTime = glfwGetTime();
    int frameCount = 0;
    double lastTime;



    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetKeyCallback(window, key_callback);


    glfwSetWindowTitle(window, "myGlWindow");


    _frame_start_time = glfwGetTime();

    float startFrame = glfwGetTime();


    win->changeCamera();





    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {



        // Rendering
        float currentFrame = glfwGetTime();
        //deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        animationTime = (currentFrame - startFrame);



        glClearColor(0.2f, 0.2f, .2f, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glEnable(GL_DEPTH_TEST);
        glEnable(GL_PROGRAM_POINT_SIZE);


        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);




        _delta_time = glfwGetTime() - _frame_start_time;
        _frame_start_time = glfwGetTime();


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        statUI(win);


        _cpu_time = glfwGetTime();
        glBeginQuery(GL_TIME_ELAPSED, _gpu_timer_query[_swap_id.ping()]);

        win->draw(animationTime);

        glEndQuery(GL_TIME_ELAPSED);
        _cpu_time = (glfwGetTime() - _cpu_time) * 1000.0;
        glGetQueryObjectui64v(_gpu_timer_query[_swap_id.pong()], GL_QUERY_RESULT, &_gpu_time);
        _swap_id.swap();


        ImGui::Render();
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        //glfwWaitEvents();


    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

