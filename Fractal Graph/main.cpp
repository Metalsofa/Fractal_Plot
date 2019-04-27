/* Main source file for Glimmer */

int fractal_type = 0;

#include <algorithm>
#include "freeglut32.h"
#include "gradients.h"
//The location on the computer where this session takes place (is a folder, not a file)
#include <string>
#include <stdlib.h>
#include <time.h>
#include <unordered_map>
#include <thread>
#include "complex.h"

long double VAR = 0.01;

long double xpan = 0.0;
long double ypan = 0.0;
long double zoom = 1.0;
long double defaultViewportSize = 10.0;

long double moddenom = 100.0;

int maxiterations = 64;

long double pointSize = 5.0;

long double speed = 0.08;

clong_double starting_point(0.0, 0.0);

unsigned int samplingResolution = 20000;

long double xmin = -2.0;
long double xmax = 1.0;
long double ymax = 1.0;
long double ymin = -1.0;

//Cyclic
const gradient rainbow = {
	{ 0.0 / 7.0, fgr::fcolor(0.0, 0.0, 0.0)},
	{ 1.0 / 7.0, fgr::fcolor(1.0, 0.0, 0.0)},
	{ 2.0 / 7.0, fgr::fcolor(1.0, 1.0, 0.0)},
	{ 3.0 / 7.0, fgr::fcolor(0.0, 1.0, 0.0)},
	{ 4.0 / 7.0, fgr::fcolor(0.0, 1.0, 1.0)},
	{ 5.0 / 7.0, fgr::fcolor(0.0, 0.0, 1.0)},
	{ 6.0 / 7.0, fgr::fcolor(1.0, 0.0, 1.0)},
	{ 7.0 / 7.0, fgr::fcolor(1.0, 0.0, 0.0)},
};

//Cyclic
const gradient twilight = {
	{ 0.0 / 6.0, fgr::fcolor(0.1f, 0.0, 0.3f)}, // Dark violet
	{ 1.0 / 6.0, fgr::fcolor(0.5f, 0.0, 0.8f)}, // Lighet violet
	{ 2.0 / 6.0, fgr::fcolor(0.6f, 0.6f, 0.8f)}, // Lighet blue
	{ 3.0 / 6.0, fgr::fcolor(0.8f, 0.8f, 0.8f)}, // lighet gtrey
	{ 4.0 / 6.0, fgr::fcolor(0.8f, 0.4f, 0.1f)}, // light bronze
	{ 5.0 / 6.0, fgr::fcolor(1.0, 0.5f, 0.5f)}, //putrply bronze
	{ 6.0 / 6.0, fgr::fcolor(0.1f, 0.0, 0.3f)}  // Dark violet
};

//Cyclic
const gradient cyanic = {
	{0.0 / 6.0, fgr::fcolor(0.0, 0.0, 0.0)},
	{1.0 / 6.0, fgr::fcolor(0.0, 0.0, 1.0)},
	{2.0 / 6.0, fgr::fcolor(0.0, 1.0, 1.0)},
	{3.0 / 6.0, fgr::fcolor(1.0, 1.0, 1.0)},
	{4.0 / 6.0, fgr::fcolor(0.0, 1.0, 1.0)},
	{5.0 / 6.0, fgr::fcolor(0.0, 1.0, 0.0)},
	{6.0 / 6.0, fgr::fcolor(0.0, 0.0, 0.0)}
};

//Cyclic
const gradient blood = {
	{0.0 / 3.0, fgr::fcolor(0.0, 0.0, 0.0)},
	{1.0 / 3.0, fgr::fcolor(0.4, 0.2, 0.0)},
	{2.0 / 3.0, fgr::fcolor(1.0, 1.0, 1.0)},
	{3.0 / 3.0, fgr::fcolor(0.0, 0.0, 0.0)}
};

//Cyclic
const gradient noir = {
	{0.0 / 5.0, fgr::fcolor(0.0, 0.0, 0.0)},
	{1.0 / 5.0, fgr::fcolor(0.4, 0.0, 0.0)},
	{2.0 / 5.0, fgr::fcolor(0.0, 0.0, 0.0)},
	{3.0 / 5.0, fgr::fcolor(1.0, 1.0, 1.0)},
	{4.0 / 5.0, fgr::fcolor(0.0, 0.0, 0.0)},
	{5.0 / 5.0, fgr::fcolor(1.0, 1.0, 1.0)}
};

const gradient& getColorScheme(int i) {
	switch (i % 5) {
	case 0:
		return rainbow;
	case 1:
		return twilight;
	case 2:
		return cyanic;
	case 3:
		return blood;
	case 4:
		return noir;
	}
}

int currentscheme = 0;

bool samplerender = true;

//Some globals
int windowHeight = 10;
int windowWidth = 10;

bool ctrlDown = false;

//Remember numbers that ARE part of the set
std::unordered_map<long double, std::vector<long double>> knownmembers;

//The gradients available to this session
std::vector<gradient> gradientSet = {
	rainbow, twilight, cyanic, blood, noir
};

//Contains pre-compiled GPU instructions for changing to the right color given a particular integer
std::vector<std::vector<GLuint>> compiled_gradients;


void ClearScreen() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }/* Main source file for Glimmer */

//Callback for when the window changes size
void changeSize(int width, int height) {
	windowHeight = height;
	windowWidth = width;
	//To avoid divide by zero:
	if (height == 0)
		height = 1;
	long double ratio = 1.0 * width / height;
	//Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);
	//Reset Matrix
	glLoadIdentity();
	//Multiply the current matrix by one that makes the view orthographic
	//gluOrtho2D((xpan - defaultViewportSize * ratio) / zoom, (xpan + defaultViewportSize * ratio) / zoom, ((ypan - defaultViewportSize) / zoom), ((ypan + defaultViewportSize) / zoom));
	//gluOrtho2D(0, windowWidth, 0, windowHeight);
	glOrtho(0.0, windowWidth, windowHeight, 0, -1.0, 1.0);
	//Set viewport to be the entire window
	glViewport(0, 0, width, height);
	//Get back to the modelview
	glMatrixMode(GL_MODELVIEW);
}

//Returns a random long double between two paramaters (thanks to https://stackoverflow.com/questions/686353/random-long double-number-generation)
long double randomFloat(long double lb, long double rb) {
	return lb + static_cast <long double> (rand()) / (static_cast <long double> (RAND_MAX / (rb - lb)));
}

void recompile_gradients() {
	compiled_gradients.resize(gradientSet.size());
	for (unsigned int i = 0; i < gradientSet.size(); ++i) {
		compiled_gradients[i].resize(maxiterations);
		for (unsigned int j = 0; j < maxiterations; ++j) {
			glDeleteLists(compiled_gradients[i][j], 1);
			compiled_gradients[i][j] = glGenLists(1);
			glNewList(compiled_gradients[i][j], GL_COMPILE);
				fgr::setcolor(mapgradient(fmodl(long double(j), moddenom) / moddenom, gradientSet[i]));
			glEndList();
		}
	}
}

 /* Initialize OpenGL Graphics */
void initGL() {
	// Set "clearing" or background color
	glClearColor(0.0, 0.0, 0.0, 0.0); // Black and opaque
	//Set up compiled gradients
	//compiled_gradients.resize(gradientSet.size());
	//for (unsigned int i = 0; i < gradientSet.size(); ++i) {
	//	compiled_gradients[i].resize(maxiterations);
	//	for (unsigned int j = 0; j < maxiterations; ++j) {
	//		compiled_gradients[i][j] = glGenLists(1);
	//	}
	//}
	//Initialize the gradients
	recompile_gradients();
}

//Test function for complex plotting: the mandelbrot set for a given c
//clong_double testfunc(clong_double z) { 
//	clong_double param(complexParameter.imaginary * cosf(complexParameter.real), complexParameter.imaginary * sinf(complexParameter.real));
//	return z * z + param;
//}


//Contains all gl-code; there should be no need to have any outside of this function
void renderScene(void) {
	//Screen-cleanup
	// Clear Color and Depth Buffers
	// Reset transformations
	glLoadIdentity();
	//gluOrtho2D(0, windowWidth, 0, windowHeight);

	///////////////////////// DRAWING INSTRUCTIONS ////////////////////////////
	glColor3f(1.0, 0.0, 1.0);
	clong_double dot;
	//Success and iterative depth
	std::pair<bool, int> eval;
	long double ratio = 1.0 * windowWidth / windowHeight;
	long double threshold = 2.0;
	if (!samplerender) {
		//ClearScreen();
		glPointSize(1.0f);
		glBegin(GL_POINTS);
		for (unsigned int i = 0; i < windowHeight; ++i) {
			dot.imaginary = (long double(i) / long double(windowHeight)) * (ymax - ymin) + ymin; //* (((ypan - defaultViewportSize) / zoom), ((ypan + defaultViewportSize) / zoom));
			//auto data_exists = knownmembers.find(dot.imaginary);
			//int prev = -1;
			for (unsigned int j = 0; j < windowWidth; ++j) {
				dot.real = (long double(j) / long double(windowWidth)) * (xmax - xmin) + xmin; //* ((xpan - defaultViewportSize * ratio) / zoom, (xpan + defaultViewportSize * ratio) / zoom);
				//if (data_exists != knownmembers.end()) {
				//	for (unsigned int q = 0; q < data_exists->second.size(); ++q) {
				//		//On even-numbered cells (0 being odd and 1 being even),
				//		if (q % 2) {
				//			if (data_exists->second[q - 1] < dot.real && dot.real < data_exists->second[q]) {
				//				eval.first = false;
				//				break;
				//				//Technically also eval.second = 0
				//			}
				//		}
				//	}
				//}
				//else
					eval = mandelbrot(starting_point, dot, threshold, maxiterations);
				if (eval.first) {
					//fgr::setcolor(mapgradient(long double(eval.second) / long double(maxiterations), cyanic));
					//fgr::setcolor(mapgradient(fmodl(long double(eval.second), 100.0l) / 100.0l, gradientSet[currentscheme]));
					glCallList(compiled_gradients[currentscheme][eval.second]);
					glVertex2i(j, i);
				}
				//if (prev != -1) {
				//	if (eval.second == 0) {
				//		if (prev != 0) {
				//			knownmembers[dot.imaginary].push_back(dot.real);
				//		}
				//	}
				//	else {
				//		if (prev == 0) {
				//			knownmembers[dot.imaginary].push_back(dot.real);
				//		}
				//	}
				//}
				//prev = eval.second;
			}
			//if (i * 64 % windowHeight == 0) {
				glEnd();
				glutSwapBuffers();
				glBegin(GL_POINTS);
			//}
		}
	}
	else {
		glAlphaFunc(GL_NOTEQUAL, 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(pointSize);
		glBegin(GL_POINTS);
		for (unsigned int s = 0; s < samplingResolution; ++s) {
			long double i = randomFloat(0.0, windowHeight);
			long double j = randomFloat(0.0, windowWidth);
			//dot.real = randomFloat((xpan - defaultViewportSize * ratio) / zoom, (xpan + defaultViewportSize * ratio) / zoom);
			//dot.imaginary = randomFloat(((ypan - defaultViewportSize) / zoom), ((ypan + defaultViewportSize) / zoom));
			dot.real = (long double(j) / long double(windowWidth)) * (xmax - xmin) + xmin; //* ((xpan - defaultViewportSize * ratio) / zoom, (xpan + defaultViewportSize * ratio) / zoom);
			dot.imaginary = (long double(i) / long double(windowHeight)) * (ymax - ymin) + ymin; //* (((ypan - defaultViewportSize) / zoom), ((ypan + defaultViewportSize) / zoom));
			eval = mandelbrot(starting_point, dot, threshold, maxiterations);
			if (eval.first) {
				//fgr::setcolor(mapgradient(long double(eval.second) / long double(maxiterations), cyanic));
				//fgr::setcolor(mapgradient(fmodl(long double(eval.second), 100.0l) / 100.0l, getColorScheme(currentscheme)));
				glCallList(compiled_gradients[currentscheme][eval.second]);
				glVertex2f(j, i);
			}
			//if (s * 4 % samplingResolution == 0) {
			//	glEnd();
			//	glutSwapBuffers();
			//	glBegin(GL_POINTS);
			//}
		}
		glDisable(GL_BLEND);
		glDisable(GL_POINT_SMOOTH);
	}
	glEnd();

	//This is the function that refreshes the canvas and implements everything we've 'drawn'
	glutSwapBuffers();
}

//Mouse click handling
void MouseClick(int button, int state, int x, int y) {
	int mod = glutGetModifiers();
	switch (button) {
	case 3:
		if (mod == GLUT_ACTIVE_CTRL) {
			if (!state) {
				zoom *= 1.15f;
				xpan *= 1.15f;
				ypan *= 1.15f;
			}
			break;
		}
		else {
			if (!state)
				if (mod == GLUT_ACTIVE_SHIFT)
					xpan += 0.5f;
				else
					ypan += 0.5f;
			break;
		}
	case 4:
		if (mod == GLUT_ACTIVE_CTRL) {
			if (!state)
				zoom /= 1.15f;
				xpan /= 1.15f;
				ypan /= 1.15f;
			break;
		}
		else {
			if (!state)
				if (mod == GLUT_ACTIVE_SHIFT)
					xpan -= 0.5f;
				else
					ypan -= 0.5f;
			break;
		}
	default:
		return;
	}
	changeSize(windowWidth, windowHeight);
	renderScene();
	return;
}

void ProcessSpecialKeys(int key, int x, int y) {
	long double xdiff = xmax - xmin;
	long double ydiff = ymax - ymin;
	switch (key) {
	case GLUT_KEY_UP:
		ymin -= ydiff / 4.0 * speed;
		ymax -= ydiff / 4.0 * speed;
		break;
	case GLUT_KEY_DOWN:
		ymin += ydiff / 4.0 * speed;
		ymax += ydiff / 4.0 * speed;
		break;
	case GLUT_KEY_RIGHT:
		xmin += xdiff / 4.0 * speed;
		xmax += xdiff / 4.0 * speed;
		break;
	case GLUT_KEY_LEFT:
		xmin -= xdiff / 4.0 * speed;
		xmax -= xdiff / 4.0 * speed;
		break;
	}
	ClearScreen();
	renderScene();
	return;
}

void processNormalKeys(unsigned char key, int x, int y) {
	long double zc = 0.75f;
	//Reset the complext parameter
	switch (key) {
	case '0':
		starting_point = clong_double();
		ClearScreen();
		break;
	case 'm':
		samplingResolution *= 2;
		break;
	case 'n':
		samplingResolution /= 2;
		ClearScreen();
		break;
	case 'w': {
		long double xdiff = xmax - xmin;
		long double ydiff = ymax - ymin;
		long double old = xmin;
		xmin = xmax - xdiff * zc;
		xmax = old + xdiff * zc;
		old = ymin;
		ymin = ymax - ydiff * zc;
		ymax = old + ydiff * zc;
	}
		ClearScreen();
		break;
	case 's': {
		long double xdiff = xmax - xmin;
		long double ydiff = ymax - ymin;
		long double old = xmin;
		xmin = xmax - xdiff / zc;
		xmax = old + xdiff / zc;
		old = ymin;
		ymin = ymax - ydiff / zc;
		ymax = old + ydiff / zc;
	}
		ClearScreen();
		break;
	case 'c':
		currentscheme++;
		currentscheme %= compiled_gradients.size();
		ClearScreen();
		break;
	case 'r':
		maxiterations *= 2;
		recompile_gradients();
		ClearScreen();
		break;
	case 'f':
		maxiterations /= 2;
		recompile_gradients();
		ClearScreen();
		break;
	case ' ':
		samplerender = !samplerender;
		ClearScreen();
		renderScene();
		samplerender = !samplerender;
		return;
	case 'q':
		exit(0);
	case 't':
		fractal_type++;
		ClearScreen();
		break;
	case 'k':
		starting_point.imaginary += speed;
		ClearScreen();
		break;
	case 'j':
		starting_point.imaginary -= speed;
		ClearScreen();
		break;
	case 'h':
		starting_point.real -= speed;
		ClearScreen();
		break;
	case 'l':
		starting_point.real += speed;
		ClearScreen();
		break;
	case 'p':
		speed *= 2.0;
		break;
	case 'o':
		speed /= 2.0;
		break;
	case 'z':
		pointSize -= 1.0;
		if (pointSize < 1.0)
			pointSize = 1.0;
		break;
	case 'x':
		pointSize += 1.0;
		break;
	}
	renderScene();
	return;
}

void ReleaseSpecialKeys(int key, int x, int y) {
	return;
}

//main function; exists to set up a few things and then enter the glut-main-loop
int main(int argc, char** argv) {
	//Initialize random
	srand(time(NULL));

	//Initialize GLUT
	glutInit(&argc, argv);

	////Choose some settings for our Window
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(900, 500);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	//Create the Window
	glutCreateWindow("Fractal grapher");
	//Maximize the window
	//glut32::maximizeWindow("GlimmerTitle");
	glutFullScreen();

	////Some settings
	//glutIgnoreKeyRepeat(1);
	////glutSetCursor(GLUT_CURSOR_NONE); //Hide the cursor

	//// Display callbacks
	glutDisplayFunc(renderScene); //Callback for when we refresh
	//glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize); //Callback for when window is resized

	//// Control callbacks
	glutKeyboardFunc(processNormalKeys); //Callback pressing a "normal" key
	glutSpecialFunc(ProcessSpecialKeys); //Callback for a "special" key
	glutSpecialUpFunc(ReleaseSpecialKeys); //Callback for releasing special keys
	//glutKeyboardUpFunc(releaseNormalKeys); //Callback for releasing "normal" keys
	glutMouseFunc(MouseClick); //Callback for mouse clicks
	//glutMotionFunc(ActiveMouseMove); //Callback for mouse movement with button down
	//glutPassiveMotionFunc(PassiveMouseMove); //Callback for mouse movement with no button down

	//initGL();
	//
	////enter GLUT event processing cycle
	//glutMainLoop();

	//glutInit(&argc, argv);          // Initialize GLUT
	//glutCreateWindow("Vertex, Primitive & Color");  // Create window with the given title
	//glutInitWindowSize(320, 320);   // Set the window's initial width & height
	//glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	//glutDisplayFunc(display);       // Register callback handler for window re-paint event
	initGL();                       // Our own OpenGL initialization
	glutMainLoop();                 // Enter the event-processing loop
	return 0;
}
