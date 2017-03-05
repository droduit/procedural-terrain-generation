#pragma once

#include "quad/quad.h"

#define THICKNESS 0.05f

class Walls {
    private:
        Quad leftWall, topWall, rightWall;

    public:
        void Init() {
            leftWall.Init(NULL);
            leftWall.setPosition(-0.975f, 0.0f);
            leftWall.setSize(THICKNESS, 2.0f);

            topWall.Init(NULL);
            topWall.setPosition(0.0f, 0.975f);
            topWall.setSize(2.0f, THICKNESS);

            rightWall.Init(NULL);
            rightWall.setPosition(0.975f, 0.0f);
            rightWall.setSize(THICKNESS, 2.0f);
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
