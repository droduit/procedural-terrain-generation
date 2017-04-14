#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
//#include <SOIL.h>

GLfloat skyboxVertices[] = {
    // Positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};


class SkyBox {

    private:
        GLuint program_id_;             // GLSL shader program ID
        GLuint skyboxVAO, skyboxVBO;

        GLuint projection_id_, view_id_, model_id_;
        GLuint light_pos_id_;

        vec3 light_pos_ = vec3(0.0f);

    public:
        void Init() {
            glDepthMask(GL_FALSE);

            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("skybox_vshader.glsl",
                                                  "skybox_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);


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
        }

        // Loads a cubemap texture from 6 individual texture faces
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

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);

            glDeleteBuffers(1, &skyboxVBO);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &skyboxVAO);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX){


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
        }

};
