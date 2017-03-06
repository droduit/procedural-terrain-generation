#pragma once

#include "quad/quad.h"
#include "paddle.h"

class Ball : public Quad {
    private:
        float vx = 0.75f, vy = 0.75f;

    public:
        Ball() {
            SetPosition(0.0f, 0.0f);
            SetSize(0.05f, 0.05f);
        }

        void Init() { Quad::Init(NULL); }

        void Update(float dt) {
            float x = GetX() + vx * dt;
            float y = GetY() + vy * dt;

            if (x < -0.94f) vx = abs(vx);
            if (x >  0.94f) vx = -abs(vx);

            if (y < -0.98f) vy = abs(vy);
            if (y >  0.94f) vy = -abs(vy);

            SetPosition(x, y);
        }

        void Bounce(float vx) {
            this->vx = max(-1.3f, min(1.3f, this->vx + 5.0f * vx));
            this->vy = sqrt(2.0f - this->vx * this->vx);
        }
};
