#include "VertexBuffer.h"
#include "Renderer.h"
#include <iostream>
VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    // generate one (1) buffer, output to buffer (&buffer is the pointer to our location in memory)
    GLCall(glGenBuffers(1, &m_RendererID));

    // bind array info to our buffer
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));

    // size in bytes, position data, and static draw (single buffer, used many times)
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
    std::cout << "BREAKING HERE" << std::endl;
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, (GLuint)m_RendererID));
}

void VertexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
