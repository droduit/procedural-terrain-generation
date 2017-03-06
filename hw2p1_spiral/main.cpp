// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "triangle/triangle.h"


enum Spiral { SIMPLE, FERMAT };
const Spiral SPIRAL_MODE = FERMAT; // CHANGE MODE HERE

const int N_TRIANGLES_SIMPLE_SPIRAL = 50;
const int N_TRIANGLES_FERMAT_SPIRAL = 500.0f;

Triangle triangle;

void Init() {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

    triangle.Init();
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // compute transformations here
    switch (SPIRAL_MODE) {
    case SIMPLE: {
        const float SCALE_FACTOR = 0.05f;
        const float TRANSLATION_FACTOR = 0.6f;
        const float ROTATION_FACTOR = 6.0f;
        for (int i = 0; i < N_TRIANGLES_SIMPLE_SPIRAL; ++i) {
            float r = i/(float)N_TRIANGLES_SIMPLE_SPIRAL; // Distance to center
            float theta = ROTATION_FACTOR * r * M_PI;
            glm::mat4 S = glm::scale(IDENTITY_MATRIX, glm::vec3(SCALE_FACTOR*r, SCALE_FACTOR*r, 0.0f));
            glm::mat4 T = glm::translate(IDENTITY_MATRIX, glm::vec3(TRANSLATION_FACTOR*r, 0.0f, 0.0f));
            glm::mat4 R = glm::rotate(IDENTITY_MATRIX, theta, glm::vec3(0.0f, 0.0f, 1.0f));

            // Rotation done AFTER the translation to create the spiral effet
            glm::mat4 model = R * T * S;
            triangle.Draw(model);
        }
        break;
    }
    case FERMAT: {
        // fermat: r = c * sqrt(n); theta = n * 137.508º (to convert to rads)
        const float SCALE_FACTOR = 0.02f;
        const float FERMAT_FACTOR = 0.6f;
        const float TRANSLATION_FACTOR = 0.06f;
        const glm::mat4 SCALE = glm::scale(IDENTITY_MATRIX, glm::vec3(SCALE_FACTOR, SCALE_FACTOR, 0.0f));
        for (int i = 0; i < N_TRIANGLES_FERMAT_SPIRAL; ++i) {
            float r = FERMAT_FACTOR * sqrt(i);
            float theta = i * (137.508f * M_PI / 180.0f);
            glm::mat4 T = glm::translate(IDENTITY_MATRIX, glm::vec3(TRANSLATION_FACTOR*r, 0.0f, 0.0f));
            glm::mat4 R = glm::rotate(IDENTITY_MATRIX, theta, glm::vec3(0.0f, 0.0f, 1.0f));

            // Rotation done AFTER the translation to create the spiral effet
            glm::mat4 model = R * T * SCALE;
            triangle.Draw(model);
        }
        break;
    }
    }
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(512, 512, "spiral", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init();

    // render loop
    while(!glfwWindowShouldClose(window)) {
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    triangle.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
