// the main routine makes the window, and then runs an even loop
// until the window is closed
#include <windows.h>
#include <iostream>
#include <ctime>

#include <FL/Fl_Gl_Window.h>
#include <Fl/Fl.h>
#include <Fl/Fl_Value_Slider.H>
#include <Fl/Fl_Button.h>
#include <Fl/Fl_Light_Button.h>
#include <Fl/Fl_Choice.h>
#include <FL/Fl_Box.h>

#include "MyGlWindow.h"


Fl_Group* widgets;


long lastRedraw;
int frameRate = 60;
int updateRate = 1;

void changeFrameCB(Fl_Widget * w, void* data)
{
	Fl_Choice * widget = (Fl_Choice *)w;
	int i = widget->value();
	const char * menu = widget->text(i);
	frameRate = atoi(menu);

	MyGlWindow * win = (MyGlWindow *)data;
	win->redraw();
}

void changeUpdateCB(Fl_Widget* w, void* data)
{
	Fl_Choice* widget = (Fl_Choice*)w;
	int i = widget->value();
	const char* menu = widget->text(i);
	updateRate = atoi(menu);

	MyGlWindow* win = (MyGlWindow*)data;
	win->redraw();
}


void idleCB(void* w)
{
	MyGlWindow * win = (MyGlWindow *)w;
	if (clock() - lastRedraw > CLOCKS_PER_SEC / frameRate) {
		lastRedraw = clock();
		win->update(updateRate);
	}
	win->redraw();
}


void Run(Fl_Widget* o, void*data)
{
	Fl_Button* b = (Fl_Button*)o; //캐스팅이 반드시 필요
	MyGlWindow * win = (MyGlWindow *)data;
	if (b->value())
		win->run = 1;
	else
		win->run = 0;
	win->damage(1);
}
void DrawLine(Fl_Widget* o, void*data)
{
	Fl_Button* b = (Fl_Button*)o; //캐스팅이 반드시 필요
	MyGlWindow* win = (MyGlWindow*)data;
	win->drawLine = !win->drawLine;
	win->damage(1);
}
void Clear(Fl_Widget* o, void* data)
{
	Fl_Button* b = (Fl_Button*)o; //캐스팅이 반드시 필요
	MyGlWindow* win = (MyGlWindow*)data;
	win->BtnClear();
	win->damage(1);
}
void Focus(Fl_Widget* w, void* data)
{
	Fl_Choice* widget = (Fl_Choice*)w;
	int i = widget->value();
	MyGlWindow* win = (MyGlWindow*)data;
	win->focus = i;
}

int main()
{
	Fl::scheme("plastic") ;// plastic
	int width = 800;
	int height = 800;
	Fl_Double_Window* wind = new Fl_Double_Window(100,100,width,height,"GL 3D FrameWork");

	wind->begin();		// put widgets inside of the window
	
	widgets = new Fl_Group(0, 0, 800, 800);
	widgets->begin();

	MyGlWindow* gl = new MyGlWindow(0, 0, width, height - 50);
	Fl::add_idle((void(*)(void*)) idleCB, gl);  //
	
	widgets->end();
	Fl_Group::current()->resizable(widgets);
	
	Fl_Choice* _changeFrameCB;
	_changeFrameCB = new Fl_Choice(100, height - 40, 50, 20, "FrameRate");
	_changeFrameCB->add("15");
	_changeFrameCB->add("30");
	_changeFrameCB->add("60");
	_changeFrameCB->value(2);
	_changeFrameCB->callback((Fl_Callback*)changeFrameCB, gl);

	Fl_Choice* _changeUpdateCB;
	_changeUpdateCB = new Fl_Choice(width - 200, height - 40, 50, 20, "UpdateRate");
	_changeUpdateCB->add("1");
	_changeUpdateCB->add("2");
	_changeUpdateCB->add("10");
	_changeUpdateCB->add("100");
	_changeUpdateCB->add("500");
	_changeUpdateCB->add("1000");
	_changeUpdateCB->add("2000");
	_changeUpdateCB->value(0);
	_changeUpdateCB->callback((Fl_Callback*)changeUpdateCB, gl);

	Fl_Light_Button* _Run = new Fl_Light_Button(width - 600, height - 40, 25, 20, "Run");
	_Run->callback(Run, gl);

	Fl_Light_Button* _DrawLine = new Fl_Light_Button(width - 500, height - 40, 50, 20, "Draw Line");
	_DrawLine->callback(DrawLine, gl);

	Fl_Button* _Clear = new Fl_Button(width - 450, height - 40, 25, 20, "Clear");
	_Clear->callback(Clear, gl);

	Fl_Choice* _Focus;
	_Focus = new Fl_Choice(width - 300, height - 40, 25, 20, "Focus");
	_Focus->add("Mouse");
	_Focus->add("Center");
	_Focus->add("Biggest Ball");
	_Focus->add("Biggest Ball Center");
	_Focus->value(0);
	_Focus->callback((Fl_Callback*)Focus, gl);

	wind->end();

	wind->show();	// this actually opens the window


	Fl::run();
	delete wind;

	return 1;
}

