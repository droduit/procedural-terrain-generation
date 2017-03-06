// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"
#include "quad/quad.h"
#include "walls.h"
#include "paddle.h"
#include "ball.h"

// Quad stuff1;
Walls walls;
Paddle paddle;
Ball ball;

void Init() {
    // sets background color
    glClearColor(1.0,1.0,1.0 /*white*/, 1.0 /*solid*/);

    walls.Init();
    paddle.Init();
    ball.Init();
}

void Update(float dt) {
    paddle.Update(dt);
    ball.Update(dt);

    if (ball.CollidesWith(paddle))
        ball.Bounce(ball.GetX() - paddle.GetX());
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    float time_s = glfwGetTime();

    walls.Draw();
    paddle.Draw();
    ball.Draw();
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static float direction = 0.0f;

    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_LEFT:
                direction += 1.0f;
                break;

            case GLFW_KEY_RIGHT:
                direction -= 1.0f;
                break;
        }
    } else if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;

            case GLFW_KEY_LEFT:
                direction -= 1.0f;
                break;

            case GLFW_KEY_RIGHT:
                direction += 1.0f;
                break;
        }
    }

    paddle.SetDirection(direction);
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
    GLFWwindow* window = glfwCreateWindow(512, 512, "planets", NULL, NULL);
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

    float lastTime = glfwGetTime();
    // render loop
    while(!glfwWindowShouldClose(window)) {
        float now = glfwGetTime();
        float dt = now - lastTime;

        Update(dt);
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();

        lastTime = now;
    }

    // {stuff}.Cleanup()
    ball.Cleanup();
    paddle.Cleanup();
    walls.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
