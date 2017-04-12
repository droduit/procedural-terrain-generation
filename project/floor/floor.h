#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

class Floor {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;             // texture ID
        GLuint texture_mirror_id_;      // texture mirror ID
        GLuint heightmap_texture_id_;

        GLuint projection_id_, view_id_, model_id_;
        GLuint light_pos_id_;

        vec3 light_pos_ = vec3(0.0f);

    public:
        void Init(GLuint heightmap_texture_id, GLuint tex_mirror = -1) {
            // compile the shaders
            program_id_ = icg_helper::LoadShaders("floor_vshader.glsl",
                                                  "floor_fshader.glsl");
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

            this->heightmap_texture_id_ = heightmap_texture_id;
            glBindTexture(GL_TEXTURE_2D, heightmap_texture_id_);
            GLuint tex_id = glGetUniformLocation(program_id_, "heightmap");
            glUniform1i(tex_id, 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            /*
            {
                // load texture
                int width;
                int height;
                int nb_component;
                string filename = "floor_texture.tga";
                // set stb_image to have the same coordinates as OpenGL
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image = stbi_load(filename.c_str(), &width,
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


                texture_mirror_id_ = (tex_mirror==-1)? texture_id_ : tex_mirror;

                // texture uniforms
                GLuint tex_id = glGetUniformLocation(program_id_, "tex");
                glUniform1i(tex_id, 0);
                GLuint tex_mirror_id = glGetUniformLocation(program_id_, "tex_mirror");
                glUniform1i(tex_mirror_id, 1);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 1);
                stbi_image_free(image);
            }
            */

            projection_id_ = glGetUniformLocation(program_id_, "projection");
            view_id_ = glGetUniformLocation(program_id_, "view");
            model_id_ = glGetUniformLocation(program_id_, "model");
            light_pos_id_ = glGetUniformLocation(program_id_, "light_pos");

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
            glDeleteTextures(1, &texture_mirror_id_);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);
            
            // bind textures
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_mirror_id_);
               
            // setup MVP
            glUniformMatrix4fv(projection_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));
            glUniformMatrix4fv(view_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
            glUniformMatrix4fv(model_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
            glUniform3fv(light_pos_id_, ONE, glm::value_ptr(light_pos_));

            // draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
