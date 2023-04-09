#include <freeglut.h>
#include <vector>
#include <memory>
#include <string>

// COSC 3P98 - Computer Graphics - Assignment 3
// This program will generate a 3d particle fountain

// Alec Ames                 Student #: 6843577
// Julian Ellis Geronimo     Student #: 6756597
// Due Date: April 10th, 2023

// gl shape types
enum Shape { SPHERE, CUBE, TETRAHEDRON, TORUS, CONE, CYLINDER };

// fire modes
enum FireMode { CONTINUOUS, MANUAL, SINGLE };

// color
struct Color {
	float r, g, b;

	void randomize() {
		r = (rand() % 100) * 0.01f;
		g = (rand() % 100) * 0.01f;
		b = (rand() % 100) * 0.01f;
	}
};

// particle struct
struct Particle {
	float px, py, pz;               // position
	float dx, dy, dz;               // direction
	float speed;                    // speed
	float rx, ry, rz;               // rotation angles
	float rix, riy, riz;  			// rotation angle increments
	float scale;                    // scale
	Shape shape;        			// object shape type
	Color color;	                // color
	int state;                      // state: alive
	float age;                      // age: used if entities have a finite lifespan
	std::shared_ptr<Particle> next; // next Record Pointer
};

std::vector<std::shared_ptr<Particle>> particles;

const int WIDTH = 800;     			// window width
const int HEIGHT = 800;    			// window height
const float G = 0.08f; 				// gravity constant
const float MOON_G = 0.02f; 		// moon gravity constant
const int MAX_PARTICLES = 1000; 	// max particle count
const int MAX_AGE = 5; 				// max age of particle

const float floorSize = 30.0f; 		// size of floor
const float fountainSize = 15.0f; 	// size of fountain
bool showMenu = true; 				// show menu toggle

// params
bool randomSpeed = false; 			// random speed toggle
bool randomColor = false; 			// random color toggle
bool randomShape = false; 			// random shape toggle
Shape userShape = TETRAHEDRON; 		// user selected shape default
bool randomScale = false; 			// random scale toggle
bool randomRotation = true; 		// random rotation toggle
bool moonGravity = false; 			// moon gravity toggle
FireMode fireMode = CONTINUOUS; 	// fire mode default
bool fired = false; 				// firing toggle

// vars for camera control
float xr = 0.0f; 					// x plane rotation
float yr = 0.0f; 					// y plane rotation
int lx = 0; 						// last x position
int ly = 0; 						// last y position
bool clicked = false; 				// mouse clicked
float zoom = -180.0f; 				// zoom

// sets up lighting 
void setupLighting() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambientLight[] = { 0.25f, 0.2f, 0.2f, 1.0f };
	GLfloat lightPosition[] = { 50.0f, 50.0f, 50.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

// draws the floor 
void drawFloor() {
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-floorSize, 0.0f, -floorSize);
	glVertex3f(-floorSize, 0.0f, floorSize);
	glVertex3f(floorSize, 0.0f, floorSize);
	glVertex3f(floorSize, 0.0f, -floorSize);
	glEnd();
}

// draws the fountain that the particles will be emitted from
void drawFountain() {
	// fountain color (patina green)
	glColor3f(0.41, 0.52, 0.45);
	// cylinder base of fountain
	glPushMatrix();
	glTranslatef(0.0f, 3.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCylinder(fountainSize, 3.0f, 24, 1);
	glPopMatrix();

	// switch to blue
	glColor3f(0.13, 0.17, 0.34);
	// fountain water
	glPushMatrix();
	glTranslatef(0.0f, 4.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCylinder(fountainSize - 0.5f, 4.0f, 24, 1);
	glPopMatrix();
	

	glColor3f(0.41, 0.52, 0.45);
	// ridge
	glPushMatrix();
	glTranslatef(0.0f, 4.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidTorus(2.0f, fountainSize, 5, 24);
	glPopMatrix();

	// cone top of fountain
	glPushMatrix();
	glTranslatef(0.0f, 4.0f, 0.0f);
	glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
	glutSolidCone(2.0f, fountainSize - 2.0f, 10, 10);
	glPopMatrix();

	// other cone top of fountain
	glPushMatrix();
	glTranslatef(0.0f, fountainSize + 4.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(3.0f, 3.0f, 20, 20);
	glPopMatrix();
}

// creates a particle
void createParticle() {
	if (particles.size() >= MAX_PARTICLES) particles.erase(particles.begin());
	auto p = std::make_shared<Particle>();
	(*p).age = 0.0f;
	(*p).px = 0.0f;
	(*p).py = fountainSize + 4.0f;
	(*p).pz = 0.0f;
	(*p).dx = (rand() % 100 - 50) * 0.01f;
	(*p).dy = 1.0f + (rand() % 100) * 0.01f;
	(*p).dz = (rand() % 100 - 50) * 0.01f;
	(*p).rx = 0.0f;
	(*p).ry = 0.0f;
	(*p).rz = 0.0f;
	(*p).rix = randomRotation ? (0.01 * (rand() % 360) - 180) : 0.0f;
	(*p).riy = randomRotation ? (0.01 * (rand() % 360) - 180) : 0.0f;
	(*p).riz = randomRotation ? (0.01 * (rand() % 360) - 180) : 0.0f;
	(*p).scale = randomScale ? (rand() % 100) * 0.01f : 1.0f;
	(*p).shape = randomShape ? static_cast<Shape>(rand() % 6) : userShape;

	if (randomColor) {
		(*p).color.randomize();
	}
	else { // default to blue (water)
		(*p).color.r = 0.16f;
		(*p).color.g = 0.25f;
		(*p).color.b = 0.49f;
	}

	if (randomSpeed) {
		(*p).speed = (rand() % 100) * 0.01f;
	}
	else {
		(*p).speed = 1.0f;
	}

	particles.push_back(p);
}

// animates the particles
void updateParticles() {
	const float bounciness = 0.5f;
	const float bounceCutoff = 0.01f;

	for (auto& p : particles) {
		(*p).dx += 0.0f;
		(*p).dy -= moonGravity ? MOON_G : G;
		(*p).dz += 0.0f;

		(*p).px += (*p).dx * (*p).speed;
		(*p).py += (*p).dy * (*p).speed;
		(*p).pz += (*p).dz * (*p).speed;

		// checks if floor is under particle
		if (((*p).px < -floorSize || (*p).px > floorSize || (*p).pz < -floorSize || (*p).pz > floorSize) == false) {
			if ((*p).py < (*p).scale / 1.5f) {
				(*p).py = (*p).scale / 1.5f;
				(*p).dy *= -bounciness;
				(*p).speed *= bounciness;
			}
		}

		(*p).rx += (*p).rix;
		(*p).ry += (*p).riy;
		(*p).rz += (*p).riz;

		(*p).age += 0.025f;
	}

	particles.erase(std::remove_if(particles.begin(), particles.end(), [bounceCutoff](const std::shared_ptr<Particle>& p) {
		return (*p).speed < bounceCutoff || (*p).age >= 5;
		}), particles.end());
}

void drawParticles() {
	for (auto& p : particles) {
		glPushMatrix();
		glColor3f((*p).color.r, (*p).color.g, (*p).color.b);
		glTranslatef((*p).px, (*p).py, (*p).pz);
		glRotatef((*p).rx, 1.0f, 0.0f, 0.0f);
		glRotatef((*p).ry, 0.0f, 1.0f, 0.0f);
		glRotatef((*p).rz, 0.0f, 0.0f, 1.0f);
		glScalef((*p).scale, (*p).scale, (*p).scale);

		switch ((*p).shape) {
		case SPHERE:
			glutSolidSphere(1, 8, 8);
			break;
		case CUBE:
			glutSolidCube(1);
			break;
		case TETRAHEDRON:
			glutSolidTetrahedron();
			break;
		case TORUS:
			glutSolidTorus(0.5, 1, 8, 8);
			break;
		case CONE:
			glutSolidCone(1, 2, 8, 8);
			break;
		case CYLINDER:
			glutSolidCylinder(1, 2, 8, 8);
			break;
		}
		glPopMatrix();
	}
}

// reset state
void reset() {
	particles.clear();
	randomSpeed = false; 			// random speed toggle
	randomColor = false; 			// random color toggle
	randomShape = false; 			// random shape toggle
	userShape = TETRAHEDRON; 		// user selected shape default
	randomScale = false; 			// random scale toggle
	randomRotation = true; 			// random rotation toggle
	moonGravity = false; 			// moon gravity toggle
	fireMode = CONTINUOUS; 			// fire mode default
	fired = false; 					// firing toggle
}

// draw overlay text
void drawText(const std::string& text, float x, float y) {
	glRasterPos2f(x, y);
	for (char c : text) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
}

// draw menu
void drawMenu() {
	// pos vars
	int gap = 12;
	int first = gap*9.5;
	int mWidth1 = 170;
	int mWidth2 = 400;

	glColor3f(0.8, 0.8, 0.8);

	if (showMenu) {
		drawText("S - Random speed: " + std::string(randomSpeed ? "true" : "false"), 10, first - gap);
		drawText("Q - Toggle fire mode", 10, first - gap * 2);
		if (fireMode == 0) drawText(" | Continuous", mWidth1, first - gap * 2);
		else if (fireMode == 1) drawText(" | F - Hold to fire", mWidth1, first - gap * 2);
		else if (fireMode == 2) drawText(" | F - Single fire", mWidth1, first - gap * 2);

		drawText("C - Random color: " + std::string(randomColor ? "true" : "false"), 10, first - gap * 3);
		drawText("X - Random shape: " + std::string(randomShape ? "true" : std::to_string(userShape + 1)), 10, first - gap * 4);
		if (!randomShape) {
			drawText("1 - Sphere", mWidth2, first - gap);
			drawText("2 - Cube", mWidth2, first - gap * 2);
			drawText("3 - Tetrahedron", mWidth2, first - gap * 3);
			drawText("4 - Torus", mWidth2, first - gap * 4);
			drawText("5 - Cone", mWidth2, first - gap * 5);
			drawText("6 - Cylinder", mWidth2, first - gap * 6);
		}
		drawText("Z - Random scale: " + std::string(randomScale ? "true" : "false"), 10, first - gap * 5);
		drawText("R - Random rotation: " + std::string(randomRotation ? "true" : "false"), 10, first - gap * 6);
		drawText("G - Moon gravity: " + std::string(moonGravity ? "true" : "false"), 10, first - gap * 7);
		drawText("I - Initialize", 10, first - gap * 8);
		drawText("H - Toggle menu", 10, first - gap * 9);
	}
	else {
		drawText("H - Toggle menu", 10, gap * 0.5);
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(0.0, 80.0, zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glRotatef(xr, 1.0f, 0.0f, 0.0f);
	glRotatef(yr, 0.0f, 1.0f, 0.0f);

	drawFloor();
	drawFountain();
	drawParticles();

	// overlay
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT), -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	drawMenu();

	// Restore the matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glutSwapBuffers();
}

void timer(int value) {
	if (fireMode == 0) createParticle();
	updateParticles();
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

// handles keyboard events
void keyboard(unsigned char key, int x, int y) {
	switch (tolower(key)) {
	case 27: // quit with esc
		exit(0);
		break;
	case 'i': // i - init
		reset();
		break;
	case 'q': // q - toggle fire mode
		fireMode = static_cast<FireMode>((fireMode + 1) % 3);
		break;
	case 'h': // h - toggle commands
		showMenu = !showMenu;
		break;
	case 's': // s - toggle random particle speed
		randomSpeed = !randomSpeed;
		break;
	case 'c': // c - toggle random color
		randomColor = !randomColor;
		break;
	case 'x': // x - toggle random shape
		randomShape = !randomShape;
		break;
	case 'z': // z - toggle random scale
		randomScale = !randomScale;
		break;
	case 'r': // r - toggle random rotation
		randomRotation = !randomRotation;
		break;
	case 'f': // f - fire
		if (fireMode == 1) {
			createParticle();
		}
		else if (fireMode == 2) {
			if (!fired) {
				createParticle();
				fired = true;
			}
		}
		break;
	case 'g': // g - toggle moon gravity
		moonGravity = !moonGravity;
		break;
	case '1':
		userShape = SPHERE;
		break;
	case '2':
		userShape = CUBE;
		break;
	case '3':
		userShape = TETRAHEDRON;
		break;
	case '4':
		userShape = TORUS;
		break;
	case '5':
		userShape = CONE;
		break;
	case '6':
		userShape = CYLINDER;
		break;
	default:
		break;
	}
}

// for single fire mode
void keyup(unsigned char key, int x, int y) {
	switch (tolower(key)) {
	case 'f':
		if (fireMode == 2) fired = false;
		break;
	default: break;
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) { // camera rotation controls
		if (state == GLUT_DOWN) {
			clicked = true;
			lx = x;
			ly = y;
		}
		else clicked = false;
	}
	// zoom controls
	if (button == 3 && state == GLUT_UP && zoom < -1.0f) zoom += 5.0f;
	if (button == 4 && state == GLUT_UP) zoom -= 5.0f;
	glutPostRedisplay();
}

void motion(int x, int y) {
	if (clicked) {
		yr += (x - lx) * 0.5f;
		xr -= (y - ly) * 0.5f;
		lx = x;
		ly = y;
		glutPostRedisplay();
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	// multisampling anti-aliasing - based on https://stackoverflow.com/a/28821361
	glutSetOption(GLUT_MULTISAMPLE, 4);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WIDTH) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) / 2);
	glutCreateWindow("Particle Fountain");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	glutTimerFunc(10, timer, 0);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	// setup projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)WIDTH / (double)HEIGHT, 1.0, 1000.0);

	// setup lighting
	setupLighting();
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glutMainLoop();
	return 0;
}