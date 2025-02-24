#include "framebuffer.h"
#include "../io/window.h"

#include <glm/glm.hpp>

namespace Brainstorm {
    Attachment::Attachment(AttachmentType type, GLint filter, GLint clamp) : texture(0), type(type), filter(filter), clamp(clamp) {}

    void FrameBuffer::createAttachments() {
        bool hasDepthAttachment = false;
        GLenum lastAttachmentId = 0;

        std::vector<GLenum> attachmentIds;
        for (size_t i = 0; i < this->attachments.size(); i++) {
            Attachment& attachment = this->attachments[i];

            glGenTextures(1, &attachment.texture);
            glBindTexture(GL_TEXTURE_2D, attachment.texture);

            GLint internalFormat = 0;

            GLenum format = 0;
            GLenum type = 0;
            GLenum attachmentId = 0;

            switch (attachment.type) {
                case AttachmentType::DEPTH: {
                    internalFormat = GL_DEPTH_COMPONENT;
                    format = GL_DEPTH_COMPONENT;
                    type = GL_FLOAT;
                    attachmentId = GL_DEPTH_ATTACHMENT;

                    hasDepthAttachment = true;

                    break;
                }
                case AttachmentType::COLOR_RGBA: {
                    internalFormat = GL_RGBA;
                    format = GL_RGBA;
                    type = GL_UNSIGNED_BYTE;
                    attachmentId = GL_COLOR_ATTACHMENT0 + lastAttachmentId++;

                    break;
                }
                case AttachmentType::COLOR_RGB: {
                    internalFormat = GL_RGB;
                    format = GL_RGB;
                    type = GL_UNSIGNED_BYTE;
                    attachmentId = GL_COLOR_ATTACHMENT0 + lastAttachmentId++;
                
                    break;
                }
                case AttachmentType::NORMAL: {
                    internalFormat = GL_RGB16F;
                    format = GL_RGB;
                    type = GL_FLOAT;
                    attachmentId = GL_COLOR_ATTACHMENT0 + lastAttachmentId++;

                    break;
                }
            }


            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, attachment.clamp);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, attachment.clamp);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, attachment.filter - GL_NEAREST + GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, attachment.filter);
            
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, this->width, this->height, 0, format, type, nullptr);

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentId, GL_TEXTURE_2D, attachment.texture, 0);
            this->attachments[i] = attachment;

            if (attachment.type != AttachmentType::DEPTH) attachmentIds.push_back(attachmentId);
        }

        if (!hasDepthAttachment) {
            glGenRenderbuffers(1, &this->depthId);
            glBindRenderbuffer(GL_RENDERBUFFER, this->depthId);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->width, this->height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthId);
        }

        glDrawBuffers(static_cast<GLsizei>(attachmentIds.size()), attachmentIds.data());
    }

    FrameBuffer::FrameBuffer(const std::vector<Attachment>& attachments, GLsizei width, GLsizei height)
            : attachments(attachments), width(width), height(height) {
        glGenFramebuffers(1, &this->id);
        glBindFramebuffer(GL_FRAMEBUFFER, this->id);

        this->createAttachments();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    FrameBuffer::~FrameBuffer() {
        this->clear();
    }

    void FrameBuffer::use() const {
        glBindFramebuffer(GL_FRAMEBUFFER, this->id);
        glViewport(0, 0, this->width, this->height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    void FrameBuffer::drop() const {
        for (const Attachment& attachment : this->attachments) {
            glBindTexture(GL_TEXTURE_2D, attachment.texture);
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::getFrameBufferWidth(), Window::getFrameBufferHeight());
    }
    void FrameBuffer::drop(GLsizei previousWidth, GLsizei previousHeight) const {
        for (const Attachment& attachment : this->attachments) {
            glBindTexture(GL_TEXTURE_2D, attachment.texture);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, previousWidth, previousHeight);
    }
    void FrameBuffer::clear() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        for (const Attachment& attachment : this->attachments) {
            glDeleteTextures(1, &attachment.texture);
        }
        
        glDeleteRenderbuffers(1, &this->depthId);
        glDeleteFramebuffers(1, &this->id);
    }
    void FrameBuffer::resize(GLsizei width, GLsizei height) {
        for (const Attachment& attachment : this->attachments) {
            glDeleteTextures(1, &attachment.texture);
        }
        glDeleteRenderbuffers(1, &this->depthId);

        this->use();

        this->width = width;
        this->height = height;

        this->createAttachments();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    GLint FrameBuffer::getTexture(size_t attachment) const {
        if (attachment >= this->attachments.size()) return 0;
        return this->attachments[attachment].texture;
    }

    GLsizei FrameBuffer::getWidth() const {
        return this->width;
    }
    GLsizei FrameBuffer::getHeight() const {
        return this->height;
    }

    glm::vec2 FrameBuffer::getSize() const {
        return glm::vec2(
            static_cast<float>(this->width),
            static_cast<float>(this->height)
        );
    }
}