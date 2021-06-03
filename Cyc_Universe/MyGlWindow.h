
#include <FL/Fl_Gl_Window.h>
#include <Fl/Fl.h>
#include <Fl/Fl_Value_Slider.H>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <Fl/Fl_Double_Window.h>

#include "stdio.h"
#include "math.h"
#include "3DUtils.h"

#include "Vec3f.h"
#include "Viewer.h"
#include <vector>

#include "core.h"
#include "Mover.h"

class MyGlWindow : public Fl_Gl_Window {
	public:
		MyGlWindow(int x, int y, int w, int h);
		

		std::vector<Vec3f *> history;
		std::vector<Mover*> m_mover; // º¤ÅÍ ( Mover ¹è¿­ )
		

		Fl_Slider* time;	
		int run;
		void update(int updateRate);
		void drawStuff();
		void doPick();
		void BtnClear();

		bool drawLine = false;
		int focus = 0;
		int selected = -1;
		unsigned int totalTick = 0;

		glm::f64vec3 centerVelocity;

	private:
		void draw();					// standard FlTk
		
		
		int handle(int);				// standard FlTk
		
		float fieldOfView;
		Viewer *m_viewer;

		void setProjection(int clearProjection = 1);
		void getMouseNDC(float& x, float& y);
		void setupLight(float x, float y, float z);
		void putText(char* string, int x, int y, float r, float g, float b);
};

