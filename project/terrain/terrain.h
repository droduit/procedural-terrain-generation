#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class Terrain {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID
        GLuint texture_id_, texture_color_id_;                     // texture ID
        GLuint num_indices_;                    // number of vertices to render

        GLuint projection_id_, view_id_, model_id_;
        GLuint light_pos_id_;

        vec3 light_pos_;

        int grid_tesselation_;
        float grid_area_;

    public:
        float diffuse_ = 0.5f, specular_ = 0.8f, alpha_ = 60.0f;
        float hsnow_ = 0.8f, fsnow_ = 2.0f;
        float fheight_ = 2.4f, fslope_ = 1.2f, fcolor_ = 0.8333f;
        bool wireframe_mode_ = false;

        void Init(GLuint heightmap_texture_id, int grid_tesselation, float grid_area) {
            grid_tesselation_ = grid_tesselation;
            grid_area_ = grid_area;

            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("terrain_vshader.glsl",
                                                  "terrain_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates and indices
            {
                // The lambda requires local variables
                int grid_tesselation_ = this->grid_tesselation_;
                int grid_area_ = this->grid_area_;

                std::vector<GLfloat> vertices;
                std::vector<GLuint> indices;

                float delta = (float)grid_area_ / (float)grid_tesselation_;
                for (int y = 0; y <= grid_tesselation_; ++y) {
                    for (int x = 0; x <= grid_tesselation_; ++x) {
                        vertices.push_back(-grid_area_ / 2.0f + delta*x); // x coordinate
                        vertices.push_back(-grid_area_ / 2.0f + delta*y); // y coordinate
                    }
                }

                // helper function to convert x and y coordinate to an index
                auto to_index = [grid_tesselation_](int x, int y) {
                    return x + y * (grid_tesselation_ + 1);
                };

                for (int y = 0; y < grid_tesselation_; ++y) {
                    for (int x = 0; x < grid_tesselation_; ++x) {
                        if (y % 2 == 0) { // forward (normal)
                            if (x == 0) { // initial points at the beginning of the line
                                indices.push_back(to_index(x, y));
                                indices.push_back(to_index(x, y+1));
                            }
                            indices.push_back(to_index(x+1, y));
                            indices.push_back(to_index(x+1, y+1));

                        } else { // backward
                            int x_ = grid_tesselation_ - x - 1;
                            if (x == 0) { // initial points at the beginning of the line
                                indices.push_back(to_index(x_+1, y));
                                indices.push_back(to_index(x_+1, y+1));
                            }
                            indices.push_back(to_index(x_, y));
                            indices.push_back(to_index(x_, y+1));
                        }
                    }
                }

                num_indices_ = indices.size();

                // position buffer
                glGenBuffers(1, &vertex_buffer_object_position_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                             &vertices[0], GL_STATIC_DRAW);

                // vertex indices
                glGenBuffers(1, &vertex_buffer_object_index_);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                             &indices[0], GL_STATIC_DRAW);

                // position shader attribute
                GLuint loc_position = glGetAttribLocation(program_id_, "position");
                glEnableVertexAttribArray(loc_position);
                glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // load/Assign texture

            this->texture_id_ = heightmap_texture_id;
            glBindTexture(GL_TEXTURE_2D, texture_id_);
            GLuint tex_id = glGetUniformLocation(program_id_, "heightmap");
            glUniform1i(tex_id, 0);
            glBindTexture(GL_TEXTURE_2D, 0);


            // load texture for heights
            {
                int width;
                int height;
                int nb_component;
                string filename = "terrain_texture.tga";
                // set stb_image to have the same coordinates as OpenGL
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image = stbi_load(filename.c_str(), &width,
                                                 &height, &nb_component, 0);

                if(image == nullptr) {
                    throw(string("Failed to load texture"));
                }

                glGenTextures(1, &texture_color_id_);
                glBindTexture(GL_TEXTURE_2D, texture_color_id_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image);
                }

                GLuint tex_id = glGetUniformLocation(program_id_, "tex_color");
                glUniform1i(tex_id, 1 /*GL_TEXTURE0*/);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 1);
                stbi_image_free(image);
            }


            // other uniforms
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
            glDeleteBuffers(1, &vertex_buffer_object_position_);
            glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteProgram(program_id_);
            glDeleteTextures(1, &texture_color_id_);

        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id_);


            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_color_id_);


            // setup MVP
            glUniformMatrix4fv(projection_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));
            glUniformMatrix4fv(view_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
            glUniformMatrix4fv(model_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
            glUniform3fv(light_pos_id_, ONE, glm::value_ptr(light_pos_));

            glUniform1f(glGetUniformLocation(program_id_, "tesselation"), this->grid_tesselation_);
            glUniform1f(glGetUniformLocation(program_id_, "area"), this->grid_area_);

            glUniform1f(glGetUniformLocation(program_id_, "diffuse"), this->diffuse_);
            glUniform1f(glGetUniformLocation(program_id_, "specular"), this->specular_);
            glUniform1f(glGetUniformLocation(program_id_, "alpha"), this->alpha_);
            glUniform1f(glGetUniformLocation(program_id_, "fheight"), this->fheight_);
            glUniform1f(glGetUniformLocation(program_id_, "hsnow"), this->hsnow_);
            glUniform1f(glGetUniformLocation(program_id_, "fsnow"), this->fsnow_);
            glUniform1f(glGetUniformLocation(program_id_, "fslope"), this->fslope_);
            glUniform1f(glGetUniformLocation(program_id_, "fcolor"), this->fcolor_);

            // draw
            if (wireframe_mode_)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

            if (wireframe_mode_)
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glBindVertexArray(0);
            glUseProgram(0);
        }

        void SetLighting(vec3 light_pos) {
            this->light_pos_ = light_pos;
        }
};
