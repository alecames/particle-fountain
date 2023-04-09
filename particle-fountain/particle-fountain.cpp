#include <freeglut.h>
#include <vector>
#include <memory>

// COSC 3P98 - Computer Graphics - Assignment 3
// This program will generate a 3d particle fountain

// Alec Ames                 Student #: 6843577
// Julian Ellis Geronimo     Student #: 6756597
// Due Date: April 10th, 2023


const int WIDTH = 800;      // window width
const int HEIGHT = 800;     // window height
const float G = 0.05f; 		// gravity constant

enum Shape { SPHERE, CUBE, TETRAHEDRON, TORUS, CONE, TEAPOT };

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
	float rix, riy, riz;   // rotation angle increments
	float scaleX, scaleY, scaleZ;   // scale factors
	Shape shape;        // object shape type
	Color color;	                // color
	int state;                      // state: alive
	float age;                      // age: used if entities have a finite lifespan
	std::shared_ptr<Particle> next; // next Record Pointer
};

// max particle count
const int MAX_PARTICLES = 1000;

std::vector<std::shared_ptr<Particle>> particles;
float floorSize = 100.0f;
float fountainSize = 15.0f;
float zoom = -180.0f;
float speed = 1.0f;
float randomSpeed = false;

// vars for mouse control
float xr = 0.0f; 	// x plane rotation
float yr = 0.0f; 	// y plane rotation
int lx = 0; 		// last x position
int ly = 0; 		// last y position
bool clicked = false;

// sets up lighting 
void setupLighting() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
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
	// cylinder base of fountain
	glColor3f(0.5f, 0.5f, 0.5f);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCylinder(fountainSize, 4.0f, 24, 1);
	glPopMatrix();

	// ridge
	glPushMatrix();
	glTranslatef(0.0f, 1.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidTorus(3.0f, fountainSize, 24, 20);
	glPopMatrix();

	// cone top of fountain
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
	glutSolidCone(2.0f, fountainSize - 2.0f, 20, 20);
	glPopMatrix();

	// other cone top of fountain
	glPushMatrix();
	glTranslatef(0.0f, fountainSize, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(3.0f, 3.0f, 20, 20);
	glPopMatrix();
}

// creates a particle
void createParticle() {
	if (particles.size() >= MAX_PARTICLES) {
		particles.erase(particles.begin());
	}
	auto p = std::make_shared<Particle>();
	(*p).px = 0.0f;
	(*p).py = fountainSize; 
	(*p).pz = 0.0f;
	(*p).dx = (rand() % 100 - 50) * 0.01f;
	(*p).dy = 1.0f + (rand() % 100) * 0.01f;
	(*p).dz = (rand() % 100 - 50) * 0.01f;
	if (randomSpeed) {
		speed = (rand() % 100) * 0.01f;
	} (*p).speed = speed;
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
		(*p).dy -= G;
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

		(*p).age += 1.0f;
	}

	particles.erase(std::remove_if(particles.begin(), particles.end(), [](const std::shared_ptr<Particle>& p) {
		return (*p).py < 0.0f;
		}), particles.end());
}

void drawParticles() {
	for (auto& p : particles) {
		glPushMatrix();
		glColor3f((*p).color.r, (*p).color.g, (*p).color.b);
		glTranslatef((*p).px, (*p).py, (*p).pz);
		if ((*p).shape == Shape::CUBE) {
			glutSolidCube(1.0f);
		}
		else if ((*p).shape == Shape::SPHERE) {
			glutSolidSphere(1.0f, 10, 10);
		}
		else if ((*p).shape == Shape::CONE) {
			glutSolidCone(1.0f, 1.0f, 10, 10);
		}
		else if ((*p).shape == Shape::TETRAHEDRON) {
			glutSolidTetrahedron();
		}
		else if ((*p).shape == Shape::TEAPOT) {
			glutSolidTeapot(1.0f);
		}
		else if ((*p).shape == Shape::TORUS) {
			glutSolidTorus(0.5f, 1.0f, 10, 10);
		}
		glPopMatrix();
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
	glutSwapBuffers();
}

void menuFunc(int option) {
	switch (option) {
	case 1:
		// 
		break;
	case 2:
		exit(0);
		break;
	}
}

void createMenu() {
	int menu = glutCreateMenu(menuFunc);
	glutAddMenuEntry("Exit", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void timer(int value) {
	createParticle();
	updateParticles();
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

// prints controls to terminal
void showcmds() {
	printf("|------------------------------------------------------------------------------|\n");
	printf("| H: Help                       PARTICLE FOUNTAIN                ESC / Q: Quit |\n");
	printf("|------------------------------------------------------------------------------|\n");
	printf("|                                                                              |\n");
	printf("|------------------------------------------------------------------------------|\n");
}

// handles keyboard events
void keyboard(unsigned char key, int x, int y) {
	switch (tolower(key)) {
	case 'q': // quit with q
		exit(0);
		break;
	case 27: // quit with esc
		exit(0);
		break;
	case 'h': // h - help
		showcmds();
		break;
	case 's': // s - toggle random particle speed
		randomSpeed = !randomSpeed;
		break;
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			clicked = true;
			lx = x;
			ly = y;
		}
		else clicked = false;
	}
	if (button == 3 && state == GLUT_UP && zoom < -1.0f) zoom += 3.0f;
	if (button == 4 && state == GLUT_UP) zoom -= 3.0f;
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
	glEnable(GL_DEPTH_TEST);
	glutCreateWindow("Particle Fountain");

	// show print controls
	showcmds();
	createMenu();

	glutTimerFunc(10, timer, 0);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
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