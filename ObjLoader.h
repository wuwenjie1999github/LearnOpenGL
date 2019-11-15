#pragma once
#include "GL\glew.h"
#include "GL\freeglut.h"
#include <vector>
#include <string>
#include "header/Camera.h"
using namespace std;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };

class ObjLoader {
public:
	ObjLoader(){}
	ObjLoader(string filename, string texturename, GLuint p);
	void Draw(mat4 view);
	GLuint SetProgram();
	vector<vector<GLfloat>> getvSets();
	vector<vector<GLint>> getfSets();
	vector<vector<GLfloat>> getvtSets();
	vector<vector<GLint>> getvtIndexSets();
	vector<vector<GLfloat>> getvnSets();
	vector<vector<GLint>> getvnIndexSets();

	void ColorObject();
	void init();
	void display();
	void move(GLfloat x, GLfloat y, GLfloat z);


	GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };
	mat4 model;
	int NumVertices;
private:
	vector<vector<GLfloat>>vSets;
	vector<vector<GLint>>fSets;
	vector<vector<GLfloat>>vtSets;
	vector<vector<GLint>>vtIndexSets;
	vector<vector<GLfloat>>vnSets;
	vector<vector<GLint>>vnIndexSets;

	
	point4* points;
	vec2* textures;
	vec3* normals;
	

	vec3 adjust_pos = vec3(0.0, 0.0, 0.0);

	GLuint program;
	string texturename;
	int Index = 0;
	GLuint vao;
	GLuint texture;

	vector<string> Tri(vector<string>);	// 三角化
	void CountNormal();	// 计算法向量


	
};