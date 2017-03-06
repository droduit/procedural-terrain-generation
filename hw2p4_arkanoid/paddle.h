#pragma once

#include "quad/quad.h"

#define speed 1.0f

class Paddle : public Quad {
    private:
        float direction = 0.0f;

    public:
        void SetDirection(float direction) { this->direction = direction; }

        Paddle() {
            SetPosition(0.0f, -0.95f);
            SetSize(0.25f, 0.05f);
        }

        void Init() { Quad::Init(NULL); }

        void Update(float dt) {
            float x = GetX() + direction * speed * dt;

            if (x < -0.86f) x = -0.86f;
            if (x >  0.86f) x =  0.86f;

            SetX(x);
        }
};
