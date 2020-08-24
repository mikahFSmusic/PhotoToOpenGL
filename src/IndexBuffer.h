#pragma once

class IndexBuffer
{
private:
	// the unique id that identifies this specific gl object to the renderer
	unsigned int m_RendererID;
	// the count of the number of elements
	unsigned int m_Count; 

public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
	inline unsigned int GetCount() const { return m_Count; };
};