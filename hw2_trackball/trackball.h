#pragma once
#include "icg_helper.h"

using namespace glm;

#define ANGLE_FACTOR 1.0f

class Trackball {
    private:
        float radius;
        vec3 anchor_pos;
        mat4 rotation;

        // projects the point p (whose z coordiante is still empty/zero) onto the
        // trackball surface. If the position at the mouse cursor is outside the
        // trackball, use a hyberbolic sheet as explained in:
        // https://www.opengl.org/wiki/Object_Mouse_Trackball.
        // The trackball radius is given by 'radius_'.
        void ProjectOntoSurface(vec3& p) const {
            // TODO 2: Implement this function. Read above link for details.

            float sqr = radius * radius;
            float sqxy = p.x * p.x + p.y * p.y;

            if (sqxy <= sqr)
                p.z = sqrt(sqr - sqxy);
            else
                p.z = sqr / (2.0f * sqrt(sqxy));
        }

    public:
        Trackball() : radius(1.0f) {}

        // this function is called when the user presses the left mouse button down.
        // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
        // is the top right corner.
        void BeingDrag(float x, float y) {
            anchor_pos = vec3(x, y, 0.0f);
            ProjectOntoSurface(anchor_pos);
        }

        // this function is called while the user moves the curser around while the
        // left mouse button is still pressed.
        // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
        // is the top right corner.
        // returns the rotation of the trackball in matrix form.
        mat4 Drag(float x, float y) {
            vec3 current_pos = vec3(x, y, 0.0f);
            ProjectOntoSurface(current_pos);

            // TODO 3: Calculate the rotation given the projections of the anocher
            // point and the current position. The rotation axis is given by the cross
            // product of the two projected points, and the angle between them can be
            // used as the magnitude of the rotation.
            // you might want to scale the rotation magnitude by a scalar factor.
            // p.s. No need for using complicated quaternions as suggested inthe wiki
            // article.
            vec3 axis = normalize(cross(anchor_pos, current_pos));
            float angle = orientedAngle(normalize(anchor_pos), normalize(current_pos), axis);

            if (abs(angle) < 0.0001) // rotate doesn't like when the angle is 0.0
                return IDENTITY_MATRIX;
            else
                return rotate(IDENTITY_MATRIX, ANGLE_FACTOR * angle, axis);
        }
};
