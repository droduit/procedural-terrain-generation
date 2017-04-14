#pragma once
#include "icg_helper.h"

#include "../framebuffer.h"

class Heightmap {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;             // texture ID

        Framebuffer framebuffer_;

        float screenquad_width_;
        float screenquad_height_;

    public:
        float dx_ = 1.5, dy_ = -1.5;
        float hcomp_ = 6.0, vcomp_ = 13.0, voffset_ = -8.5;
        float H_ = 0.5, lacunarity_ = 2.0, warp_ = 0.15;
        int type_ = 3, seed_ = 4, octaves_ = 8;

        GLuint Init(float screenquad_width, float screenquad_height) {
            // set screenquad size
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;

            this->texture_id_ = framebuffer_.Init(screenquad_width, screenquad_height, GL_R32F, GL_RED, GL_FLOAT, true);

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

            glUniform1f(glGetUniformLocation(program_id_, "dx"), this->dx_);
            glUniform1f(glGetUniformLocation(program_id_, "dy"), this->dy_);
            glUniform1f(glGetUniformLocation(program_id_, "hcomp"), this->hcomp_);
            glUniform1f(glGetUniformLocation(program_id_, "vcomp"), this->vcomp_);
            glUniform1f(glGetUniformLocation(program_id_, "voffset"), this->voffset_);

            glUniform1i(glGetUniformLocation(program_id_, "seed"), this->seed_);

            glUniform1i(glGetUniformLocation(program_id_, "type"), this->type_);
            glUniform1f(glGetUniformLocation(program_id_, "H"), this->H_);
            glUniform1f(glGetUniformLocation(program_id_, "lacunarity"), this->lacunarity_);
            glUniform1f(glGetUniformLocation(program_id_, "warp"), this->warp_);
            glUniform1i(glGetUniformLocation(program_id_, "octaves"), this->octaves_);

            // draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindVertexArray(0);
            glUseProgram(0);

            framebuffer_.Unbind();
        }
};
