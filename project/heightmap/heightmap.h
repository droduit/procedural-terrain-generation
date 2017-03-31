#pragma once
#include "icg_helper.h"

#include "../framebuffer.h"

class Heightmap {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;             // texture ID

        float seed_;
        float screenquad_width_;
        float screenquad_height_;
        float dx_, dy_;

        Framebuffer framebuffer_;

    public:
        GLuint Init(float screenquad_width, float screenquad_height, float seed = 0.0) {
            this->dx_ = 0;
            this->dy_ = 0;

            this->seed_ = seed;

            // set screenquad size
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;

            this->texture_id_ = framebuffer_.Init(screenquad_width, screenquad_height, GL_R32F, GL_RED, GL_FLOAT);

            // compile the shaders
            program_id_ = icg_helper::LoadShaders("heightmap_vshader.glsl",
                                                  "heightmap_fshader.glsl");
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

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);

            // finally draw the heighmap
            Draw();

            return this->texture_id_;
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);

            framebuffer_.Cleanup();
        }

        void Draw() {
            framebuffer_.Bind();

            glViewport(0, 0, screenquad_width_, screenquad_height_);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // window size uniforms
            /*
            glUniform1f(glGetUniformLocation(program_id_, "tex_width"),
                        this->screenquad_width_);
            glUniform1f(glGetUniformLocation(program_id_, "tex_height"),
                        this->screenquad_height_);
                        */

            // bind texture
            /*
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);
            */

            glUniform1f(glGetUniformLocation(program_id_, "seed"), this->seed_);
            glUniform1f(glGetUniformLocation(program_id_, "dx"), this->dx_);
            glUniform1f(glGetUniformLocation(program_id_, "dy"), this->dy_);

            // draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindVertexArray(0);
            glUseProgram(0);

            framebuffer_.Unbind();
        }

        void Move(float dx, float dy) {
            this->dx_ = dx;
            this->dy_ = dy;

            this->Draw();
        }
};
