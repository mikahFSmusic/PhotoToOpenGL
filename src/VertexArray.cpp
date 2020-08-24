#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"
#include <iostream>

VertexArray::VertexArray()
{
    GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
    Bind();

    // TODO: IT BREAKS HERE -- WHY?

    vb.Bind();
    // gets elements, automatically types
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        // gets current element in loop, automatically types the variable
        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(i));

        // Type Associations for debugging
        std::cout << "GL_FLOAT: " << GL_FLOAT << std::endl;
        std::cout << "GL_UNSIGNED_INT: " << GL_UNSIGNED_INT << std::endl;
        std::cout << "GL_UNSIGNED_BYTE: " << GL_UNSIGNED_BYTE << std::endl;

        // Print out for debugging
        std::cout << "Count: " << element.count << std::endl;
        std::cout << "Type: " << element.type << std::endl;
        std::cout << "Norm: " << element.normalized << std::endl;
        std::cout << "Stride: " << layout.GetStride() << std::endl;
        std::cout << "Offset: " << offset << std::endl;
        // creates a pointer to the memory location of an attribute, must be enabled
        GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));
        // calculates new offset value by multiplying number of elements * size in bytes of the input type
        offset += VertexBufferElement::GetSizeOfType(element.type) * element.count;
    }
}

void VertexArray::Bind() const
{
    GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
    GLCall(glBindVertexArray(0));
}
