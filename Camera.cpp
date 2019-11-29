#include "header/Angel.h"
#include "header/Camera.h"


Camera::Camera()
{
	camera_front.x = sin(DegreesToRadians * yaw) * cos(DegreesToRadians * pitch);
	camera_front.y = -sin(DegreesToRadians * pitch);
	camera_front.z = -cos(DegreesToRadians * yaw) * cos(DegreesToRadians * pitch);
	camera_front = normalize(camera_front);
}

void Camera::setProgram(GLuint program)
{
	this->program = program;
}


