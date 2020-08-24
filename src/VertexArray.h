#pragma once

#include "VertexBuffer.h"

class VertexBufferLayout;

class VertexArray
{
private: 
	unsigned int m_RendererID;
public: 
	VertexArray();
	~VertexArray();
	
	// adds a vertex buffer to the array, and specifies it's layout
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind() const;
	void Unbind() const;
};