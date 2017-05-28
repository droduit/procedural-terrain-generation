#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

// Taken from https://learnopengl.com/#!Advanced-OpenGL/Cubemaps
GLuint loadCubemap(vector<const GLchar*> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    int width,height;
    unsigned char* image;
    int nb_components;

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for(GLuint i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(1);
        image = stbi_load(faces.at(i), &width, &height, &nb_components, 0);

        if (image == nullptr)
            throw(std::string("Failed to load texture"));

        int color_mode = nb_components == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
            color_mode, width, height, 0, color_mode, GL_UNSIGNED_BYTE, image);

        stbi_image_free(image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

class SkyBox {

    private:
	    GLuint program_id_;             // GLSL shader program ID
	    GLuint vertex_array_id_;                // vertex array object
	    GLuint vertex_buffer_object_position_;  // memory buffer for positions
	    GLuint vertex_buffer_object_index_;     // memory buffer for indices
	    GLuint num_indices_;

    public:
        float hour_ = 12.0, sun_delta_ = 0.999;
        float rotX_ = -M_PI/2, rotY_ = 0.0f, rotZ_ = 0.0f;

        void Init() {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("skybox_vshader.glsl",
                                                  "skybox_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            glDepthMask(GL_FALSE);

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                // Position buffer
                const GLfloat position[] = {
                    -1.0f, -1.0f,  1.0f,  // 0, left, bottom, front
                     1.0f, -1.0f,  1.0f,  // 1, right, bottom, front
                     1.0f,  1.0f,  1.0f,  // 2, right, top, front
                    -1.0f,  1.0f,  1.0f,  // 3, left, top, front
                    -1.0f, -1.0f, -1.0f,  // 4, left, bottom, back
                     1.0f, -1.0f, -1.0f,  // 5, right, bottom, back
                     1.0f,  1.0f, -1.0f,  // 6, right, top, back
                    -1.0f,  1.0f, -1.0f,  // 7, left, top, back
                };

                // buffer
                glGenBuffers(1, &vertex_buffer_object_position_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

                // index buffer
                const GLuint index[] = {
                    0, 1, 2,  // front face triangle 1
                    0, 2, 3,  // front face triangle 2
                    0, 3, 4,  // left bottom
                    3, 7, 4,  // left top
                    1, 5, 2,  // right bottom
                    2, 6, 5,  // right top
                    0, 4, 1,  // bottom right
                    1, 4, 5,  // bottom left
                    4, 6, 5,  // back bottom
                    4, 7, 6,  // back top
                    2, 7, 3,  // top left
                    2, 6, 7   // top right
                };

                num_indices_ = sizeof(index) / sizeof(GLuint);

                glGenBuffers(1, &vertex_buffer_object_index_);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
            }

            // load texture
            {
                vector<const GLchar*> faces;
                faces.push_back("tex_sky_right.tga");
                faces.push_back("tex_sky_left.tga");
                faces.push_back("tex_sky_bottom.tga");
                faces.push_back("tex_sky_top.tga");
                faces.push_back("tex_sky_back.tga");
                faces.push_back("tex_sky_front.tga");
                GLuint cubemapTexture = loadCubemap(faces);

                glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glDepthMask(GL_TRUE);
            glUseProgram(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
			glDeleteBuffers(1, &vertex_buffer_object_position_);
			glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
        }

        void Draw(const vec3 cam_pos = vec3(0.0f),
                  const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX){

            float time_s = glfwGetTime();

            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // Keep in background
            glDisable(GL_DEPTH_TEST);

            // setup MVP
            glm::mat4 MVP = mat4(1.f);
            MVP = glm::rotate(MVP, rotX_, glm::vec3(1.0f, 0.0f, 0.0f));
            MVP = glm::rotate(MVP, time_s*rotY_, glm::vec3(0.0f, 1.0f, 0.0f));
            MVP = glm::rotate(MVP, rotZ_, glm::vec3(0.0f, 0.0f, 1.0f));
            MVP = projection * view * MVP;

            GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, glm::value_ptr(MVP));

            glUniform3fv(glGetUniformLocation(program_id_, "cam_pos"), ONE, glm::value_ptr(cam_pos));

            glUniform1f(glGetUniformLocation(program_id_, "hour"), this->hour_);
            glUniform1f(glGetUniformLocation(program_id_, "sun_delta"), this->sun_delta_);
            glUniform1f(glGetUniformLocation(program_id_, "rotX"), this->rotX_);
            glUniform1f(glGetUniformLocation(program_id_, "rotY"), this->rotY_);
            glUniform1f(glGetUniformLocation(program_id_, "rotZ"), this->rotZ_);

            glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

            glEnable(GL_DEPTH_TEST);
            glBindVertexArray(0);
            glUseProgram(0);
        }

};
