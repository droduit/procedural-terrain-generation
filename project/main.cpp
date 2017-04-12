// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <unistd.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include "heightmap/heightmap.h"
#include "terrain/terrain.h"

#include "framebuffer.h"
#include "water/water.h"
#include "screenquad/screenquad.h"

#define CAMERA_SPEED 0.05

using namespace glm;

Heightmap heightmap;
Terrain terrain;
Water water;
Framebuffer water_reflection;
ScreenQuad sq;

int window_width = 1200;
int window_height = 900;

mat4 projection_matrix;
mat4 view_matrix;

// Camera
vec4 cam_vel;
vec2 cam_dir;
vec3 cam_pos;
vec3 light_pos;

void Init(GLFWwindow* window) {
    glClearColor(1.0, 1.0, 1.0 /*white*/, 1.0 /*solid*/);
    glEnable(GL_DEPTH_TEST);

    // setup view and projection matrices
    light_pos = vec3(-1.0f, 0.0f, 2.0f);

    cam_pos = vec3(0.0f, 0.0f, 3.0f);
    cam_dir = vec2(12.5f, -1.8f);

    projection_matrix = perspective(45.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);

    const int grid_tesselation = 512, grid_area = 600;
    GLuint heightmap_tex_id = heightmap.Init(grid_tesselation, grid_tesselation);
    terrain.Init(heightmap_tex_id, grid_tesselation, grid_area);
    terrain.SetLighting(light_pos);

    GLuint reflection_texture_id = water_reflection.Init(window_width, window_height);

    sq.Init(window_width, window_height, reflection_texture_id);
    water.Init(heightmap_tex_id, reflection_texture_id, grid_tesselation, grid_area);
}

void Update(float dt) {
    static bool first_run = true;
    static float speed = 0.0;
    static float hoffset[2] = { heightmap.dx_, heightmap.dy_ };
    static float camera_position[3]  = { 0.0, 0.0, 0.0 };
    static float camera_direction[2] = { 0.0, 0.0 };
    static float fog_color[3] = { 0.73, 0.8, 1.0 };

    camera_position[0] = cam_pos[0]; camera_position[1] = cam_pos[1]; camera_position[2] = cam_pos[2];
    camera_direction[0] = cam_dir[0]; camera_direction[1] = cam_dir[1];

    ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (first_run)
        ImGui::SetNextTreeNodeOpen(true);

    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::DragFloat3("position", camera_position, 0.005);
        ImGui::DragFloat2("direction", camera_direction, 0.005);
        ImGui::Checkbox("wireframe", &terrain.wireframe_mode_);

        cam_pos[0] = camera_position[0]; cam_pos[1] = camera_position[1]; cam_pos[2] = camera_position[2];
        cam_dir[0] = camera_direction[0]; cam_dir[1] = camera_direction[1];
    }

    if (first_run)
        ImGui::SetNextTreeNodeOpen(true);

    if (ImGui::CollapsingHeader("Terrain Options")) {
        ImGui::DragFloat2("hoffset", hoffset, 0.005);
        ImGui::DragFloat("voffset", &heightmap.voffset_, 0.005);

        ImGui::DragFloat("hcomp", &heightmap.hcomp_, 0.005);
        ImGui::DragFloat("vcomp", &heightmap.vcomp_, 0.005);

        ImGui::DragInt("seed", &heightmap.seed_, 0.05);
        ImGui::DragFloat("speed", &speed, 0.01);
    }

    if (first_run)
        ImGui::SetNextTreeNodeOpen(true);

    if (ImGui::CollapsingHeader("Fog Options")) {
        ImGui::RadioButton("linear", &terrain.fog_type_, 0); ImGui::SameLine();
        ImGui::RadioButton("exponential", &terrain.fog_type_, 1);

        ImGui::SliderFloat("start", &terrain.fog_start_, 20, 100);
        ImGui::SliderFloat("end", &terrain.fog_end_, 20, 100);
        ImGui::SliderFloat3("color", fog_color, 0.0, 1.0);
        ImGui::DragFloat("density", &terrain.fog_density_, 0.0001);
        ImGui::DragFloat("power", &terrain.fog_power_, 0.005);

        terrain.fog_color_[0] = fog_color[0];
        terrain.fog_color_[1] = fog_color[1];
        terrain.fog_color_[2] = fog_color[2];

        glClearColor(fog_color[0], fog_color[1], fog_color[2], 1.0);
    }

    if (first_run)
        ImGui::SetNextTreeNodeOpen(true);

    if (ImGui::CollapsingHeader("Texture Options")) {
        ImGui::DragFloat("fcolor", &terrain.fcolor_, 0.005);
        ImGui::DragFloat("fslope", &terrain.fslope_, 0.005);
        ImGui::DragFloat("fheight", &terrain.fheight_, 0.005);
        ImGui::DragFloat("hsnow", &terrain.hsnow_, 0.005);
        ImGui::DragFloat("fsnow", &terrain.fsnow_, 0.005);

        ImGui::DragFloat("diffuse", &terrain.diffuse_, 0.005);
        ImGui::DragFloat("specular", &terrain.specular_, 0.005);
        ImGui::DragFloat("alpha", &terrain.alpha_, 0.5);
    }

    if (first_run)
        ImGui::SetNextTreeNodeOpen(true);

    if (ImGui::CollapsingHeader("Harmonic Options")) {
        ImGui::RadioButton("fBm", &heightmap.type_, 0); ImGui::SameLine();
        ImGui::RadioButton("ridged fBm", &heightmap.type_, 1); ImGui::SameLine();
        ImGui::RadioButton("billowy fBm", &heightmap.type_, 2);
        ImGui::RadioButton("swiss turbulence", &heightmap.type_, 3);

        ImGui::SliderFloat("H", &heightmap.H_, 0.01, 2.0);
        ImGui::SliderFloat("lacunarity", &heightmap.lacunarity_, 0.8, 3.0);
        ImGui::SliderInt("octaves", &heightmap.octaves_, 1, 24);
    }

    cam_dir.x -= cam_vel[3] * dt;

    vec2 cam_dir_2d(-cos(cam_dir.x), -sin(cam_dir.x));

    float cam_speed = glm::max(0.5f, (float)pow(abs(cam_pos.z), 0.8f));

    cam_pos.z += dt * cam_vel[2] * cam_speed;

    vec3 cam_target(
        sin(cam_dir.y) * cos(cam_dir.x),
        sin(cam_dir.y) * sin(cam_dir.x),
        cos(cam_dir.y)
    );

    vec3 cam_up(
        cos(cam_dir.y) * cos(cam_dir.x),
        cos(cam_dir.y) * sin(cam_dir.x),
        -sin(cam_dir.y)
    );

    vec3 cam_look = cam_pos + cam_target;
    view_matrix = lookAt(cam_pos, cam_look, cam_up);

    terrain.cam_pos_ = cam_pos;

    heightmap.dx_ = (hoffset[0] += speed * dt * cam_dir_2d.x + dt * cam_vel[0] * cam_speed * cam_dir_2d.x - dt * cam_vel[1] * cam_speed * cam_dir_2d.y);
    heightmap.dy_ = (hoffset[1] += speed * dt * cam_dir_2d.y + dt * cam_vel[0] * cam_speed * cam_dir_2d.y + dt * cam_vel[1] * cam_speed * cam_dir_2d.x);
    heightmap.Draw();

    first_run = false;
}

void Display() {
    {
        water_reflection.Bind();

        vec3 cam_target(
            sin(cam_dir.y) * cos(cam_dir.x),
            sin(cam_dir.y) * sin(cam_dir.x),
            cos(cam_dir.y)
        );

        vec3 cam_up(
            cos(cam_dir.y) * cos(cam_dir.x),
            cos(cam_dir.y) * sin(cam_dir.x),
            -sin(cam_dir.y)
        );

        vec3 cam_look = cam_pos + cam_target;
        mat4 view_matrix_ = lookAt(cam_pos, cam_look, cam_up);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        terrain.Draw(IDENTITY_MATRIX, view_matrix_, projection_matrix);

        water_reflection.Unbind();
    }

    glViewport(0, 0, window_width, window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //sq.Draw();

    terrain.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    water.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix);
    glDisable(GL_BLEND);
}

// gets called when the windows/framebuffer is resized.
void ResizeCallback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    projection_matrix = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);

    glViewport(0, 0, width, height);

    water_reflection.Cleanup();
    water.SetReflection(water_reflection.Init(window_width, window_height));
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (ImGui::GetIO().WantCaptureKeyboard) {
        ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
        return;
    }

    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;

            case GLFW_KEY_W:
                cam_vel[0] += CAMERA_SPEED;
                break;
            case GLFW_KEY_S:
                cam_vel[0] -= CAMERA_SPEED;
                break;
            case GLFW_KEY_A:
                cam_vel[1] += CAMERA_SPEED;
                break;
            case GLFW_KEY_D:
                cam_vel[1] -= CAMERA_SPEED;
                break;
            case GLFW_KEY_F:
                cam_vel[2] -= CAMERA_SPEED * 20;
                break;
            case GLFW_KEY_R:
                cam_vel[2] += CAMERA_SPEED * 20;
                break;
            case GLFW_KEY_Q:
                cam_vel[3] -= CAMERA_SPEED * 20;
                break;
            case GLFW_KEY_E:
                cam_vel[3] += CAMERA_SPEED * 20;
                break;

            default:
                ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
        }
    }

    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_W:
                cam_vel[0] -= CAMERA_SPEED;
                break;
            case GLFW_KEY_S:
                cam_vel[0] += CAMERA_SPEED;
                break;
            case GLFW_KEY_A:
                cam_vel[1] -= CAMERA_SPEED;
                break;
            case GLFW_KEY_D:
                cam_vel[1] += CAMERA_SPEED;
                break;
            case GLFW_KEY_F:
                cam_vel[2] += CAMERA_SPEED * 20;
                break;
            case GLFW_KEY_R:
                cam_vel[2] -= CAMERA_SPEED * 20;
                break;
            case GLFW_KEY_Q:
                cam_vel[3] += CAMERA_SPEED * 20;
                break;
            case GLFW_KEY_E:
                cam_vel[3] -= CAMERA_SPEED * 20;
                break;

            default:
                ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
        }
    }
}

void CharCallback(GLFWwindow *window, unsigned int codepoint) {
    ImGui_ImplGlfwGL3_CharCallback(window, codepoint);
}

bool cameraMoving = false;
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);

    cameraMoving = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !ImGui::GetIO().WantCaptureMouse;
}

void CursorPosCallback(GLFWwindow *window, double posx, double posy) {
    const float da = M_PI / 2000.0;
    static double last_posx = posx, last_posy = posy;

    if (cameraMoving && !ImGui::GetIO().WantCaptureMouse) {
        double dx = posx - last_posx, dy = posy - last_posy;

        cam_dir.x -= dx * da;
        cam_dir.y -= dy * da;
    }

    last_posx = posx; last_posy = posy;
}

void MouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if (!glfwInit()) {
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
                                          "Terrain Generation", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for inputs
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharCallback(window, CharCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);

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

    // init ImGui
    ImGui_ImplGlfwGL3_Init(window, false);

    // initialize our OpenGL program
    Init(window);

    float lastTime = glfwGetTime();
    // render loop
    while(!glfwWindowShouldClose(window)){
        float dt = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();

        ImGui_ImplGlfwGL3_NewFrame();

        Update(dt);

        Display();
        ImGui::Render();
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // cleanup
    terrain.Cleanup();
    heightmap.Cleanup();
    water.Cleanup();
    water_reflection.Cleanup();
    sq.Cleanup();

    // close OpenGL window and terminate GLFW
    ImGui_ImplGlfwGL3_Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
