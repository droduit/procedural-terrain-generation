#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

static const unsigned int NbCubeVertices = 36;
static const glm::vec3 CubeVertices[] =
{
    glm::vec3(-0.5, -0.5, -0.5),
    glm::vec3(-0.5, 0.5, -0.5),
    glm::vec3(0.5, -0.5, -0.5),
    glm::vec3(-0.5, 0.5, -0.5),
    glm::vec3(0.5, -0.5, -0.5),
    glm::vec3(0.5, 0.5, -0.5),
    glm::vec3(0.5, 0.5, 0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(0.5, 0.5, -0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(0.5, 0.5, -0.5),
    glm::vec3(0.5, -0.5, -0.5),
    glm::vec3(0.5, 0.5, 0.5),
    glm::vec3(-0.5, 0.5, 0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(-0.5, 0.5, 0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(-0.5, -0.5, 0.5),
    glm::vec3(-0.5, -0.5, 0.5),
    glm::vec3(-0.5, -0.5, -0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(-0.5, -0.5, -0.5),
    glm::vec3(0.5, -0.5, 0.5),
    glm::vec3(0.5, -0.5, -0.5),
    glm::vec3(-0.5, 0.5, -0.5),
    glm::vec3(-0.5, -0.5, -0.5),
    glm::vec3(-0.5, 0.5, 0.5),
    glm::vec3(-0.5, -0.5, -0.5),
    glm::vec3(-0.5, 0.5, 0.5),
    glm::vec3(-0.5, -0.5, 0.5),
    glm::vec3(0.5, 0.5, -0.5),
    glm::vec3(-0.5, 0.5, -0.5),
    glm::vec3(0.5, 0.5, 0.5),
    glm::vec3(-0.5, 0.5, -0.5),
    glm::vec3(0.5, 0.5, 0.5),
    glm::vec3(-0.5, 0.5, 0.5)
};


static const unsigned int NbCubeUVs = 36;

static const glm::vec2 CubeUVs[] =
{
    glm::vec2(0.333, 0.75),
    glm::vec2(0.666, 0.75),
    glm::vec2(0.333, 0.5),
    glm::vec2(0.666, 0.75),
    glm::vec2(0.333, 0.5),
    glm::vec2(0.666, 0.5),

    glm::vec2(0.666, 0.25),
    glm::vec2(0.333, 0.25),
    glm::vec2(0.666, 0.5),
    glm::vec2(0.333, 0.25),
    glm::vec2(0.666, 0.5),
    glm::vec2(0.333, 0.5),

    glm::vec2(0.666, 0.25),
    glm::vec2(0.666, 0.0),
    glm::vec2(0.333, 0.25),
    glm::vec2(0.666, 0.0),
    glm::vec2(0.333, 0.25),
    glm::vec2(0.333, 0.0),

    glm::vec2(0.0, 0.75),
    glm::vec2(0.333, 0.75),
    glm::vec2(0.0, 0.5),
    glm::vec2(0.333, 0.75),
    glm::vec2(0.0, 0.5),
    glm::vec2(0.333, 0.5),

    glm::vec2(0.666, 0.75),
    glm::vec2(0.333, 0.75),
    glm::vec2(0.666, 1.0),
    glm::vec2(0.333, 0.75),
    glm::vec2(0.666, 1.0),
    glm::vec2(0.333, 1.0),

    glm::vec2(0.666, 0.5),
    glm::vec2(0.666, 0.75),
    glm::vec2(1.0, 0.5),
    glm::vec2(0.666, 0.75),
    glm::vec2(1.0, 0.5),
    glm::vec2(1.0, 0.75)
};


class SkyBox {

    private:
    GLuint program_id_;             // GLSL shader program ID
    GLuint vertex_buffer_object_;   // memory buffer
    GLuint vertex_array_id_;     // vertex array object
    GLuint tex_id;                  // texture ID

    public:
        void Init() {
            //glDepthMask(GL_FALSE);

            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("skybox_vshader.glsl",
                                                  "skybox_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, NbCubeVertices * sizeof(glm::vec3), &CubeVertices[0], GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // texture coordinates
            {
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, NbCubeUVs * sizeof(glm::vec2), &CubeUVs[0], GL_STATIC_DRAW);

                // attribute
                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_, "vtexcoord");
                glEnableVertexAttribArray(vertex_texture_coord_id);
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // load texture
            {
                int width;
                int height;
                int nb_component;
                string texture_filename = "cube_texture.tga";
                stbi_set_flip_vertically_on_load(1);
                unsigned char* image = stbi_load(texture_filename.c_str(), &width, &height, &nb_component, 0);

                if(image == nullptr) {
                    throw(std::string("Failed to load texture"));
                }

                glGenTextures(1, &tex_id);
                glBindTexture(GL_TEXTURE_2D, tex_id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                if(nb_component == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                } else if(nb_component == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
                }

                GLuint tex_id = glGetUniformLocation(program_id_, "tex");
                glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

                // cleanup
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(image);

            }

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);

            /*
            // Setup skybox VAO
            glGenVertexArrays(1, &skyboxVAO);
            glGenBuffers(1, &skyboxVBO);
            glBindVertexArray(skyboxVAO);
            glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
            glBindVertexArray(0);


            // Cubemap (Skybox)
           vector<const GLchar*> faces;
           faces.push_back("skybox/right.tga");
           faces.push_back("skybox/left.tga");
           faces.push_back("skybox/top.tga");
           faces.push_back("skybox/bottom.tga");
           faces.push_back("skybox/back.tga");
           faces.push_back("skybox/front.tga");
           GLuint cubemapTexture = loadCubemap(faces);
           */
        }

        // Loads a cubemap texture from 6 individual texture faces
        /*
        GLuint loadCubemap(vector<const GLchar*> faces) {
            GLuint textureID;
            glGenTextures(1, &textureID);
            glActiveTexture(GL_TEXTURE0);

            int width,height;
            unsigned char* image;

            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
            for(GLuint i = 0; i < faces.size(); i++)
            {
                image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                    GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
                );
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            return textureID;
        }
        */


        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteTextures(1, &tex_id);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX){


            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // Keep in background
            glDisable(GL_DEPTH_TEST);

            // bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex_id);

            // setup MVP
            glm::mat4 MVP = projection * view * model;
            GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, glm::value_ptr(MVP));

            glDrawArrays(GL_TRIANGLES, 0, NbCubeVertices);

            glEnable(GL_DEPTH_TEST);
            glBindVertexArray(0);
            glUseProgram(0);


            /* -----------------------
            glUniformMatrix4fv(glGetUniformLocation(program_id_, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(glGetUniformLocation(program_id_, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(program_id_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

            // time
            glUniform1f(glGetUniformLocation(program_id_, "time"), glfwGetTime());

            // Draw skybox as last
            glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content

            glm::mat4 view3 = glm::mat4(glm::mat3(view));	// Remove any translation component of the view matrix
            glUniformMatrix4fv(glGetUniformLocation(program_id_, "view"), 1, GL_FALSE, glm::value_ptr(view3));
            glUniformMatrix4fv(glGetUniformLocation(program_id_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

            // skybox cube
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(program_id_, "skybox"), 0);
           // glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS); // Set depth function back to default

            glUseProgram(0);
            */
        }

};
