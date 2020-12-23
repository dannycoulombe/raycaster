//
// Created by dcoulombe on 12/23/20.
//

#ifndef MAZE_COLLISION_H
#define MAZE_COLLISION_H

bool isInCollision(float x, float y)
{
    int mapX = x / MAP_SIZE;
    int mapY = y / MAP_SIZE;

    if (map[mapY][mapX] != 1) {
        return false;
    }

    return true;
}

void adjustPositionCandidate(float &currentX, float &currentY, float newX, float newY)
{
    if (!isInCollision(newX, newY)) {
        currentY = newY;
        currentX = newX;
        return;
    }

    if (!isInCollision(currentX, newY)) {
        currentY = newY;
        return;
    }

    if (!isInCollision(newX, currentY)) {
        currentX = newX;
        return;
    }
}

#endif //MAZE_COLLISION_H
