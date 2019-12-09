// 3DModelDemo_5.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "Header/Angel.h"
#include "Header/SOIL.h"
#include "Header/Camera.h"
#include "ObjLoader.h"
#include <winuser.h>
//#include <GLFW/glfw3.h>
#include <ctime>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

#define KEYDOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
#define KEYUP(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 0:1)

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

vector<ObjLoader*> objContainer;



Camera camera;

int width = 512, height = 512;

float lastX = width / 2;
float lastY = height / 2;



vec3 adjust_pos(0.0, 0.0, 0.0);

GLfloat aspect;
GLuint Projection;

GLuint program;

// OpenGL initialization
void
init()
{
	for (int i = 0; i < objContainer.size(); i++)
		objContainer[i]->init();


	// Retrieve transformation uniform variable locations

	glEnable(GL_DEPTH_TEST);

	glClearColor(0, 0.1, 0.1, 1.0);
}


void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//  Generate tha model-view matrixn

	mat4 view = LookAt(camera.camera_pos, camera.camera_pos + camera.camera_front, camera.camera_up);
	//mat4 view = LookAt(camera.camera_pos, vec3(0,0,0), camera.camera_up);

	
	point4 pointLightPositions[] = {
			point4(0, 0, 4, 1),
			point4(-4, 0, 0, 1),
			point4(0, 4, 0, 1)
	};
	
	for (int j = 0; j < 3; j++)
		pointLightPositions[j] = view * pointLightPositions[j];

	glUseProgram(program);
	glUniform4f(glGetUniformLocation(program, "material.diffuse"), 1.0, 1.0, 1.0, 1.0);
	glUniform4f(glGetUniformLocation(program, "material.specular"), 0.8, 0.8, 0.8, 1.0);
	glUniform1f(glGetUniformLocation(program, "material.shininess"), 50.0);

	// Point light 1
	glUniform3f(glGetUniformLocation(program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
	glUniform4f(glGetUniformLocation(program, "pointLights[0].ambient"), 0.1, 0.1, 0.1, 1.0);
	glUniform4f(glGetUniformLocation(program, "pointLights[0].diffuse"), 1, 0, 0, 1.0);
	glUniform4f(glGetUniformLocation(program, "pointLights[0].specular"), 0.5, 0.5, 0.5, 1.0);
	//Point light 2
	glUniform3f(glGetUniformLocation(program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
	glUniform4f(glGetUniformLocation(program, "pointLights[1].ambient"), 0.1, 0.1, 0.1, 1.0);
	glUniform4f(glGetUniformLocation(program, "pointLights[1].diffuse"), 0, 1, 0, 1.0);
	glUniform4f(glGetUniformLocation(program, "pointLights[1].specular"), 0.5, 0.5, 0.5, 1.0);
	// Point light 3
	glUniform3f(glGetUniformLocation(program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
	glUniform4f(glGetUniformLocation(program, "pointLights[2].ambient"), 0.1, 0.1, 0.1, 1.0);
	glUniform4f(glGetUniformLocation(program, "pointLights[2].diffuse"), 0, 0, 1, 1.0);
	glUniform4f(glGetUniformLocation(program, "pointLights[2].specular"), 0.5, 0.5, 0.5, 1.0);
	
	for (int i = 0; i < objContainer.size(); i++)
	{
		objContainer[i]->Draw(view);
	}

	glutSwapBuffers();
}

void
keyboard(unsigned char key, int x, int y)
{

	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}

	glutPostRedisplay();
}

void
mouse(int button, int state, int x, int y)
{
	lastX = x;
	lastY = y;
	/*if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
		case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
		case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
		}
	}*/
}

void mouse_move(int x, int y)
{

	float xoffset = x - lastX;
	float yoffset = y - lastY;
	lastX = x;
	lastY = y;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camera.yaw += xoffset;
	camera.pitch += yoffset;

	if (camera.pitch > 89.0f)
		camera.pitch = 89.0f;
	if (camera.pitch < -89.0f)
		camera.pitch = -89.0f;

	vec3 front;
	front.x = sin(DegreesToRadians * camera.yaw) * cos(DegreesToRadians * camera.pitch);
	front.y = -sin(DegreesToRadians * camera.pitch);
	front.z = -cos(DegreesToRadians * camera.yaw) * cos(DegreesToRadians * camera.pitch);
	camera.camera_front = normalize(front);
	glutPostRedisplay();
}


void
idle(void)
{
	float currentFrame = (float)clock();
	camera.deltaTime = currentFrame - camera.lastFrame;
	///cout << deltaTime << endl;
	camera.lastFrame = currentFrame;

	float rotateSpeed = 1.0;
	float cameraSpeed = camera.deltaTime / 100;
	if (cameraSpeed > 1)
		cameraSpeed = 0.2;

	for (int i = 0; i < objContainer.size(); i++)
	{
		if (KEYDOWN(74))	// 按下J
			objContainer[i]->Theta[Xaxis] += rotateSpeed;
		if (KEYDOWN(75))	// 按下K
			objContainer[i]->Theta[Yaxis] += rotateSpeed;
		if (KEYDOWN(76))	// 按下L
			objContainer[i]->Theta[Zaxis] += rotateSpeed;
	}
	if (KEYDOWN(87))	// 按下W
		camera.camera_pos += cameraSpeed * camera.camera_front;
	if (KEYDOWN(83))	// 按下S
		camera.camera_pos -= cameraSpeed * camera.camera_front;
	if (KEYDOWN(65))	// 按下A
		camera.camera_pos -= normalize(cross(camera.camera_front, camera.camera_up)) * cameraSpeed;
	if (KEYDOWN(68))	// 按下D
		camera.camera_pos += normalize(cross(camera.camera_front, camera.camera_up)) * cameraSpeed;


	//Theta[Axis] += 0.1;

	/*if (Theta[Axis] > 360.0) {
		Theta[Axis] -= 360.0;
	}*/

	glutPostRedisplay();
}

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	aspect = GLfloat(width) / height;
	mat4  projection = Perspective(45.0, aspect, 0.03, 4000);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}

int
main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("3D Model");

	glewInit();

	program = InitShader("Shader/vshader53.glsl", "Shader/fshader53.glsl");

	Projection = glGetUniformLocation(program, "Projection");

	vector<string> vector1;
	vector1.push_back("Textures/white.png");
	ObjLoader objModel_1 = ObjLoader("Model/1.obj", vector1, program);
	objModel_1.move(-1, 0, 0);
	objContainer.push_back(&objModel_1);

	vector<string> vector2;
	vector2.push_back("Textures/white.png");
	ObjLoader objModel_2 = ObjLoader("Model/1.obj", vector2, program);
	objModel_2.move(1, 0, 0);
	objContainer.push_back(&objModel_2);

	glUseProgram(program);
	// Load shaders and use the resulting shader program
	//camera.setProgram(program);

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_move);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}

