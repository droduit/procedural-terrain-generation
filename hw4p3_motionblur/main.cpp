// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"
#include "glm/gtc/matrix_transform.hpp"

#include "framebuffer.h"

#include "quad/quad.h"
#include "screenquad/screenquad.h"

int window_width = 1024;
int window_height = 768;

Quad wheel;

FrameBuffer framebuffer;
ScreenQuad screenquad;

using namespace glm;

mat4 projection_matrix;
mat4 view_matrix;
mat4 old_MVP;

float rotation_angle = 0.0;
float previousTime = 0.0;
float spin_factor = 3;
bool enabled = true;

void Init(GLFWwindow* window) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    wheel.Init();

    // on retina/hidpi displays, pixels != screen coordinates
    // this unsures that the framebuffer for the motion blur effect
    // has the same size as the window
    // (see http://www.glfw.org/docs/latest/window.html#window_fbsize)
    glfwGetFramebufferSize(window, &window_width, &window_height);
    GLuint framebuffer_texture_id;
    GLuint framebuffer_velocity_texture_id;
    std::tie(framebuffer_texture_id, framebuffer_velocity_texture_id) =
            framebuffer.Init(window_width, window_height, true /*interpolate*/);

    screenquad.Init(window_width, window_height, framebuffer_texture_id,
                    framebuffer_velocity_texture_id);

    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);

    vec3 cam_pos(0.0f, 0.01f, 3.0f);
    vec3 cam_look(0.0f, 0.0f, 0.0f);
    vec3 cam_up(0.0f, 0.0f, 1.0f);
    view_matrix = lookAt(cam_pos, cam_look, cam_up);

    old_MVP = projection_matrix * view_matrix;
}

void Display() {
    float currentTime = glfwGetTime();
    float dt = currentTime - previousTime;
    previousTime = currentTime;
    rotation_angle += dt*spin_factor;

    mat4 model = rotate(IDENTITY_MATRIX, -rotation_angle, vec3(0.0f, 0.0f, 1.0f));

    mat4 MVP = projection_matrix * view_matrix * model;

    framebuffer.ClearContent();
    framebuffer.Bind();
    wheel.Draw(MVP, enabled ? old_MVP : MVP);
    framebuffer.Unbind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window_width, window_height);

    screenquad.Draw();
    old_MVP = MVP;
}

// gets called when the windows/framebuffer is resized.
void ResizeCallback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);

    glViewport(0, 0, window_width, window_height);
    framebuffer.Cleanup();
    framebuffer.Init(window_width, window_height);
    screenquad.UpdateSize(window_width, window_height);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if(action == GLFW_RELEASE) {
        switch (key) {
            case 'Q':
                spin_factor+=0.33;
                break;
            case 'A':
                spin_factor-=0.33;
                break;
            case 'E':
                enabled = !enabled;
                break;
            default:
                break;
        }
        std::cout<< "spin factor: "<< spin_factor << std::endl;
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
    GLFWwindow* window = glfwCreateWindow(window_width, window_height,
                                          "Motion Blur", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, ResizeCallback);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init(window);

    // render loop
    while(!glfwWindowShouldClose(window)){
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    wheel.Cleanup();
    screenquad.Cleanup();
    framebuffer.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
