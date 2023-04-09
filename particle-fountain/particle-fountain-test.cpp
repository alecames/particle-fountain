#include <freeglut.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <math.h>
#include <vector>

// COSC 3P98 - Computer Graphics - Assignment 3
// This program will generate a 3d particle fountain
// Alec Ames 				Student #: 6843577
// Julian Ellis Geronimo 	Student #: 6756597
// Due Date: April 10th, 2023

const int WIDTH = 800;		// window width
const int HEIGHT = 800;		// window height

// a point contains an (x,y) to determine their placement on the window
// struct Point {
// 	float x, y;

// 	// boolean operators 
// 	bool operator==(const Point& other) const { return (x == other.x) && (y == other.y); }
// 	bool operator!=(const Point& other) const { return x != other.x || y != other.y; }
// 	bool operator<(const Point& other) const { return x < other.x || (x == other.x && y < other.y); }
// 	bool operator>(const Point& other) const { return x > other.x || (x == other.x && y > other.y); }
// };

struct Particle {
	float x, y, z;
	float vx, vy, vz;
	float ax, ay, az;
	float dax, day, daz;
	float speed;
	bool active;
};

const int MAX_PARTICLES = 1000;
std::vector<Particle> particles(MAX_PARTICLES);
float floorSize = 100.0f;
float fountainSize = 5.0f;

void initParticles() {
	for (auto& p : particles) {
		p.active = false;
	}
}

void createParticle() {
	
	for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            float x = 0.0f;
            float y = fountainSize; // Set the particle's initial Y position to the top of the fountain
            float z = 0.0f;
            float vx = float(rand() % 100 - 50) / 50.0f;
            float vy = float(rand() % 100) / 50.0f;
            float vz = float(rand() % 100 - 50) / 50.0f;
            float ax = 0.0f;
            float ay = 0.0f;
            float az = 0.0f;
            float dax = 0.0f;
            float day = 0.0f;
            float daz = 0.0f;
            float speed = 1.0f; // Set the default speed to 1
            bool active = true;

			particles[i] = { x, y, z, vx, vy, vz, ax, ay, az, dax, day, daz, speed, active };
			printf("Creating particle at %f, %f, %f", x, y, z);
			break;
        }
    }
}


void updateParticles() {
	float g = 0.01f; // gravity

	for (auto& p : particles) {
		if (p.active) {
			p.vx += 0.0f;
			p.vy += -g;
			p.vz += 0.0f;
			p.x += p.vx;
			p.y += p.vy;
			p.z += p.vz;
			p.ax += p.dax;
			p.ay += p.day;
			p.az += p.daz;

			if (p.y < -floorSize / 2 && abs(p.x) < floorSize / 2 && abs(p.z) < floorSize / 2) {
				p.vy = -p.vy * 0.9f; // bounce and apply friction
			}

			if (p.y < -floorSize) {
				p.active = false; // particle has fallen below the ground
			}

			// draw the particle
			glPushMatrix();
			glTranslatef(p.x, p.y, p.z);
			glRotatef(p.ax, 1.0f, 0.0f, 0.0f);
			glRotatef(p.ay, 0.0f, 1.0f, 0.0f);
			glRotatef(p.az, 0.0f, 0.0f, 1.0f);
			glColor3f(1.0f, 0.5f, 0.0f); // set particle color (orange)
			glutSolidCube(1.0f); // draw a small cube as a particle
			glPopMatrix();
		}
	}
}

void drawScene() {
	// Draw the ground
	glColor3f(0.0f, 1.0f, 0.0f); // set ground color (green)
	glBegin(GL_QUADS);
	glVertex3f(-floorSize / 2, -floorSize / 2, floorSize / 2);
	glVertex3f(floorSize / 2, -floorSize / 2, floorSize / 2);
	glVertex3f(floorSize / 2, -floorSize / 2, -floorSize / 2);
	glVertex3f(-floorSize / 2, -floorSize / 2, -floorSize / 2);
	glEnd();


	glColor3f(1.0f, 0.0f, 0.0f); // set test cube color (red)
	glPushMatrix();
	glTranslatef(0.0f, fountainSize / 2, 0.0f);
	glutSolidCube(1.0f); // draw a test cube
	glPopMatrix();

	// Draw the fountain
	glColor3f(0.1f, 0.1f, 0.1f); // set fountain color (blue)
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glutSolidCube(fountainSize); // draw a cube as a fountain
	glPopMatrix();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	drawScene();
	updateParticles();

	glutSwapBuffers();
}

void timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

// prints controls to terminal
void showcmds() {
	printf("|------------------------------------------------------------------------------|\n");
	printf("| H: Help                       PARTICLE FOUNTAIN                ESC / Q: Quit |\n");
	printf("|------------------------------------------------------------------------------|\n");
	printf("| 1:             | 2:            | 3:               | 4:          | L:         |\n");
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
	case 'f': // fire particles
		createParticle();
		break;
	case 'r': // reset the simulation
		initParticles();
		break;
	default:
		break;
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WIDTH) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) / 2);
	glEnable(GL_DEPTH_TEST);
	glutCreateWindow("Particle Fountain");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, double(WIDTH) / double(HEIGHT), 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 100.0, 300.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// show print controls
	showcmds();

	glutTimerFunc(10, timer, 0);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glEnable(GL_BLEND); // attempt to smooth out points
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glutMainLoop();
	return 0;
}