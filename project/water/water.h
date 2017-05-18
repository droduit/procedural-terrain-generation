#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

class Water {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_mirror_id_;      // texture mirror ID
        GLuint heightmap_texture_id_;
        GLuint reflection_texture_id_;
        GLuint normal_map_id_; // normal map texture id

        GLuint projection_id_, view_id_, model_id_;
        GLuint light_pos_id_;

        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_; // memory buffer for indices
        GLuint num_indices_; // number of vertices to render

        vec3 light_pos_ = vec3(0.0f);

        float *dx_ = nullptr;
        float *dy_ = nullptr;

        int grid_tesselation_;
        float grid_area_;
        float fb_ratio_;

    public:
        float diffuse_ = 0.5f, specular_ = 0.8f, alpha_ = 60.0f;
        vec3 cam_pos_, fog_color_;
        float fog_start_ = 80.0f, fog_end_ = 100.0f, fog_density_ = 0.004f, fog_power_ = 6.0f;
        int fog_type_ = 1;

        void Init(GLuint heightmap_texture_id, GLuint reflection_texture_id, int grid_tesselation, float grid_area, float fb_ratio, float *dx, float *dy) {
            grid_tesselation_ = grid_tesselation;
            grid_area_ = grid_area;
            fb_ratio_ = fb_ratio;

            dx_ = dx;
            dy_ = dy;

            // compile the shaders
            program_id_ = icg_helper::LoadShaders("water_vshader.glsl",
                                                  "water_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
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

            {
                int width;
                int height;
                int nb_component;
                string filename = "normal_map.jpg";
                // set stb_image to have the same coordinates as OpenGL
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image = stbi_load(filename.c_str(), &width,
                                                 &height, &nb_component, 0);

                if(image == nullptr) {
                    throw(string("Failed to load texture"));
                }

                glGenTextures(1, &normal_map_id_);
                glBindTexture(GL_TEXTURE_2D, normal_map_id_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, image);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image);
                }

                GLuint tex_id = glGetUniformLocation(program_id_, "normal_map");
                glUniform1i(tex_id, 2);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 2);
                stbi_image_free(image);
            }

            this->heightmap_texture_id_ = heightmap_texture_id;
            glBindTexture(GL_TEXTURE_2D, heightmap_texture_id_);
            GLuint heigh_tex_id = glGetUniformLocation(program_id_, "heightmap");
            glUniform1i(heigh_tex_id, 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            this->reflection_texture_id_ = reflection_texture_id;
            glBindTexture(GL_TEXTURE_2D, reflection_texture_id_);
            GLuint refl_tex_id = glGetUniformLocation(program_id_, "reflection");
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glUniform1i(refl_tex_id, 1);
            glBindTexture(GL_TEXTURE_2D, 1);

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
            glDeleteTextures(1, &normal_map_id_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);

        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // heightmap
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, heightmap_texture_id_);

            // reflection
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, reflection_texture_id_);

            // normal map
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, normal_map_id_);

            // setup MVP;
            glUniformMatrix4fv(projection_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));
            glUniformMatrix4fv(view_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
            glUniformMatrix4fv(model_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
            glUniform3fv(light_pos_id_, ONE, glm::value_ptr(light_pos_));

            glUniform3fv(glGetUniformLocation(program_id_, "cam_pos"), ONE, glm::value_ptr(cam_pos_));

            glUniform1f(glGetUniformLocation(program_id_, "diffuse"), this->diffuse_);
            glUniform1f(glGetUniformLocation(program_id_, "specular"), this->specular_);
            glUniform1f(glGetUniformLocation(program_id_, "alpha"), this->alpha_);

            glUniform3fv(glGetUniformLocation(program_id_, "fog_color"), ONE, glm::value_ptr(fog_color_));
            glUniform1f(glGetUniformLocation(program_id_, "fog_start"), this->fog_start_);
            glUniform1f(glGetUniformLocation(program_id_, "fog_end"), this->fog_end_);
            glUniform1i(glGetUniformLocation(program_id_, "fog_type"), this->fog_type_);
            glUniform1f(glGetUniformLocation(program_id_, "fog_density"), this->fog_density_);
            glUniform1f(glGetUniformLocation(program_id_, "fog_power"), this->fog_power_);


            glUniform1f(glGetUniformLocation(program_id_, "water_fb_ratio"), fb_ratio_);

            glUniform1f(glGetUniformLocation(program_id_, "dx"), *this->dx_);
            glUniform1f(glGetUniformLocation(program_id_, "dy"), *this->dy_);

            glUniform1f(glGetUniformLocation(program_id_, "tesselation"), this->grid_tesselation_);
            glUniform1f(glGetUniformLocation(program_id_, "area"), this->grid_area_);
            glUniform1f(glGetUniformLocation(program_id_, "time"), glfwGetTime());

            // draw
            glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }

        void SetReflection(GLuint new_reflection_tex_id) {
            this->reflection_texture_id_ = new_reflection_tex_id;
        }
};
