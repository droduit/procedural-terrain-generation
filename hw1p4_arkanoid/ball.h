#pragma once

#include "quad/quad.h"
#include "paddle.h"

#define INIT_VX 1.4142135623731
#define MAX_SPEED 2.0f
#define MAX_V_AXIS 1.9f
#define BOUNCE_FACTOR 5.0f
#define BALL_SIZE 0.05f
#define BALL_WALL_CONSTRAINT 0.94f
#define BALL_FLOOR_CONSTRAINT 0.98f

class Ball : public Quad {
    private:
        float vx = 0.0f, vy = 0.0f;

        void SetVx(float vx) {
            this->vx = max(-MAX_V_AXIS, min(vx, MAX_V_AXIS));
            this->vy = sqrt(MAX_SPEED * MAX_SPEED - this->vx * this->vx);
        }

    public:
        Ball() {
            SetPosition(0.0f, 0.0f);
            SetSize(BALL_SIZE, BALL_SIZE);
            SetVx(INIT_VX);
        }

        void Init() { Quad::Init(NULL); }

        void Update(float dt) {
            float x = GetX() + vx * dt;
            float y = GetY() + vy * dt;

            if (x < -BALL_WALL_CONSTRAINT) vx = abs(vx);
            if (x >  BALL_WALL_CONSTRAINT) vx = -abs(vx);

            if (y >  BALL_WALL_CONSTRAINT) vy = -abs(vy);
            if (y < -BALL_FLOOR_CONSTRAINT) vy = abs(vy);

            SetPosition(x, y);
        }

        void Bounce(float dx) {
            SetVx(this->vx + BOUNCE_FACTOR * dx);
        }
};
