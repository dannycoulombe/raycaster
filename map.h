#include "maps/dev.h"

#define MAP_SIZE_RATIO 0.25
#define MAP_SIZE_WIDTH 256
#define MAP_SIZE_HEIGHT 256
#define MAP_POSITION_X 10
#define MAP_POSITION_Y (SCREEN_HEIGHT - ((MAP_SIZE_HEIGHT * 2) * MAP_SIZE_RATIO) - 10)
#define MAP_DEPTH 32
#define MAP_SIZE 64
#define MAP_RAY_RESOLUTION 1024
#define MAP_VIEW_ANGLE 30

#define MAP_RAY_DIST_V 0
#define MAP_RAY_DIST_H 1
#define MAP_RAY_X 2
#define MAP_RAY_Y 3
#define MAP_RAY_VECTOR_X 4
#define MAP_RAY_VECTOR_Y 5
#define MAP_RAY_ANGLE 6
#define MAP_RAY_SIDE 7

int rays[MAP_RAY_RESOLUTION][8];
int totalRays = ARRAY_SIZE(rays);
float raySize = SCREEN_WIDTH / totalRays;

void drawPlayerOnMap()
{
    // Calculate map ratio
    float pxR = (px * MAP_SIZE_RATIO) + MAP_POSITION_X;
    float pyR = (py * MAP_SIZE_RATIO) + MAP_POSITION_Y;

    // Player's point on the map
    glColor3f(1, 1, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(pxR, pyR);
    glEnd();

    // Player's direction on the map
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(pxR, pyR);
    glVertex2i(pxR + (pdx * 20), pyR + (pdy * 20));
    glEnd();
}

void drawBackground()
{
    // Roof
    glColor3f(0,0.33,0);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(SCREEN_WIDTH, 0);
    glVertex2i(SCREEN_WIDTH,SCREEN_HEIGHT / 2);
    glVertex2i(0,SCREEN_HEIGHT / 2);
    glEnd();

    // Floor
    glColor3f(0.33,0.33,0.33);
    glBegin(GL_QUADS);
    glVertex2i(0,SCREEN_HEIGHT / 2);
    glVertex2i(SCREEN_WIDTH,SCREEN_HEIGHT / 2);
    glVertex2i(SCREEN_WIDTH, SCREEN_HEIGHT);
    glVertex2i(0, SCREEN_HEIGHT);
    glEnd();
}

void calculateRays() {

    int bitShift = 6;
    int r, mx, my, mp, dof, side;
    float vx, vy, rx, ry, ra, xo, yo, disV, disH;
    float rotate = ((float) MAP_VIEW_ANGLE * 2) / (float) MAP_RAY_RESOLUTION;

    ra = FixAng(pa + (float) MAP_VIEW_ANGLE); // rotate half map view angle from player angle

    for (r = 0; r < totalRays; r++) {

        // VERTICAL
        dof = 0;
        disV = 100000;
        float Tan = tan(degToRad(ra));
        if (cos(degToRad(ra)) > 0.001) {
            rx = (((int) px >> bitShift) << bitShift) + MAP_SIZE;
            ry = (px - rx) * Tan + py;
            xo = MAP_SIZE;
            yo = -xo * Tan;
        } // looking left
        else if (cos(degToRad(ra)) < -0.001) {
            rx = (((int) px >> bitShift) << bitShift) - 0.0001;
            ry = (px - rx) * Tan + py;
            xo = -MAP_SIZE;
            yo = -xo * Tan;
        } // looking right
        else {
            rx = px;
            ry = py;
            dof = MAP_DEPTH;
        } //looking up or down. no hit

        while (dof < MAP_DEPTH) {
            mx = (int) (rx) >> bitShift;
            my = (int) (ry) >> bitShift;
            mp = my * mapX + mx;

            if (mp > 0 && mp < mapX * mapY && map[my][mx] == 1) {
                dof = MAP_DEPTH;
                disV = cos(degToRad(ra)) * (rx - px) - sin(degToRad(ra)) * (ry - py);
            } // hit
            else {
                rx += xo;
                ry += yo;
                dof += 1;
            } // check next horizontal
        }
        vx = rx;
        vy = ry;

        // HORIZONTAL
        dof = 0;
        disH = 100000;
        Tan = 1.0 / Tan;
        if (sin(degToRad(ra)) > 0.001) {
            ry = (((int) py >> bitShift) << bitShift) - 0.0001;
            rx = (py - ry) * Tan + px;
            yo = -MAP_SIZE;
            xo = -yo * Tan;
        } // looking up
        else if (sin(degToRad(ra)) < -0.001) {
            ry = (((int) py >> bitShift) << bitShift) + MAP_SIZE;
            rx = (py - ry) * Tan + px;
            yo = MAP_SIZE;
            xo = -yo * Tan;
        } // looking down
        else {
            rx = px;
            ry = py;
            dof = MAP_DEPTH;
        } // looking straight left or right

        while (dof < MAP_DEPTH) {
            mx = (int) (rx) >> bitShift;
            my = (int) (ry) >> bitShift;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && map[my][mx] == 1) {
                dof = MAP_DEPTH;
                disH = cos(degToRad(ra)) * (rx - px) - sin(degToRad(ra)) * (ry - py);
            } // hit
            else {
                rx += xo;
                ry += yo;
                dof += 1;
            } // check next horizontal
        }

        side = 0;
        if(disV < disH) { // horizontal hit first
            rx = vx;
            ry = vy;
            disH = disV;
            side = 1;
        }

        // Fix fish-eye effect
        float ca = FixAng(pa - ra);
        disH = disH * cos(degToRad(ca));

        if (dof > 0) {
            rays[r][MAP_RAY_DIST_H] = disH;
            rays[r][MAP_RAY_DIST_V] = disV;
            rays[r][MAP_RAY_X] = rx;
            rays[r][MAP_RAY_Y] = ry;
            rays[r][MAP_RAY_VECTOR_X] = vx;
            rays[r][MAP_RAY_VECTOR_Y] = vy;
            rays[r][MAP_RAY_ANGLE] = ra;
            rays[r][MAP_RAY_SIDE] = side;
        }

        ra = FixAng(ra - rotate); // go to next ray
    }
}

void drawMap2DBackground()
{
    float maxWidth = mapX * MAP_SIZE * MAP_SIZE_RATIO;
    float mapHeight = mapY * MAP_SIZE * MAP_SIZE_RATIO;

    // Map background
    glColor4f(0, 0, 0, 0.33);
    glBegin(GL_QUADS);
    glVertex2i(MAP_POSITION_X, MAP_POSITION_Y);
    glVertex2i(MAP_POSITION_X, mapHeight + MAP_POSITION_Y);
    glVertex2i(maxWidth + MAP_POSITION_X, mapHeight + MAP_POSITION_Y);
    glVertex2i(maxWidth + MAP_POSITION_X, MAP_POSITION_Y);
    glEnd();
}

void drawMap2D()
{
    int x, y, xo, yo, mapSR; // x/y & offsets, mapSizeRatio
    for (y = 0; y < ARRAY_SIZE(map); y++) {
        for (x = 0; x < ARRAY_SIZE(map[y]); x++) {

            if (map[y][x] == 1) {
                glColor4f(1,1,1, 1); // wall
            } else {
                continue;
            }

            mapSR = MAP_SIZE * MAP_SIZE_RATIO;
            xo = x * mapSR + MAP_POSITION_X;
            yo = y * mapSR + MAP_POSITION_Y;

            int pos[4][2] = {
                {0 + xo, 0 + yo},
                {0 + xo, mapSR + yo},
                {mapSR + xo, mapSR + yo},
                {mapSR + xo, 0 + yo},
            };

            glBegin(GL_QUADS); // Top-left, bottom-left, bottom-right, top-right
            glVertex2i(pos[0][0], pos[0][1]);
            glVertex2i(pos[1][0], pos[1][1]);
            glVertex2i(pos[2][0], pos[2][1]);
            glVertex2i(pos[3][0], pos[3][1]);
            glEnd();
        }
    }
}

void drawMap2DRays()
{
    float pxR = (px * MAP_SIZE_RATIO) + MAP_POSITION_X;
    float pyR = (py * MAP_SIZE_RATIO) + MAP_POSITION_Y;

    int r;
    for(r = 0; r < totalRays; r++) {

        float rx = rays[r][MAP_RAY_X];
        float ry = rays[r][MAP_RAY_Y];
        float side = rays[r][MAP_RAY_SIDE];
        float rxR = (rx * MAP_SIZE_RATIO) + MAP_POSITION_X;
        float ryR = (ry * MAP_SIZE_RATIO) + MAP_POSITION_Y;

        glColor4f(1, 0, 0, 0.005);
        glLineWidth(5);
        glBegin(GL_LINES);
        glVertex2i(pxR, pyR);
        glVertex2i(rxR, ryR);
        glEnd();
    }
}

void drawMap3D()
{
    int r;
    for (r = 0; r < totalRays; r++) {

        float disH = rays[r][MAP_RAY_DIST_H];
//        float disV = rays[r][MAP_RAY_DIST_V];
//        float rx = rays[r][MAP_RAY_X];
//        float ry = rays[r][MAP_RAY_Y];
//        float vx = rays[r][MAP_RAY_VECTOR_X];
//        float vy = rays[r][MAP_RAY_VECTOR_Y];
//        float ra = rays[r][MAP_RAY_ANGLE];
        float side = rays[r][MAP_RAY_SIDE];

        // Line height and limits
        int lineH = (MAP_SIZE * SCREEN_WIDTH) / (disH);
        if (lineH > SCREEN_WIDTH) {
            lineH = SCREEN_WIDTH;
        }

        // Line offset
        int lineOff = (SCREEN_HEIGHT / 2) - (lineH >> 1);

        // Merge ray's vertical polygon to next if exists to smooth out the forms
        int lineHNext = lineH;
        int lineOffNext = lineOff;
//        if ((r + 1) < totalRays) {
//            lineHNext = (MAP_SIZE * SCREEN_WIDTH) / (rays[r + 1][MAP_RAY_DIST_H]);
//            if (lineHNext > SCREEN_WIDTH) {
//                lineHNext = SCREEN_WIDTH;
//            }
//            lineOffNext = (SCREEN_HEIGHT / 2) - (lineHNext >> 1);
//        }

        // Draw vertical line (wall)
        glColor3f(0, side == 0 ? 0.5 : 0.8, 0);
        glBegin(GL_QUADS);
        glVertex2i(r * raySize, lineOff);
        glVertex2i((r * raySize) + raySize, lineOffNext);
        glVertex2i((r * raySize) + raySize,lineOffNext + lineHNext);
        glVertex2i(r * raySize,lineOff + lineH);
        glEnd();
    }
}