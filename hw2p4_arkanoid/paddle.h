#pragma once

#include "quad/quad.h"

#define speed 1.0f
#define PADDLE_Y -0.95f
#define PADDLE_WIDTH 0.25f
#define PADDLE_HEIGHT 0.05f
#define PADDLE_CONSTRAINT 0.86f

class Paddle : public Quad {
    private:
        float direction = 0.0f;

    public:
        void SetDirection(float direction) { this->direction = direction; }

        Paddle() {
            SetPosition(0.0f, PADDLE_Y);
            SetSize(PADDLE_WIDTH, PADDLE_HEIGHT);
        }

        void Init() { Quad::Init(NULL); }

        void Update(float dt) {
            float x = GetX() + direction * speed * dt;

            if (x < -PADDLE_CONSTRAINT) x = -PADDLE_CONSTRAINT;
            if (x >  PADDLE_CONSTRAINT) x =  PADDLE_CONSTRAINT;

            SetX(x);
        }
};
