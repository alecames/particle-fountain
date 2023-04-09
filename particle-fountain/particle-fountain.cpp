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
const float GRAVITY = 9.81f; // gravity constant

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
	float rIncrX, rIncrY, rIncrZ;   // rotation angle increments
	float scaleX, scaleY, scaleZ;   // scale factors
	ParticleShape shapeType;        // object shape type
	float r, g, b;                  // color
	int state;                      // state
	float age;                      // age
	std::shared_ptr<Particle> next; // next Record Pointer
};


const int MAX_PARTICLES = 1000;
const int PARTICLE_CREATION_INTERVAL = 100; // interval in milliseconds between particle creation

std::vector<std::shared_ptr<Particle>> particles;
int particleCreationTimer = 0; // timer to keep track of the time since the last particle was created
float groundSize = 100.0f;
float fountainSize = 15.0f;

float xRot = 0.0f;
float yRot = 0.0f;
int lastX = 0;
int lastY = 0;
bool clicked = false;

// sets up lighting 
void setupLighting() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 50.0f, 100.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

// draws the floor 
void drawFloor() {
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-groundSize, 0.0f, -groundSize);
	glVertex3f(-groundSize, 0.0f, groundSize);
	glVertex3f(groundSize, 0.0f, groundSize);
	glVertex3f(groundSize, 0.0f, -groundSize);
	glEnd();
}

// draws the fountain that the particles will be emitted from
void drawFountain() {
	glColor3f(0.35, 0.67, 0.37);
	glPushMatrix();
	glTranslatef(0.0f, fountainSize / 2, 0.0f);
	glutSolidCube(fountainSize);
	glPopMatrix();
}

void createParticle() {
	if (particles.size() >= MAX_PARTICLES) {
		particles.erase(particles.begin());
	}
	float speed = 1.0f; // default speed
	float g = 0.05f; // gravity

	auto p = std::make_shared<Particle>();
	(*p).px = 0.0f;
	(*p).py = fountainSize;
	(*p).pz = 0.0f;
	(*p).dx = (rand() % 100 - 50) * 0.01f;
	(*p).dy = 1.0f + (rand() % 100) * 0.01f;
	(*p).dz = (rand() % 100 - 50) * 0.01f;
	(*p).speed = (rand() % 100) * 0.01f;
	(*p).shapeType = ParticleShape::CUBE;
	(*p).r = (rand() % 100) * 0.01f;
	(*p).g = (rand() % 100) * 0.01f;
	(*p).b = (rand() % 100) * 0.01f;
	(*p).age = 0.0f;

	particles.push_back(p);
}


void updateParticles(float g) {
	for (auto& p : particles) {
		(*p).dx += 0.0f;
		(*p).dy -= g;
		(*p).dz += 0.0f;

		(*p).px += (*p).dx * (*p).speed;
		(*p).py += (*p).dy * (*p).speed;
		(*p).pz += (*p).dz * (*p).speed;

		(*p).age += 1.0f;

		// remove the particle if it falls below the ground
		if ((*p).py < 0.0f) {
			particles.erase(std::remove(particles.begin(), particles.end(), p), particles.end());
		}
	}
}

// display loop
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0, 80.0, 200.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	drawFloor();
	drawFountain();

	glutSwapBuffers();
}

void timer(int value) {
	updateParticles(0.05f);
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
	case 's': // s - create a particle with random speed
		createParticle();
		break;
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			clicked = true;
			lastX = x;
			lastY = y;
		}
		else {
			clicked = false;
		}
	}
}

void motion(int x, int y) {
	if (clicked) {
		yRot += (x - lastX) * 0.5f;
		xRot += (y - lastY) * 0.5f;
		lastX = x;
		lastY = y;
		glutPostRedisplay();
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WIDTH) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) / 2);
	glEnable(GL_DEPTH_TEST);
	glutCreateWindow("Particle Fountain");

	// show print controls
	showcmds();

	glutTimerFunc(10, timer, 0);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glEnable(GL_BLEND); // attempt to smooth out points
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

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