#pragma once

#include "quad/quad.h"

#define THICKNESS 0.05f

class Walls {
    private:
        Quad leftWall, topWall, rightWall;

    public:
        void Init() {
            leftWall.Init(NULL);
            leftWall.SetPosition(-0.975f, 0.0f);
            leftWall.SetSize(THICKNESS, 2.0f);

            topWall.Init(NULL);
            topWall.SetPosition(0.0f, 0.975f);
            topWall.SetSize(2.0f, THICKNESS);

            rightWall.Init(NULL);
            rightWall.SetPosition(0.975f, 0.0f);
            rightWall.SetSize(THICKNESS, 2.0f);
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
