#include <iostream> // 순서 상관 없음
//#include <GL/glew.h>
#include <glad/glad.h>
//#include <glm/glm.hpp> // 순서 상관 없음

#include <GLFW/glfw3.h> // 순서가 바뀌면 안됨
#include "MyGlWindow.h"

#include "Gui.h"

GLFWwindow* window;
MyGlWindow* win;

bool lbutton_down;
bool rbutton_down;
bool mbutton_down;

bool W_key_down;
bool A_key_down;
bool S_key_down;
bool D_key_down;

double m_lastMouseX;
double m_lastMouseY;
double cx, cy;
static void key_callback(GLFWwindow*, int, int, int, int);
static void window_resize(GLFWwindow* window, int width, int height);
static void window_size_callback(GLFWwindow* window, int width, int height);
static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouseDragging(double width, double height);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float animationTime = 0.0f;
float currentFrame;

bool show_test_window = false;

/// /////////////////////////////////////////////////
void statUI(MyGlWindow* win);
void drawStatsImGui(MyGlWindow* win, const double& time, const double& delta_time);

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
/// /////////////////////////////////////////////////
int main() {
	// @------------------------------------------------------------------------------@
	// | Settings																	  |
	// @------------------------------------------------------------------------------@
#pragma region Settings
	// @------------------------------------------------------------------------------@
	// | Initialize the library														  |
	// | openGL 버전 설정 = MAJOR.MINOR = 4.3										  |
	// | CORE = 최신기능, COMPATIBLE = 모든기능										  |
	// @------------------------------------------------------------------------------@
	if (!glfwInit()) { 
		// 초기화 무조건 해야함.
		std::cout << "glfw init error" << std::endl;
		return 0;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



	// @------------------------------------------------------------------------------@
	// | Create a windowed mode window and its OpenGL context						  |
	// | glfw = 창, 이벤트															  |
	// @------------------------------------------------------------------------------@
	int width = 800;
	int height = 800;
	GLFWwindow* window = glfwCreateWindow(width, height, u8"나의 openGL윈도우", NULL, NULL); // NULL NULL = 창으로 열겠다. 다른 값 넣으면 풀스크린 가능 // 한글은 u8을 붙여줘야함.
	//glfwSetWindowTitle(window, u8"나의 openGL윈도우"); // 한글은 u8을 붙여줘야함.
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // 거대한 구조체 생성
	//////////////////////////////////////////////////////////
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glViewport(0, 0, 800, 600);
	//////////////////////////////////////////////////////////
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	const char* glsl_version = "#version 450";

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	ImGui::StyleColorsLight();
	//////////////////////////////////////////////////////////



	// @------------------------------------------------------------------------------@
	// | Callbacks																	  |
	// @------------------------------------------------------------------------------@
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, window_resize);
	glfwSetWindowSizeCallback(window, window_size_callback);

	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);



	// @------------------------------------------------------------------------------@
	// | Make the window's context current											  |
	// | glew = Helper																  |
	// @------------------------------------------------------------------------------@
	glfwSwapInterval(0);
	// enable vsync = 모니터 주사율과 동기화; glewExperimental = GL_TRUE; // 지워도 됨. 코어프로파일을 쓸때 TRUE 값 넣도록 만들었는데 현재는 자동으로 됨.
#pragma endregion



	// @------------------------------------------------------------------------------@
	// | Rendering																	  |
	// @------------------------------------------------------------------------------@
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
	win = new MyGlWindow(width, height);

	// Timer Queries
	glGenQueries(2, _gpu_timer_query);
	// Dummy query
	glBeginQuery(GL_TIME_ELAPSED, _gpu_timer_query[_swap_id.pong()]);
	glEndQuery(GL_TIME_ELAPSED);

	// MSAA 안티앨리어싱
	//glfwWindowHint(GLFW_SAMPLES, 8);

	float startFrame = glfwGetTime();

	//double fps_previousTime = glfwGetTime();
	//double fps_currentTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		//fps_currentTime = glfwGetTime();
		//deltaTime = fps_currentTime - fps_previousTime;
		glfwSetWindowTitle(window, std::to_string(deltaTime).c_str());
		// Rendering
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		animationTime = (currentFrame - startFrame);
		//////////////////////////////////////////////////////////
		// @------------------------------------------------------------------------------@
		// | imgui																		  |
		// @------------------------------------------------------------------------------@
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//adding widgets here
		//ImGui::ShowDemoWindow(&show_test_window);

		//////////////////////////////////////////////////////////
		Gui::Test();
		statUI(win);
		//////////////////////////////////////////////////////////



		_cpu_time = glfwGetTime();
		glBeginQuery(GL_TIME_ELAPSED, _gpu_timer_query[_swap_id.ping()]);

		win->draw(animationTime);

		glEndQuery(GL_TIME_ELAPSED);
		_cpu_time = (glfwGetTime() - _cpu_time) * 1000.0;
		glGetQueryObjectui64v(_gpu_timer_query[_swap_id.pong()], GL_QUERY_RESULT, &_gpu_time);
		_swap_id.swap();


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



		//////////////////////////////////////////////////////////
		glfwSwapBuffers(window); // Swap front and back buffers
		glfwPollEvents(); // Poll forand process events

		if (io.WantCaptureMouse == false) {
			mouseDragging(width, height);
		}


		/*float speed = 1000.0f;
		glm::vec3 direction = win->m_viewer->getViewDir();
		glm::vec3 right = win->m_viewer->getUpVector();
		// 앞으로
		if (W_key_down) {
			win->m_viewer->getViewPoint() += direction * deltaTime * speed;
		}
		// 뒤로
		if (S_key_down) {
			win->m_viewer->getViewPoint() -= direction * deltaTime * speed;
		}
		// 오른쪽
		if (D_key_down) {
			win->m_viewer->getViewPoint() += right * deltaTime * speed;
		}
		// 왼쪽
		if (A_key_down) {
			win->m_viewer->getViewPoint() -= right * deltaTime * speed;
		}*/


		//fps_previousTime = fps_currentTime;
	}

	// @------------------------------------------------------------------------------@
	// | Terminate																	  |
	// @------------------------------------------------------------------------------@
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}



// @------------------------------------------------------------------------------@
// | Callbacks																	  |
// @------------------------------------------------------------------------------@
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
		printf("r");

	else if (key == GLFW_KEY_W) {
		//std::cout << "GLFW_MOUSE_BUTTON_MIDDLE" << std::endl;
		if (GLFW_PRESS == action)
			W_key_down = true;
		else if (GLFW_RELEASE == action)
			W_key_down = false;
	}
	else if (key == GLFW_KEY_A) {
		//std::cout << "GLFW_MOUSE_BUTTON_MIDDLE" << std::endl;
		if (GLFW_PRESS == action)
			A_key_down = true;
		else if (GLFW_RELEASE == action)
			A_key_down = false;
	}
	else if (key == GLFW_KEY_S) {
		//std::cout << "GLFW_MOUSE_BUTTON_MIDDLE" << std::endl;
		if (GLFW_PRESS == action)
			S_key_down = true;
		else if (GLFW_RELEASE == action)
			S_key_down = false;
	}
	else if (key == GLFW_KEY_D) {
		//std::cout << "GLFW_MOUSE_BUTTON_MIDDLE" << std::endl;
		if (GLFW_PRESS == action)
			D_key_down = true;
		else if (GLFW_RELEASE == action)
			D_key_down = false;
	}
}
static void window_resize(GLFWwindow* window, int width, int height) {
	win->resize(width, height);
}

static void window_size_callback(GLFWwindow* window, int width, int height) {
	win->setSize(width, height);
	win->setAspect(width / (float)height);
}
static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	cx = xpos;
	cy = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		std::cout << "GLFW_PRESS" << std::endl;
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		m_lastMouseX = xpos;
		m_lastMouseY = ypos;

		win->mouse_button_click(xpos, ypos);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		//std::cout << "GLFW_MOUSE_BUTTON_LEFT" << std::endl;
		if (GLFW_PRESS == action)
			lbutton_down = true;
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		//std::cout << "GLFW_MOUSE_BUTTON_RIGHT" << std::endl;
		if (GLFW_PRESS == action)
			rbutton_down = true;
		else if (GLFW_RELEASE == action)
			rbutton_down = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		//std::cout << "GLFW_MOUSE_BUTTON_MIDDLE" << std::endl;
		if (GLFW_PRESS == action)
			mbutton_down = true;
		else if (GLFW_RELEASE == action)
			mbutton_down = false;
	}
}
void mouseDragging(double width, double height)
{
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
	m_lastMouseX = cx;
	m_lastMouseY = cy;
}


using namespace std;


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
		dt += deltaTime;

		if (dt > 1.0f)
		{
			fps = frame_count / dt;
			frame_count = 0;
			dt -= 1.0f;

		}


		float frame_time_ms = 1000.0f * static_cast<float>(deltaTime);

		ImGui::Begin("Stats", &gl_stats_window);
		ImGui::Text("General Stats:");
		ImGui::InputFloat("Frame Time (ms)", &frame_time_ms, 0, 0, "%.2f",
			ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat("FPS", &fps, 0, 0, "%.1f",
			ImGuiInputTextFlags_ReadOnly);
		ImGui::Separator();
		ImGui::Text("Scene Stats:");

		drawStatsImGui(win, currentFrame, deltaTime);



		ImGui::End();
	}
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