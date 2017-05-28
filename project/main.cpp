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
#include "shadows.h"
#include "water/water.h"
#include "skybox/skybox.h"
#include "screenquad/screenquad.h"

#define CAMERA_SPEED (0.05)
#define FRAMEBUFFER_RATIO (0.5)

using namespace glm;

Heightmap heightmap;
Terrain terrain;
Water water;
Framebuffer water_reflection;
ScreenQuad sq;
SkyBox skybox;
Shadows shadows;

int window_width = 1600;
int window_height = 1000;
int water_framebuffer_width = (int) window_width*FRAMEBUFFER_RATIO;
int water_framebuffer_height = (int) window_height*FRAMEBUFFER_RATIO;
const vec3 cam_up = vec3(0.0f, 0.0f, 1.0f);

mat4 projection_matrix;
mat4 framebuffer_projection_matrix;
mat4 view_matrix;
mat4 light_matrix;

// Camera
vec4 cam_vel;
vec4 cam_acc; // Stock the user informations of movements
vec2 cam_dir;
vec3 cam_pos;
vec3 light_pos;

GLuint shadows_tex_id = 0;
GLuint heightmap_tex_id = 0;

void Init(GLFWwindow* window) {
    glClearColor(1.0, 1.0, 1.0 /*white*/, 1.0 /*solid*/);
    glEnable(GL_DEPTH_TEST);

    // setup view and projection matrices
    light_pos = vec3(300.0f, 0.0f, 60.0f);

    cam_pos = vec3(0.0f, 0.0f, 3.0f);
    cam_dir = vec2(12.5f, -1.8f);

    projection_matrix = perspective(45.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);
    framebuffer_projection_matrix = perspective(45.0f, (float)water_framebuffer_width/ (float)water_framebuffer_height, 0.1f, 1000.0f);

    mat4 light_projection = ortho(-80.0f, 80.0f, -300.0f, 300.0f, 0.1f, 800.0f); // Don't need to resize it in window resize callback
    mat4 light_view = lookAt(light_pos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    light_matrix = light_projection * light_view;

    const int grid_tesselation = 512, grid_area = 600;
    shadows_tex_id = shadows.Init(grid_tesselation, grid_tesselation);

    heightmap_tex_id = heightmap.Init(grid_tesselation, grid_tesselation);
    terrain.Init(heightmap_tex_id, shadows_tex_id, grid_tesselation, grid_area);
    terrain.SetLighting(light_pos, light_matrix);

    GLuint reflection_texture_id = water_reflection.Init(water_framebuffer_width, water_framebuffer_height);

    sq.Init(window_width, window_height, reflection_texture_id);
    water.Init(heightmap_tex_id, reflection_texture_id, grid_tesselation, grid_area, FRAMEBUFFER_RATIO, &heightmap.dx_, &heightmap.dy_);

    skybox.Init();

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

        water.fog_type_ = terrain.fog_type_;
        water.fog_start_ = terrain.fog_start_;
        water.fog_end_ = terrain.fog_end_;
        water.fog_color_[0] = fog_color[0];
        water.fog_color_[1] = fog_color[1];
        water.fog_color_[2] = fog_color[2];
        water.fog_density_ = terrain.fog_density_;
        water.fog_power_ = terrain.fog_power_;

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

        ImGui::SliderFloat("light bias min", &terrain.light_bias_min_, 0.1, 10);
        ImGui::SliderFloat("light bias max", &terrain.light_bias_max_, 0.1, 10);
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
        ImGui::SliderFloat("warp", &heightmap.warp_, 0.01, 2.0);
        ImGui::SliderInt("octaves", &heightmap.octaves_, 1, 24);
    }

    if (first_run)
        ImGui::SetNextTreeNodeOpen(true);

    if (ImGui::CollapsingHeader("SkyBox")) {
        ImGui::SliderFloat("Hour", &skybox.hour_, 0.0, 24.0);
        ImGui::SliderFloat("Rotate X", &skybox.rotX_, 0.0, 2*3.142);
        ImGui::SliderFloat("Rotate Y", &skybox.rotY_, 0.0, 2*3.142);
        ImGui::SliderFloat("Rotate Z", &skybox.rotZ_, 0.0, 2*3.142);
    }

    ImGui::Image((void*)shadows_tex_id, ImVec2(512, 512));

    if(first_run)
        ImGui::SetNextTreeNodeOpen(true);

    // Updating lighting
    float sun_angle = M_PI * (skybox.hour_ - 7.0) / 12.0;
    light_pos = vec3(300.0 * cos(sun_angle), 0.0, 300.0 * sin(sun_angle));
    mat4 light_projection = ortho(-300.0f, 300.0f, -300.0f, 300.0f, 0.1f, 800.0f);
    mat4 light_view = lookAt(light_pos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    light_matrix = light_projection * light_view;

    terrain.SetLighting(light_pos, light_matrix);

    // Updating camera
    float cam_speed = glm::max(0.5f, (float)pow(abs(cam_pos.z), 0.8f));

    for (int i = 0; i < 4; i++)
        cam_vel[i] += dt * (30.0 * cam_acc[i] - cam_vel[i] * 15.0);

    cam_dir.x -= cam_vel[3] * dt;

    vec2 cam_dir_2d(-cos(cam_dir.x), -sin(cam_dir.x));


    cam_pos.z += dt * cam_vel[2] * cam_speed;
    cam_pos.z = glm::max(cam_pos.z, heightmap.GetCenterHeight() + 0.8f);

    vec3 cam_target(
        sin(cam_dir.y) * cos(cam_dir.x),
        sin(cam_dir.y) * sin(cam_dir.x),
        cos(cam_dir.y)
    );

    vec3 cam_look = cam_pos + cam_target;
    view_matrix = lookAt(cam_pos, cam_look, cam_up);

    terrain.cam_pos_ = cam_pos;
    water.cam_pos_ = cam_pos;

    heightmap.dx_ = (hoffset[0] += speed * dt * cam_dir_2d.x + dt * cam_vel[0] * cam_speed * cam_dir_2d.x - dt * cam_vel[1] * cam_speed * cam_dir_2d.y);
    heightmap.dy_ = (hoffset[1] += speed * dt * cam_dir_2d.y + dt * cam_vel[0] * cam_speed * cam_dir_2d.y + dt * cam_vel[1] * cam_speed * cam_dir_2d.x);
    heightmap.Draw();

    terrain.hoffset_.x = hoffset[0];
    terrain.hoffset_.y = hoffset[1];

    first_run = false;
}


// http://stackoverflow.com/questions/785097/how-do-i-implement-a-b%C3%A9zier-curve-in-c
vec3 getBezierPoint(vec3* points, float t) {
    int numPoints = 10;
    vec3* tmp = new vec3[numPoints];
    memcpy(tmp, points, numPoints * sizeof(vec2));

    int i = numPoints - 1;
    while (i > 0) {
        for (int k = 0; k < i; k++)
            tmp[k] = tmp[k] + t * ( tmp[k+1] - tmp[k] );
        i--;
    }

    vec3 answer = tmp[0];
    delete[] tmp;
    return answer;
}


void Display() {
    float time = glfwGetTime();


    /*
    vec3 bPoints = getBezierPoint(time);
    cam_pos[2] = bPoints[2];
    cam_dir = vec2(bPoints[0], bPoints[1]);
    */

    // Draw the water reflection on a framebuffer
    {
        water_reflection.Bind();

        vec3 cam_target(
            sin(cam_dir.y) * cos(cam_dir.x),
            sin(cam_dir.y) * sin(cam_dir.x),
            -cos(cam_dir.y)
        );

        vec3 cam_pos_invert = cam_pos;
        cam_pos_invert.z = -cam_pos_invert.z;
        vec3 cam_look = cam_pos_invert + cam_target;
        mat4 view_matrix_ = lookAt(cam_pos_invert, cam_look, cam_up);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: Don't draw the skybox for the water reflection but use the cubemap texture directly
        skybox.Draw(cam_pos_invert, IDENTITY_MATRIX, view_matrix_, framebuffer_projection_matrix);

        glEnable(GL_CLIP_DISTANCE0);
        terrain.SetClipPlane(vec4(0.0f, 0.0f, 1.0f, 0.0f));
        terrain.Draw(IDENTITY_MATRIX, view_matrix_, framebuffer_projection_matrix);
        terrain.SetClipPlane(vec4(0.0f));
        glDisable(GL_CLIP_DISTANCE0);

        water_reflection.Unbind();
    }

    {
        shadows.Bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        terrain.Draw(light_matrix);
        shadows.Unbind();
    }

    glViewport(0, 0, window_width, window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox.Draw(cam_pos, IDENTITY_MATRIX, view_matrix, projection_matrix);
    terrain.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    water.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix);
    glDisable(GL_BLEND);

    //sq.Draw();
}

// gets called when the windows/framebuffer is resized.
void ResizeCallback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;
    water_framebuffer_width = (int) window_width*FRAMEBUFFER_RATIO;
    water_framebuffer_height = (int) window_height*FRAMEBUFFER_RATIO;

    projection_matrix = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
    framebuffer_projection_matrix = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);

    glViewport(0, 0, width, height);

    water_reflection.Cleanup();
    water.SetReflection(water_reflection.Init(water_framebuffer_width, water_framebuffer_height));
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
            case GLFW_KEY_P:
                cout << "cam_pos : (" << cam_pos[0] << ", " << cam_pos[1] << ", " << cam_pos[2] << ")" << endl;
                cout << "cam_dir : (" << cam_dir[0] << ", " << cam_dir[1] << ")" << endl;
                break;


            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;

            case GLFW_KEY_W:
                cam_acc[0] += CAMERA_SPEED;
                break;
            case GLFW_KEY_S:
                cam_acc[0] -= CAMERA_SPEED;
                break;
            case GLFW_KEY_A:
                cam_acc[1] += CAMERA_SPEED;
                break;
            case GLFW_KEY_D:
                cam_acc[1] -= CAMERA_SPEED;
                break;
            case GLFW_KEY_F:
                cam_acc[2] -= CAMERA_SPEED * 20;
                break;
            case GLFW_KEY_R:
                cam_acc[2] += CAMERA_SPEED * 20;
                break;
            case GLFW_KEY_Q:
                cam_acc[3] -= CAMERA_SPEED * 20;
                break;
            case GLFW_KEY_E:
                cam_acc[3] += CAMERA_SPEED * 20;
                break;

            default:
                ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
        }
    }

    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_W:
                cam_acc[0] -= CAMERA_SPEED;
                break;
            case GLFW_KEY_S:
                cam_acc[0] += CAMERA_SPEED;
                break;
            case GLFW_KEY_A:
                cam_acc[1] -= CAMERA_SPEED;
                break;
            case GLFW_KEY_D:
                cam_acc[1] += CAMERA_SPEED;
                break;
            case GLFW_KEY_F:
                cam_acc[2] += CAMERA_SPEED * 20;
                break;
            case GLFW_KEY_R:
                cam_acc[2] -= CAMERA_SPEED * 20;
                break;
            case GLFW_KEY_Q:
                cam_acc[3] += CAMERA_SPEED * 20;
                break;
            case GLFW_KEY_E:
                cam_acc[3] -= CAMERA_SPEED * 20;
                break;

            default:
                ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
        }
    }

    if(abs(cam_acc[0]) <  10e-4) {
        cam_acc[0] = 0;
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
    skybox.Cleanup();
    shadows.Cleanup();
    sq.Cleanup();

    // close OpenGL window and terminate GLFW
    ImGui_ImplGlfwGL3_Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
