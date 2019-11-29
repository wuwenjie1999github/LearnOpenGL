#pragma once
#include"Angel.h"

class Camera
{
private:
	
public:
	GLuint program;

	vec3 camera_pos = vec3(-5.0, 5.0, 5.0);
	vec3 camera_front = vec3(0.0, 0.0, -1.0);
	vec3 camera_up = vec3(0.0, 1.0, 0.0);

	float deltaTime = 0.0f;	// ��ǰ֡����һ֡��ʱ���
	float lastFrame = 0.0f;	// ��һ֡��ʱ��

	float yaw = 45.0f;
	float pitch = 38.0f;

	Camera();
	void setProgram(GLuint program);
};