#pragma once
#include "icg_helper.h"

#include <glm/gtc/type_ptr.hpp>

class Triangle {

    private:
        GLuint vertex_array_id_; // vertex array object
        GLuint program_id_; // GLSL shader program ID
        GLuint vertex_buffer_object_; // memory buffer
        GLuint MVP_id_; // MVP matrix

    public:
        void Init() {
            // compile the shaders
            program_id_ = icg_helper::LoadShaders("triangle_vshader.glsl",
                                                  "triangle_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // Vertex Buffer
            const GLfloat vertex_point[] = { /*V1*/-1.0f, -1.0f, 0.0f,
                                             /*V2*/ 1.0f, -1.0f, 0.0f,
                                             /*V3*/ 0.0f,  1.0f, 0.0f};
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                         vertex_point, GL_STATIC_DRAW);

            // vertex point shader attribute
            // fetch Attribute ID for Vertex Positions
            GLuint position = glGetAttribLocation(program_id_, "vpoint");
            glEnableVertexAttribArray(position); // enable it
            glVertexAttribPointer(position, 3, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);


            // Model View Project uniform
            {
                MVP_id_ = glGetUniformLocation(program_id_, "MVP");
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
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // setup MVP
            glm::mat4 MVP = model*view*projection;
            glUniformMatrix4fv(MVP_id_, ONE, GL_FALSE, glm::value_ptr(MVP));
            
            // draw
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
