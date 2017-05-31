#pragma once
#include "icg_helper.h"

class Shadows {

    private:
        int width_;
        int height_;
        GLuint framebuffer_object_id_;
        GLuint depth_texture_id_;

        //static GLenum buffer_count_ = 0;

    public:
        // warning: overrides viewport!!
        void Bind() {
            glViewport(0, 0, width_, height_);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
        }

        void Unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        int Init(int image_width, int image_height) {
            this->width_ = image_width;
            this->height_ = image_height;

            // create depth attachment
            {
                glGenTextures(1, &depth_texture_id_);
                glBindTexture(GL_TEXTURE_2D, depth_texture_id_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                // create texture for the depth attachment
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0,
                             GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            }

            // tie it all together
            {
                glGenFramebuffers(1, &framebuffer_object_id_);
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id_);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id_, 0);
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
                    GL_FRAMEBUFFER_COMPLETE) {
                    cerr << "!!!ERROR: Framebuffer not OK :(" << endl;
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0); // avoid pollution
            }

            return depth_texture_id_;
        }

        void Cleanup() {
            glDeleteTextures(1, &depth_texture_id_);
            glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
            glDeleteFramebuffers(1, &framebuffer_object_id_);
        }
};
