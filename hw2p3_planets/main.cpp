// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "quad/quad.h"

#define TIME_FACTOR 4.0f

// Quad stuff1;
// ...

Quad space, sun, earth, moon;

void Init() {
    // sets background color
    glClearColor(1.0,1.0,1.0 /*white*/, 1.0 /*solid*/);

    space.Init("space.tga");
    sun.Init("sun.tga");
    earth.Init("earth.tga");
    moon.Init("moon.tga");
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    float time_s = TIME_FACTOR * glfwGetTime();

    glm::mat4 sunMat = glm::mat4(1.0f);
    sunMat = glm::translate(sunMat, glm::vec3(0.2, 0.0, 0.0));
    sunMat = glm::scale(sunMat, glm::vec3(0.2, 0.2, 1.0));
    sunMat = glm::rotate(sunMat, -time_s, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 earthMat = glm::mat4(1.0f);
    earthMat = glm::translate(earthMat, glm::vec3(0.7 * cos(0.75f * time_s), 0.5 * sin(0.75f * time_s), 0.0f));
    earthMat = glm::rotate(earthMat, time_s * 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    earthMat = glm::scale(earthMat, glm::vec3(0.1f, 0.1f, 1.0f));

    glm::mat4 moonMat = earthMat;
    moonMat = glm::translate(moonMat, glm::vec3(2.0f, 0.0f, 0.0f));
    moonMat = glm::scale(moonMat, glm::vec3(0.4f, 0.4f, 1.0f));
    moonMat = glm::rotate(moonMat, time_s * 4.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    // compute the transformation matrices
    space.Draw();
    sun.Draw(sunMat);
    earth.Draw(earthMat);
    moon.Draw(moonMat);
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

    // render loop
    while(!glfwWindowShouldClose(window)) {
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // {stuff}.Cleanup()
    moon.Cleanup();
    earth.Cleanup();
    sun.Cleanup();
    space.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
