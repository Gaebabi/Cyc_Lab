

#include "MyGlWindow.h"



#include <iostream>
#include "drawUtils.h"


#include "timing.h"


static double DEFAULT_VIEW_POINT[3] = {30, 30, 30};
static double DEFAULT_VIEW_CENTER[3] = {0, 0, 0};
static double DEFAULT_UP_VECTOR[3] = {0, 1, 0};
glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);

MyGlWindow::MyGlWindow(int x, int y, int w, int h) :
  Fl_Gl_Window(x,y,w,h)
//==========================================================================
{
    
    mode( FL_RGB|FL_ALPHA|FL_DOUBLE | FL_STENCIL );
 	
	fieldOfView = 45;

	glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
	glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

	float aspect = (w / (float)h);
	m_viewer = new Viewer(viewPoint, viewCenter, upVector, 45.0f, aspect);

	// 공 생성 ( 벡터에 add )
	for (int i = 0; i < 500; i++) {
		m_mover.push_back(new Mover());
	}


	TimingData::init();
	run = 0;

}


  void MyGlWindow::setupLight(float x, float y, float z)
  {
	  
	  // set up the lighting
	  GLfloat lightPosition[] = {500, 900.0, 500, 1.0};
	  GLfloat lightPosition2[] = {1, 0, 0, 0};
	  GLfloat lightPosition3[] = {0, -1, 0, 0};

	  GLfloat violetLight[] = {0.5f, 0.1f, .5f, 1.0};
	  GLfloat whiteLight[] = {1, 1, 1, 1.0};
	  GLfloat whiteLight2[] = { .3, .3, .3, 1.0 };
	  GLfloat blueLight[] = {.1f,.1f,.3f,1.0};


	  glEnable(GL_COLOR_MATERIAL);
	  glEnable(GL_DEPTH_TEST);
	  glEnable(GL_LIGHTING);
	  glEnable(GL_LIGHT0);
	  glEnable(GL_LIGHT1);
	  glEnable(GL_LIGHT2);
	  glEnable(GL_DEPTH);

	  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);


	  lightPosition[0] = x;
	  lightPosition[1] = y;
      lightPosition[2] = z;

	  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	  glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);

 	  glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
 	  glLightfv(GL_LIGHT1, GL_AMBIENT, whiteLight2);
// 
	  glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	  glLightfv(GL_LIGHT2, GL_DIFFUSE, whiteLight);  

	  	
	  glEnable (GL_BLEND); 
	  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  }





void MyGlWindow::drawStuff()
{
   polygonf( 4, 20., 0.,-25.,  20., 0., 25.,  20., 30., 25.,  20., 30., -25.);
}

//==========================================================================
void MyGlWindow::draw()
//==========================================================================
{

  glViewport(0,0,w(),h());

	// clear the window, be sure to clear the Z-Buffer too
  glClearColor(0.0f,0.0f,.0f,0);		// background should be blue


  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glEnable(GL_DEPTH);

  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  
  
  // now draw the ground plane
  setProjection();
  setupFloor();

  glPushMatrix();
	//drawFloor(200,20);
  glPopMatrix();


  setupLight(m_viewer->getViewPoint().x,m_viewer->getViewPoint().y,m_viewer->getViewPoint().z);

    
 // Add a sphere to the scene.
//Draw axises
  glLineWidth(3.0f);
  glBegin(GL_LINES);
  glColor3f(1,0,0);

  glVertex3f(0,0.1,0);
  glVertex3f(0,100,0);

  glColor3f(0,1,0);

  glVertex3f(0,0.1,0);
  glVertex3f(100,0.1,0);

  glColor3f(0,0,1);

  glVertex3f(0,0.1,0);
  glVertex3f(0,0.1,100);
  glEnd();
  glLineWidth(1.0f);




  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);


  glEnable(GL_LIGHTING);
  glDisable(GL_BLEND);

  //draw objects
  glPushMatrix();
  for (unsigned int i = 0; i < m_mover.size(); i++) {
	  if (m_mover[i]->active)
		  m_mover[i]->draw();
  }
  glPopMatrix();

  

 glEnable(GL_COLOR_MATERIAL);

}

void MyGlWindow::test()
{
	
}

// 중력 상수 G
const double G = 9.8;
void MyGlWindow::update(int updateRate)
{
	TimingData::get().update();
	if (!run) return;
	
	unsigned int i = 0, j = 0, size = m_mover.size();
	double distance, F;
	unsigned int rep;
	/* 배속 START */
	for (rep = 0; rep < updateRate; ++rep) {

		/* 인력 START */
			// 모든 공 루프 (자신)
			for (i = 0; i < size; ++i) {
				// 활성화 상태일 경우
				if (m_mover[i]->active) {
					glm::f64vec3 _velocity(0, 0, 0);
					// 모든 공 루프 (상대)
					for (j = 0; j < size; ++j) {
						if (i != j && m_mover[j]->active) { // 자신이 아닐경우 & 상대가 활성화 상태인지
							cyclone::Vector3 _vec = m_mover[j]->m_position - m_mover[i]->m_position;

							glm::f64vec3 _dir(_vec.x, _vec.y, _vec.z);
							// 거리 구하기
							distance = glm::length(_dir);

							// F = G*(m1*m2/r^2)
							F = G * ((m_mover[i]->ballMass) * (m_mover[j]->ballMass) / (distance * distance));

							F = F * 0.001; // 정밀도
							// 방향 * F
							_velocity += (glm::normalize(_dir) * F);
						}
					}
					// 인력 적용
					m_mover[i]->velocity += glm::f64vec3(_velocity.x, _velocity.y, _velocity.z);
				}
			}
		/* 인력 END */

		/* velocity 적용 START */
			for (i = 0; i < size; ++i) {
				if (m_mover[i]->active) // 활성화 상태인지
					m_mover[i]->update(); // velocity 적용
			}
		/* velocity 적용 END */


		/* 충돌 체크 START */
			// 모든	 공 루프 (자신)
			for (i = 0; i < size; ++i) {
				// 활성화 상태일 경우
				if (m_mover[i]->active) {
					// 모든 공 루프 (상대)
					for (j = 0; j < size; ++j) {
						if (i != j && m_mover[j]->active) { // 자신이 아닐경우 & 상대가 활성화 상태인지

							if (m_mover[i]->ballMass >= m_mover[j]->ballMass) {
								cyclone::Vector3 _vec = m_mover[j]->m_position - m_mover[i]->m_position;

								glm::f64vec3 _distance(_vec.x, _vec.y, _vec.z);
								// 거리 구하기
								distance = glm::length(_distance);

								// 충돌 시 합치기
								if (distance < m_mover[i]->ballSize + m_mover[j]->ballSize) {
									m_mover[j]->velocity *= 0.5;
									m_mover[i]->velocity += m_mover[j]->velocity;
									//m_mover[i]->ballMass += m_mover[j]->ballMass;
									//m_mover[i]->m_position = (m_mover[i]->m_position + m_mover[j]->m_position) * 0.5; // 위치 보간

									// 상대 비활성화
									//m_mover[j]->active = false;

									// 다시 루프 시작
									i = -1;
									break;
								}
							}

						}
					}
				}
			}
		/* 충돌 체크 END */

	}
	/* 배속 END */


	// 제일 큰 행성 포커스
	if (updateRate == 1) {
		unsigned int maxInd = 0;
		double maxSize = 0;
		for (unsigned int i = 0; i < m_mover.size(); i++) {
			if (m_mover[i]->ballMass > maxSize) {
				maxSize = m_mover[i]->ballMass;
				maxInd = i;
			}
		}
		//m_viewer->m_viewPoint = glm::vec3(m_mover[maxInd]->m_position.x, m_mover[maxInd]->m_position.y, m_mover[maxInd]->m_position.z);
		m_viewer->centerAt(glm::vec3(m_mover[maxInd]->m_position.x, m_mover[maxInd]->m_position.y, m_mover[maxInd]->m_position.z));
	}

	/*
	// doPick() 구현중...
	for (unsigned int i = 0; i < m_mover.size(); i++) {
		glLoadName(i + 1);
		m_mover[i]->draw();
	}
	glBegin(GL_LINE_STRIP);
	for (unsigned int i = 0; i < m_mover.size(); i++) {
		cyclone::Vector3 p = m_mover[i]->m_position;
		glVertex3f(p.x, p.y, p.z);
	}
	glEnd();
	// doPick() 구현중...
	*/
}


void MyGlWindow::doPick()
{
	make_current();		// since we'll need to do some GL stuff

	int mx = Fl::event_x(); // where is the mouse?
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	// set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((double)mx, (double)(viewport[3] - my), 5, 5, viewport);

	// now set up the projection
	setProjection(false);

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100, buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selected = buf[3] - 1;
	}
	else {// nothing hit, nothing selected
		selected = -1;
	}
	//printf("Selected Cube %d\n", selectedCube);
}


void MyGlWindow::setProjection(int clearProjection)
//==========================================================================
{
  glMatrixMode(GL_PROJECTION);
  glViewport(0,0,w(),h());
  if (clearProjection)
	glLoadIdentity();
  // compute the aspect ratio so we don't distort things
  double aspect = ((double) w()) / ((double) h());
  gluPerspective(fieldOfView, aspect, 1, 100000);

  // put the camera where we want it to be
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // use the transformation in the ArcBall

  gluLookAt( 
	  m_viewer->getViewPoint().x, m_viewer->getViewPoint().y, m_viewer->getViewPoint().z, 
	  m_viewer->getViewCenter().x, m_viewer->getViewCenter().y, m_viewer->getViewCenter().z, 
	  m_viewer->getUpVector().x, m_viewer->getUpVector().y, m_viewer->getUpVector().z
	  );
  
  glDisable (GL_BLEND); 
}

static int last_push;
int m_pressedMouseButton;
int m_lastMouseX;
int m_lastMouseY;

int MyGlWindow::handle(int e)
//==========================================================================
{

	switch (e) {
	case FL_SHOW:		// you must handle this, or not be seen!
		show();
		return 1;
	case FL_PUSH:
	{

		m_pressedMouseButton = Fl::event_button();
		m_lastMouseX = Fl::event_x();
		m_lastMouseY = Fl::event_y();

		if (m_pressedMouseButton == 1) {
			doPick();

			if (selected >= 0) {
				std::cout << "picked" << std::endl;
			}
			damage(1);
			return 1;
		};
		break;


	}
	damage(1);
	return 1;
	case FL_RELEASE:
		m_pressedMouseButton = -1;
		damage(1);

		return 1;
	case FL_DRAG: // if the user drags the mouse
	{
	  if (selected >= 0 && m_pressedMouseButton == 1) {



		  double r1x, r1y, r1z, r2x, r2y, r2z;
		  getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

		  double rx, ry, rz;
		  mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z,
			  static_cast<double>(m_mover[selected]->m_position.x),
			  static_cast<double>(m_mover[selected]->m_position.y),
			  static_cast<double>(m_mover[selected]->m_position.z),
			  rx, ry, rz,
			  (Fl::event_state() & FL_CTRL) != 0);

				m_mover[selected]->m_position = glm::f64vec3(rx, ry, rz);

		  damage(1);
	  }
	  else {


		  double fractionChangeX = static_cast<double>(Fl::event_x() - m_lastMouseX) / static_cast<double>(this->w());
		  double fractionChangeY = static_cast<double>(m_lastMouseY - Fl::event_y()) / static_cast<double>(this->h());

		  if (m_pressedMouseButton == 1) {
			  m_viewer->rotate(fractionChangeX, fractionChangeY);
		  }
		  else if (m_pressedMouseButton == 2) {
			  m_viewer->zoom(fractionChangeY);
		  }
		  else if (m_pressedMouseButton == 3) {
			  m_viewer->translate(-fractionChangeX, -fractionChangeY, (Fl::event_key(FL_Shift_L) == 0) || (Fl::event_key(FL_Shift_R) == 0));
		  }
		  else {
			  std::cout << "Warning: dragging with unknown mouse button!  Nothing will be done" << std::endl;
		  }

		  m_lastMouseX = Fl::event_x();
		  m_lastMouseY = Fl::event_y();
		  redraw();
	  }
  }

  return 1;
  
  case FL_KEYBOARD:
	  return 0;

  default:
	return 0;
  }
}


//
// get the mouse in NDC
//==========================================================================
void MyGlWindow::getMouseNDC(float& x, float& y)
//==========================================================================
{
	// notice, we put everything into doubles so we can do the math
	  float mx = (float) Fl::event_x();	// remeber where the mouse went down
	  float my = (float) Fl::event_y();

	  // we will assume that the viewport is the same as the window size
	  float wd = (float) w();
	  float hd = (float) h();

	  // remember that FlTk has Y going the wrong way!
	  my = hd-my;
	
	  x = (mx / wd) * 2.0f - 1.f;
	  y = (my / hd) * 2.0f - 1.f;
}

