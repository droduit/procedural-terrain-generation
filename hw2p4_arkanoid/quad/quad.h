#pragma once
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Quad {

    private:
        float x, y;
        float width, height;

        GLuint vertex_array_id_; // vertex array object
        GLuint program_id_; // GLSL shader program ID
        GLuint vertex_buffer_object_; // memory buffer
        GLuint texture_id_; // texture ID
        GLuint MVP_id_; // MVP matrix
        GLuint time_id;

    public:
        float GetX() { return x; }
        float GetY() { return y; }
        float GetWidth() { return width; }
        float GetHeight() { return height; }

        void SetX(float x) { this->x = x; }
        void SetY(float y) { this->y = y; }
        void SetWidth(float w) { this->width = w; }
        void SetHeight(float h) { this->height = h; }

        void SetPosition(float x, float y) {
            SetX(x);
            SetY(y);
        }

        void SetSize(float width, float height) {
            SetWidth(width);
            SetHeight(height);
        }

        void Update(float dt) {}

        bool CollidesWith(Quad &that) {
            const float thisHW = this->width / 2.0f;
            const float thisHH = this->height / 2.0f;
            const float thatHW = that.width / 2.0f;
            const float thatHH = that.height / 2.0f;

            return !(
                (that.x + thatHW < this->x - thisHW) ||
                (that.x - thatHW > this->x + thisHW) ||
                (that.y + thatHH < this->y - thisHH) ||
                (that.y - thatHH > this->y + thisHH)
            );
        }

        void Init(const char* texture_filename) {
            // compile the shaders
            program_id_ = icg_helper::LoadShaders("quad_vshader.glsl",
                                                  "quad_fshader.glsl");

            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                const GLfloat vertex_point[] = { /*V1*/ -1.0f, -1.0f, 0.0f,
                                                 /*V2*/ +1.0f, -1.0f, 0.0f,
                                                 /*V3*/ -1.0f, +1.0f, 0.0f,
                                                 /*V4*/ +1.0f, +1.0f, 0.0f};
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                             vertex_point, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // texture coordinates
            {
                const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f, 0.0f,
                                                               /*V2*/ 1.0f, 0.0f,
                                                               /*V3*/ 0.0f, 1.0f,
                                                               /*V4*/ 1.0f, 1.0f};

                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                             vertex_texture_coordinates, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,
                                                                     "vtexcoord");
                glEnableVertexAttribArray(vertex_texture_coord_id);
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE,
                                      ZERO_BUFFER_OFFSET);
            }

            // load texture
            if (texture_filename != NULL)
            {
                int width;
                int height;
                int nb_component;
                // set stb_image to have the same coordinates as OpenGL
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image = stbi_load(texture_filename, &width,
                                                 &height, &nb_component, 0);

                if(image == nullptr) {
                    throw(string("Failed to load texture"));
                }

                glGenTextures(1, &texture_id_);
                glBindTexture(GL_TEXTURE_2D, texture_id_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image);
                }

                GLuint tex_id = glGetUniformLocation(program_id_, "tex");
                glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(image);
            }

            // Uniforms
            {
                MVP_id_ = glGetUniformLocation(program_id_, "MVP");
                time_id = glGetUniformLocation(program_id_, "time");
            }

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteTextures(1, &texture_id_);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);

            // Position + Size
            glm::mat4 PS = IDENTITY_MATRIX;
            PS = glm::translate(PS, glm::vec3(x, y, 0.0f));
            PS = glm::scale(PS, glm::vec3(width / 2.0f, height / 2.0f, 1.0f));

            // setup MVP
            glm::mat4 MVP = projection*view*model*PS;
            glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

            glUniform1f(time_id, glfwGetTime());

            // glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // glDisable(GL_BLEND);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
