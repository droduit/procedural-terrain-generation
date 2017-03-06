#pragma once

#include "quad/quad.h"

#define WALL_THICKNESS 0.05f
#define WALL_OFFSET 0.975f
#define WALL_HEIGHT 2.0f

class Walls {
    private:
        Quad leftWall, topWall, rightWall;

    public:
        void Init() {
            leftWall.Init(NULL);
            leftWall.SetPosition(-WALL_OFFSET, 0.0f);
            leftWall.SetSize(WALL_THICKNESS, WALL_HEIGHT - 0.05f);

            topWall.Init(NULL);
            topWall.SetPosition(0.0f, WALL_OFFSET);
            topWall.SetSize(WALL_HEIGHT, WALL_THICKNESS);

            rightWall.Init(NULL);
            rightWall.SetPosition(WALL_OFFSET, 0.0f);
            rightWall.SetSize(WALL_THICKNESS, WALL_HEIGHT - 0.05f);
        }

        void Draw() {
            leftWall.Draw();
            topWall.Draw();
            rightWall.Draw();
        }

        void Cleanup() {
            leftWall.Cleanup();
            topWall.Cleanup();
            rightWall.Cleanup();
        }
};
