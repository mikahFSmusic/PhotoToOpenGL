#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "vendors/glm/glm.hpp"
#include "vendors/glm/gtx/transform.hpp"

class Camera
{
public:
	Camera(glm::vec3& pos, float fov, float aspect, float z_Near, float z_Far) {
		m_Position = pos; 
		m_Perspective = glm::perspective(fov, aspect, z_Near, z_Far);
		m_Forward = glm::vec3(0, 0, 1);
		m_Up = glm::vec3(0, 1, 0);

	}

	inline glm::mat4 GetViewProjection() const {
		return m_Perspective * glm::lookAt(m_Position, m_Position + m_Forward, m_Up);

	}
protected: 
private: 
	glm::mat4 m_Perspective;
	glm::vec3 m_Position;
	glm::vec3 m_Forward;
	glm::vec3 m_Up;
};

#endif CAMERA_H_INCLUDED
