// Copyright (C) 2016 - LGG EPFL

#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>

// GL Error checking
#include "check_error_gl.h"

// Small library to load images
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Small library to load obj files
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// we use GLM for linear algebra
#define GLM_FORCE_CXX11
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

// these namespaces assumed by default
using namespace std;

// convenience constants
static const int ONE = 1;
static const bool DONT_NORMALIZE = false;
static const bool DONT_TRANSPOSE = false;
static const int ZERO_STRIDE = 0;
static const void* ZERO_BUFFER_OFFSET = 0;
static const glm::mat4 IDENTITY_MATRIX = glm::mat4(1.0f);

// on some OSs the exit flags are not defined
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

namespace icg_helper {

// compiles the vertex, geometry and fragment shaders stored in the given strings
inline GLuint CompileShaders(const char* vshader,
                             const char* fshader,
                             const char* gshader = NULL,
                             const char* tcshader = NULL,
                             const char* teshader = NULL) {
    const int SHADER_LOAD_FAILED = 0;
    GLint success = GL_FALSE;
    int info_log_length;

    // create the Vertex Shader
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // compile Vertex Shader
    fprintf(stdout, "Compiling Vertex shader: ");
    char const * vertex_source_pointer = vshader;
    glShaderSource(vertex_shader_id, 1, &vertex_source_pointer , NULL);
    glCompileShader(vertex_shader_id);

    // check Vertex Shader
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if(!success) {
        vector<char> vertex_shader_error_message(info_log_length);
        glGetShaderInfoLog(vertex_shader_id, info_log_length, NULL,
                           &vertex_shader_error_message[0]);
        fprintf(stdout, "Failed:\n%s\n", &vertex_shader_error_message[0]);
        return SHADER_LOAD_FAILED;
    }
    else{
        fprintf(stdout, "Success\n");
    }

    GLuint tessellation_control_shader_id = 0;
    if(tcshader != NULL) {
        // create the Shader
        tessellation_control_shader_id = glCreateShader(GL_TESS_CONTROL_SHADER);

        // compile Shader
        fprintf(stdout, "Compiling Tesselation Control shader: ");
        char const * tessellation_control_source_pointer = tcshader;
        glShaderSource(tessellation_control_shader_id, 1,
                       &tessellation_control_source_pointer , NULL);
        glCompileShader(tessellation_control_shader_id);

        // check Shader
        glGetShaderiv(tessellation_control_shader_id, GL_COMPILE_STATUS, &success);
        glGetShaderiv(tessellation_control_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if(!success) {
            vector<char> tessellation_control_shader_error_message(info_log_length);
            glGetShaderInfoLog(tessellation_control_shader_id, info_log_length,
                               NULL, &tessellation_control_shader_error_message[0]);
            fprintf(stdout, "Failed:\n%s\n", &tessellation_control_shader_error_message[0]);
            return SHADER_LOAD_FAILED;
        }
        else
            fprintf(stdout, "Success\n");
    }

    GLuint tessellation_evaluation_shader_id = 0;
    if(teshader != NULL) {
        // create the Shader
        tessellation_evaluation_shader_id = glCreateShader(GL_TESS_EVALUATION_SHADER);

        // compile Shader
        fprintf(stdout, "Compiling Tesselation Evaluation shader: ");
        char const * tessellation_evaluation_source_pointer = teshader;
        glShaderSource(tessellation_evaluation_shader_id, 1,
                       &tessellation_evaluation_source_pointer , NULL);
        glCompileShader(tessellation_evaluation_shader_id);

        // check Shader
        glGetShaderiv(tessellation_evaluation_shader_id, GL_COMPILE_STATUS, &success);
        glGetShaderiv(tessellation_evaluation_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if(!success) {
            vector<char> tessellation_evaluation_shader_error_message(info_log_length);
            glGetShaderInfoLog(tessellation_evaluation_shader_id, info_log_length,
                               NULL, &tessellation_evaluation_shader_error_message[0]);
            fprintf(stdout, "Failed:\n%s\n", &tessellation_evaluation_shader_error_message[0]);
            return SHADER_LOAD_FAILED;
        }
        else
            fprintf(stdout, "Success\n");
    }

    GLuint geometry_shader_id = 0;
    if(gshader != NULL) {
        // create the Geometry Shader
        geometry_shader_id = glCreateShader(GL_GEOMETRY_SHADER);

        // compile Geometry Shader
        fprintf(stdout, "Compiling Geometry shader: ");
        char const * geometry_source_pointer = gshader;
        glShaderSource(geometry_shader_id, 1, &geometry_source_pointer , NULL);
        glCompileShader(geometry_shader_id);

        // check Geometry Shader
        glGetShaderiv(geometry_shader_id, GL_COMPILE_STATUS, &success);
        glGetShaderiv(geometry_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if(!success) {
            vector<char> geometry_shader_error_message(info_log_length);
            glGetShaderInfoLog(geometry_shader_id, info_log_length, NULL,
                               &geometry_shader_error_message[0]);
            fprintf(stdout, "Failed:\n%s\n", &geometry_shader_error_message[0]);
            return SHADER_LOAD_FAILED;
        }
        else
            fprintf(stdout, "Success\n");
    }

    // create the Fragment Shader
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // compile Fragment Shader
    fprintf(stdout, "Compiling Fragment shader: ");
    char const * fragment_source_pointer = fshader;
    glShaderSource(fragment_shader_id, 1, &fragment_source_pointer , NULL);
    glCompileShader(fragment_shader_id);

    // check Fragment Shader
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
    glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if(!success) {
        vector<char> fragment_shader_error_message(info_log_length);
        glGetShaderInfoLog(fragment_shader_id, info_log_length, NULL,
                           &fragment_shader_error_message[0]);
        fprintf(stdout, "Failed:\n%s\n", &fragment_shader_error_message[0]);
        return SHADER_LOAD_FAILED;
    }
    else
        fprintf(stdout, "Success\n");


    // Link the program
    fprintf(stdout, "Linking shader program: ");
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    if(tcshader != NULL) glAttachShader(program_id, tessellation_control_shader_id);
    if(teshader != NULL) glAttachShader(program_id, tessellation_evaluation_shader_id);
    if(gshader  != NULL) glAttachShader(program_id, geometry_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    // Check the program
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    vector<char> program_error_message(max(info_log_length, int(1)));
    glGetProgramInfoLog(program_id, info_log_length, NULL, &program_error_message[0]);
    if(!success) {
        fprintf(stdout, "Failed:\n%s\n", &program_error_message[0]);
        return SHADER_LOAD_FAILED;
    }
    else {
        fprintf(stdout, "Success\n");
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
    if(tcshader != NULL) glDeleteShader(tessellation_control_shader_id);
    if(teshader != NULL) glDeleteShader(tessellation_evaluation_shader_id);
    if(gshader != NULL) glDeleteShader(geometry_shader_id);

    // make sure you see the text in terminal
    fflush(stdout);

    return program_id;
}


// compiles the vertex, geometry and fragment shaders using file path
// TODO: add support for tessellation shaders
inline GLuint LoadShaders(const char * vertex_file_path,
                          const char * fragment_file_path,
                          const char * geometry_file_path = NULL) {
    const int SHADER_LOAD_FAILED = 0;

    string vertex_shader_code, fragment_shader_code, geometry_shader_code;
    {
        // read the Vertex Shader code from the file
        ifstream vertex_shader_stream(vertex_file_path, ios::in);
        if(vertex_shader_stream.is_open()) {
            vertex_shader_code = string(istreambuf_iterator<char>(vertex_shader_stream),
                                        istreambuf_iterator<char>());
            vertex_shader_stream.close();
        } else {
            printf("Could not open file: %s\n", vertex_file_path);
            return SHADER_LOAD_FAILED;
        }

        // read the Fragment Shader code from the file
        ifstream fragment_shader_stream(fragment_file_path, ios::in);
        if(fragment_shader_stream.is_open()) {
            fragment_shader_code = string(istreambuf_iterator<char>(fragment_shader_stream),
                                          istreambuf_iterator<char>());
            fragment_shader_stream.close();
        } else {
            printf("Could not open file: %s\n", fragment_file_path);
            return SHADER_LOAD_FAILED;
        }

        // read the Geometry Shader code from the file
        if(geometry_file_path != NULL) {
            ifstream geometry_shader_stream(geometry_file_path, ios::in);
            if(geometry_shader_stream.is_open()) {
                geometry_shader_code = string(istreambuf_iterator<char>(geometry_shader_stream),
                                              istreambuf_iterator<char>());
                geometry_shader_stream.close();
            } else {
                printf("Could not open file: %s\n", geometry_file_path);
                return SHADER_LOAD_FAILED;
            }
        }
    }

    // compile them
    char const *vertex_source_pointer = vertex_shader_code.c_str();
    char const *fragment_source_pointer = fragment_shader_code.c_str();
    char const *geometry_source_pointer = NULL;
    if(geometry_file_path != NULL) geometry_source_pointer = geometry_shader_code.c_str();

    int status = CompileShaders(vertex_source_pointer, fragment_source_pointer,
                                geometry_source_pointer);
    if(status == SHADER_LOAD_FAILED)
        printf("Failed linking:\n  vshader: %s\n  fshader: %s\n  gshader: %s\n",
               vertex_file_path, fragment_file_path, geometry_file_path);
    return status;
}
}
