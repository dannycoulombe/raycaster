//https://www.youtube.com/watch?v=gYRrGTC7GtA

#include <iostream>
#include <GL/glut.h>
#include <cmath>
#include <map>

#include "constants.h"
#include "shared.h"
#include "functions.h"
#include "map.h"
#include "collision.h"

std::map<char, bool> activeKeys;

void keyboardDown(unsigned char key, int x, int y)
{
    if (activeKeys.find(key) == activeKeys.end()) {
        activeKeys.insert(std::pair<char, bool>(key, true));
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    activeKeys.erase(key);
}

void handleMovement(float &x, float &y, float &dx, float &dy, float &a)
{
    int move_factor = MOVE_FACTOR;
    int rotate_factor = MOVE_FACTOR;
    float cpx = px, cpy = py;

    float spa;
    float sa = 90;

    for (auto const& key : activeKeys) {
        switch (key.first) {

            case 'q': // strife left
                spa = FixAng(pa + sa);
                cpx += cos(degToRad(spa)) * move_factor;
                cpy += -sin(degToRad(spa)) * move_factor;
                break;

            case 'e': // strife right
                spa = FixAng(pa - sa);
                cpx += cos(degToRad(spa)) * move_factor;
                cpy += -sin(degToRad(spa)) * move_factor;
                break;

            case 'w': // forward
                cpx += pdx * move_factor;
                cpy += pdy * move_factor;
                break;

            case 's': // backward
                cpx -= pdx * move_factor;
                cpy -= pdy * move_factor;
                break;

            case 'a': // rotate left
                pa = FixAng(pa + rotate_factor);
                pdx = cos(degToRad(pa));
                pdy = -sin(degToRad(pa));
                break;

            case 'd': // rotate right
                pa = FixAng(pa - rotate_factor);
                pdx = cos(degToRad(pa));
                pdy = -sin(degToRad(pa));
                break;
            default: break;
        }
    }

    adjustPositionCandidate(x, y, cpx, cpy);

//    if (isInCollision(x, y)) {
//
//    }

    glutPostRedisplay(); // flag as redrawing required
}

void display()
{
    handleMovement(px, py, pdx, pdy, pa);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawBackground();
    calculateRays();
    drawMap3D();
    drawMap2DBackground();
    drawMap2DRays();
    drawMap2D();
    drawPlayerOnMap();

    glutSwapBuffers(); // promote back buffer to front buffer
}

void init()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    gluOrtho2D(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0); // define a 2D orthographic projection matrix

    // initialize player variables
    px = 300;
    py = 300;
    pa = M_PI;
    pdx = cos(degToRad(pa)) * MOVE_FACTOR;
    pdy = -sin(degToRad(pa)) * MOVE_FACTOR;
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("Raycasting Maze");
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutMainLoop();
}
