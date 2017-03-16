#pragma once
#include "icg_helper.h"

#include "texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// setup 1D color texture
#define NB_COLOR 6
GLfloat tex[NB_COLOR] = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0};

#undef M_PI
#define M_PI 3.14159

struct Light {
        glm::vec3 La = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);

        glm::vec3 light_pos = glm::vec3(0.0f, 0.0f, 2.0f);

        // pass light properties to the shader
        void Setup(GLuint program_id) {
            glUseProgram(program_id);

            // given in camera space
            GLuint light_pos_id = glGetUniformLocation(program_id, "light_pos");

            GLuint La_id = glGetUniformLocation(program_id, "La");
            GLuint Ld_id = glGetUniformLocation(program_id, "Ld");
            GLuint Ls_id = glGetUniformLocation(program_id, "Ls");

            glUniform3fv(light_pos_id, ONE, glm::value_ptr(light_pos));
            glUniform3fv(La_id, ONE, glm::value_ptr(La));
            glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
            glUniform3fv(Ls_id, ONE, glm::value_ptr(Ls));
        }
};

struct Material {
        glm::vec3 ka = glm::vec3(0.18f, 0.1f, 0.1f);
        glm::vec3 kd = glm::vec3(0.9f, 0.5f, 0.5f);
        glm::vec3 ks = glm::vec3(0.8f, 0.8f, 0.8f);
        float alpha = 60.0f;

        // pass material properties to the shaders
        void Setup(GLuint program_id) {
            glUseProgram(program_id);

            GLuint ka_id = glGetUniformLocation(program_id, "ka");
            GLuint kd_id = glGetUniformLocation(program_id, "kd");
            GLuint ks_id = glGetUniformLocation(program_id, "ks");
            GLuint alpha_id = glGetUniformLocation(program_id, "alpha");

            glUniform3fv(ka_id, ONE, glm::value_ptr(ka));
            glUniform3fv(kd_id, ONE, glm::value_ptr(kd));
            glUniform3fv(ks_id, ONE, glm::value_ptr(ks));
            glUniform1f(alpha_id, alpha);
        }
};

class Mesh : public Material, public Light {

    private:
        GLuint vertex_array_id_;        // vertex array object

        // GLSL shader program ID
        GLuint gouraud_program_id_;
        GLuint phong_program_id_;
        GLuint toon_program_id_;
        GLuint art_program_id_;
        GLuint flat_program_id_;
        GLuint spot_program_id_;

        GLuint active_program_id_;

        // tinyobjloader shapes (contains points, indices, etc)
        std::vector<tinyobj::shape_t> shapes_;

        GLuint vertex_buffer_object_;           // memory buffer
        GLuint vertex_normal_buffer_object_;    // memory buffer
        GLuint texture_1d_id_;                  // texture ID
        GLuint texture_2d_id_;                  // texture ID

    private:
        void BindShader(GLuint program_id) {
            glUseProgram(program_id);
            glBindVertexArray(vertex_array_id_);

            // vertex attribute id for positions
            GLint vertex_point_id = glGetAttribLocation(program_id, "vpoint");
            if (vertex_point_id >= 0) {
                glEnableVertexAttribArray(vertex_point_id);

                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glVertexAttribPointer(vertex_point_id, 3 /*vec3*/, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // vertex attribute id for normals
            GLint vertex_normal_id = glGetAttribLocation(program_id, "vnormal");
            if (vertex_normal_id >= 0) {
                glEnableVertexAttribArray(vertex_normal_id);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_normal_buffer_object_);
                glVertexAttribPointer(vertex_normal_id, 3 /*vec3*/, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // textures
            // TODO
            // TODO 2.1 bind texture_1d_id_ for toon shading
            // TODO 3.1 bind texture_2d_id_ for artistic shading
        }

        void UnbindShader(GLuint program_id) {
            GLint vertex_point_id = glGetAttribLocation(program_id, "vpoint");
            if (vertex_point_id >= 0) {
                glDisableVertexAttribArray(vertex_point_id);
            }

            GLint vertex_normal_id = glGetAttribLocation(program_id, "vnormal");
            if (vertex_normal_id >= 0) {
                glDisableVertexAttribArray(vertex_normal_id);
            }

            glUseProgram(0);
            glBindVertexArray(0);
        }

    public:
        void Init(const string& filename) {

            string error;
            // obj files can contains material informations
            vector<tinyobj::material_t> materials;

            bool objLoadReturn = tinyobj::LoadObj(shapes_, materials, error,
                                                  filename.c_str());

            if(!error.empty()) {
                cerr << error << endl;
            }

            if(!objLoadReturn) {
                exit(EXIT_FAILURE);
            }

            // only load the first shape from the obj file
            // (see tinyobjloader for multiple shapes inside one .obj file)

            int number_of_vertices = shapes_[0].mesh.positions.size();
            int number_of_indices = shapes_[0].mesh.indices.size();
            int number_of_normals = shapes_[0].mesh.normals.size();
            printf("Loaded mesh '%s' (#V=%d, #I=%d, #N=%d)\n", filename.c_str(),
                   number_of_vertices, number_of_indices, number_of_normals);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex buffer
            glGenBuffers(ONE, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, number_of_vertices * sizeof(float),
                         &shapes_[0].mesh.positions[0], GL_STATIC_DRAW);

            // normal buffer
            glGenBuffers(ONE, &vertex_normal_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_normal_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, number_of_normals * sizeof(float),
                         &shapes_[0].mesh.normals[0], GL_STATIC_DRAW);

            // index buffer
            GLuint vertex_buffer_object_indices;
            glGenBuffers(ONE, &vertex_buffer_object_indices);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_indices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         number_of_indices * sizeof(unsigned int),
                         &shapes_[0].mesh.indices[0], GL_STATIC_DRAW);

            // 1D texture for toon shader
            glGenTextures(1, &texture_1d_id_);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_1D, texture_1d_id_);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RED, NB_COLOR, 0, GL_RED, GL_FLOAT, tex);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // 2D texture for artistic shader
            glGenTextures(1, &texture_2d_id_);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_2d_id_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_WIDTH, TEX_HEIGHT, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, TEX_PIXEL_DATA);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // unbind
            glBindVertexArray(0);

            // compile the shaders
            gouraud_program_id_ = icg_helper::LoadShaders("gouraud_vshader.glsl",
                                                          "gouraud_fshader.glsl");
            if(!gouraud_program_id_) {
                exit(EXIT_FAILURE);
            }
            phong_program_id_ = icg_helper::LoadShaders("phong_vshader.glsl",
                                                        "phong_fshader.glsl");
            if(!phong_program_id_) {
                exit(EXIT_FAILURE);
            }

            toon_program_id_ = icg_helper::LoadShaders("toon_vshader.glsl",
                                                       "toon_fshader.glsl");
            if(!toon_program_id_) {
                exit(EXIT_FAILURE);
            }

            art_program_id_ = icg_helper::LoadShaders("art_vshader.glsl",
                                                      "art_fshader.glsl");
            if(!art_program_id_) {
                exit(EXIT_FAILURE);
            }

            flat_program_id_ = icg_helper::LoadShaders("flat_vshader.glsl",
                                                       "flat_fshader.glsl");
            if(!flat_program_id_) {
                exit(EXIT_FAILURE);
            }

            spot_program_id_ = icg_helper::LoadShaders("spot_vshader.glsl",
                                                       "spot_fshader.glsl");
            if(!spot_program_id_) {
                exit(EXIT_FAILURE);
            }

            // initial shader.
            active_program_id_ = gouraud_program_id_;
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteBuffers(1, &vertex_normal_buffer_object_);
            glDeleteProgram(gouraud_program_id_);
            glDeleteProgram(phong_program_id_);
            glDeleteProgram(toon_program_id_);
            glDeleteProgram(art_program_id_);
            glDeleteProgram(flat_program_id_);
            glDeleteProgram(spot_program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteTextures(1, &texture_1d_id_);
            glDeleteTextures(1, &texture_2d_id_);
        }

        void UseGouraudShader() {
            active_program_id_ = gouraud_program_id_;
        }

        void UseFlatShader() {
            active_program_id_ = flat_program_id_;
        }

        void UsePhongShader() {
            active_program_id_ = phong_program_id_;
        }

        void UseToonShader() {
            active_program_id_ = toon_program_id_;
        }

        void UseArtisticShader() {
            active_program_id_ = art_program_id_;
        }

        void UseSpotLightPhongShader() {
            active_program_id_ = spot_program_id_;
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {

            float scale = 1.0;
            glm::mat4 M = model;
            M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.5f));
            M = glm::rotate(M, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
            M = glm::scale(M, glm::vec3(scale));

            BindShader(active_program_id_);

            Material::Setup(active_program_id_);
            Light::Setup(active_program_id_);

            // set up spot light if needed
            GLint spot_dir_id = glGetUniformLocation(active_program_id_, "spot_dir");
            if (spot_dir_id >=0) {
                glm::vec3 spot_dir = light_pos;
                glUniform3fv(spot_dir_id, ONE, glm::value_ptr(spot_dir));
            }
            // setup matrix stack
            GLint model_id = glGetUniformLocation(active_program_id_,
                                                  "model");
            glUniformMatrix4fv(model_id, ONE, DONT_TRANSPOSE, glm::value_ptr(M));
            GLint view_id = glGetUniformLocation(active_program_id_,
                                                 "view");
            glUniformMatrix4fv(view_id, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
            GLint projection_id = glGetUniformLocation(active_program_id_,
                                                       "projection");
            glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE,
                               glm::value_ptr(projection));
            glDrawElements(GL_TRIANGLES, /*#vertices*/ shapes_[0].mesh.indices.size(),
                    GL_UNSIGNED_INT, ZERO_BUFFER_OFFSET);
            UnbindShader(active_program_id_);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
