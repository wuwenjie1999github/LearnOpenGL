#include "ObjLoader.h"
#include "header/Angel.h"
#include "header/SOIL.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
using namespace std;

ObjLoader::ObjLoader(string filename, vector<string> textureContainer, GLuint p)
{
	string line;
	ifstream f;
	f.open(filename, ios::in);
	if (!f.is_open()) {
		cout << "Something Went Wrong When Opening Objfiles" << endl;
	}
	else
	{
		this->program = p;
		while (!f.eof()) {
			getline(f, line);
			vector<string>parameters;
			string tailMark = " ";
			string ans = "";

			line = line.append(tailMark);
			for (int i = 0; i < line.length(); i++) {
				char ch = line[i];
				if (ch != ' ') {
					ans += ch;
				}
				else if (ans != " " && ans != "") {
					parameters.push_back(ans);
					ans = "";
				}
			}
			if (parameters.empty())
				parameters.push_back(ans);

			if (parameters[0] == "v") {
				vector<GLfloat>Point;
				for (int i = 1; i < 4; i++) {
					GLfloat xyz = atof(parameters[i].c_str());
					Point.push_back(xyz);
				}
				vSets.push_back(Point);
			}

			else if (parameters[0] == "vn") {
				vector<GLfloat> normal;
				for (int i = 1; i < 4; i++)
				{
					GLfloat data = atof(parameters[i].c_str());
					normal.push_back(data);
				}
				vnSets.push_back(normal);
			}

			else if (parameters[0] == "vt") {
				vector<GLfloat> texture;
				for (int i = 1; i < parameters.size(); i++)
				{
					GLfloat data = atof(parameters[i].c_str());
					texture.push_back(data);
				}
				vtSets.push_back(texture);
			}

			else if (parameters[0] == "f") {
				vector<GLint>vIndexSets;
				vector<GLint>textureIndexSets;
				vector<GLint>normalIndexSets;
				int paraSize = (parameters.size() - 3) * 3;
				parameters = Tri(parameters);
				for (int i = 1; i <= paraSize; i++) {
					string x = parameters[i];
					string vAns = "";
					string vtAns = "";
					string vnAns = "";
					int tag = 1;
					for (int j = 0; j < x.length(); j++) {
						if (x[j] != '/')
						{
							if (tag == 1)	// 顶点索引
								vAns += x[j];
							else if (tag == 2)	// 纹理索引
								vtAns += x[j];
							else if (tag == 3)	// 法向量索引
								vnAns += x[j];
							else
								cerr << "Wrong format in obj file" << endl;
						}
						else
						{
							tag++;
						}
					}
					GLint index = atof(vAns.c_str());
					index--;
					vIndexSets.push_back(index);

					index = atof(vtAns.c_str());
					index--;
					index = index >= 0 ? index : 0;
					textureIndexSets.push_back(index);

					index = atof(vnAns.c_str());
					index--;
					if (index >= 0)
						normalIndexSets.push_back(index);

					if (i % 3 == 0)
					{
						fSets.push_back(vIndexSets);
						vtIndexSets.push_back(textureIndexSets);
						if (!normalIndexSets.empty())
							vnIndexSets.push_back(normalIndexSets);

						vIndexSets.clear();
						textureIndexSets.clear();
						normalIndexSets.clear();
					}
				}



			}


		}

		if (vnIndexSets.empty())	// 如果Obj文件中没有法向量信息就自己计算
			CountNormal();

	}

	f.close();

	textureVec.assign(textureContainer.begin(), textureContainer.end());
	NumVertices = fSets.size() * 3;
	points = new point4[NumVertices];
	textures = new vec2[NumVertices];
	normals = new vec3[NumVertices];
}

void ObjLoader::Draw(mat4 view)
{
	glBindVertexArray(vao);
	for (int i = 0; i < textureVec.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textureAddress[i]);

	}
	model = Translate(adjust_pos) * RotateX(Theta[Xaxis]) * RotateY(Theta[Yaxis]) * RotateZ(Theta[Zaxis]);
	mat4  model_view = view * model;
	GLuint ModelView = glGetUniformLocation(program, "ModelView");
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	
}

GLuint ObjLoader::SetProgram()
{
	glUseProgram(program);
	return program;
}


vector<vector<GLfloat>> ObjLoader::getvSets()
{
	return vSets;
}

vector<vector<GLint>> ObjLoader::getfSets()
{
	return fSets;
}

vector<vector<GLfloat>> ObjLoader::getvtSets()
{
	return vtSets;
}

vector<vector<GLint>> ObjLoader::getvtIndexSets()
{
	return vtIndexSets;
}

vector<vector<GLfloat>> ObjLoader::getvnSets()
{
	return vnSets;
}

vector<vector<GLint>> ObjLoader::getvnIndexSets()
{
	return vnIndexSets;
}

vector<string> ObjLoader::Tri(vector<string> paramaters)
{
	vector<string> result;
	result.push_back("f");
	string firstElement = paramaters[0];
	int size = paramaters.size() - 1;
	if (size < 3)
		cout << "the fSetsets_Size is not correct" << endl;
	for (int end = 3; end <= size; end++)
	{
		result.push_back(paramaters[1]);
		result.push_back(paramaters[end - 1]);
		result.push_back(paramaters[end]);
	}

	return result;
}

void ObjLoader::CountNormal()
{
	for (int i = 0; i < fSets.size(); i++)
	{
		vector<GLfloat> normalSets;
		vector<GLint>normalIndexSets;

		GLint firstVertexIndex = (fSets[i])[0];
		GLint secondVertexIndex = (fSets[i])[1];
		GLint thirdVertexIndex = (fSets[i])[2];

		vec4 a = vec4(vSets[firstVertexIndex][0], vSets[firstVertexIndex][1], vSets[firstVertexIndex][2], 1.0);
		vec4 b = vec4(vSets[secondVertexIndex][0], vSets[secondVertexIndex][1], vSets[secondVertexIndex][2], 1.0);
		vec4 c = vec4(vSets[thirdVertexIndex][0], vSets[thirdVertexIndex][1], vSets[thirdVertexIndex][2], 1.0);

		vec4 u = a - b;
		vec4 v = b - c;
		vec4 normal = normalize(cross(u, v));

		normalSets.push_back(normal[0]);
		normalSets.push_back(normal[1]);
		normalSets.push_back(normal[2]);
		vnSets.push_back(normalSets);

		normalIndexSets.push_back(i);
		normalIndexSets.push_back(i);
		normalIndexSets.push_back(i);
		vnIndexSets.push_back(normalIndexSets);
	}
}

void ObjLoader::ColorObject()
{
	for (int i = 0; i < fSets.size(); i++)
	{
		GLint firstVertexIndex = (fSets[i])[0];
		GLint secondVertexIndex = (fSets[i])[1];
		GLint thirdVertexIndex = (fSets[i])[2];

		GLint firstTextureIndex = (vtIndexSets[i])[0];
		GLint secondTextureIndex = (vtIndexSets[i])[1];
		GLint thirdTextureIndex = (vtIndexSets)[i][2];

		GLint firstNormalIndex = (vnIndexSets[i])[0];
		GLint secondNormalIndex = (vnIndexSets[i])[1];
		GLint thirdNormalIndex = (vnIndexSets[i])[2];

		points[Index] = point4(vSets[firstVertexIndex][0], vSets[firstVertexIndex][1], vSets[firstVertexIndex][2], 1.0);
		textures[Index] = vec2(vtSets[firstTextureIndex][0], 1 - vtSets[firstTextureIndex][1]);
		normals[Index] = vec3(vnSets[firstNormalIndex][0], vnSets[firstNormalIndex][1], vnSets[firstNormalIndex][2]);
		Index++;

		points[Index] = point4(vSets[secondVertexIndex][0], vSets[secondVertexIndex][1], vSets[secondVertexIndex][2], 1.0);
		textures[Index] = vec2(vtSets[secondTextureIndex][0], 1 - vtSets[secondTextureIndex][1]);
		normals[Index] = vec3(vnSets[secondNormalIndex][0], vnSets[secondNormalIndex][1], vnSets[secondNormalIndex][2]);
		Index++;

		points[Index] = point4(vSets[thirdVertexIndex][0], vSets[thirdVertexIndex][1], vSets[thirdVertexIndex][2], 1.0);
		textures[Index] = vec2(vtSets[thirdTextureIndex][0], 1 - vtSets[thirdTextureIndex][1]);
		normals[Index] = vec3(vnSets[thirdNormalIndex][0], vnSets[thirdNormalIndex][1], vnSets[thirdNormalIndex][2]);
		Index++;
	}
}

void ObjLoader::init()
{
	ColorObject();

	for (int i = 0; i < textureVec.size(); i++)
	{
		GLuint texture;
		string texturename = textureVec[i];
		glGenTextures(1, &texture);
		textureAddress.push_back(texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		string param = "texture";
		param.append(to_string(texture));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int picture_width, picture_height;
		unsigned char* image = SOIL_load_image(texturename.c_str(), &picture_width, &picture_height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, picture_width, picture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glUseProgram(program);
		glUniform1i(glGetUniformLocation(program, param.c_str()), 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
		12 * sizeof(GLfloat) * fSets.size()
		+ 6 * sizeof(GLfloat) * vtIndexSets.size()
		+ 9 * sizeof(GLfloat) * vnIndexSets.size(),
		NULL, GL_STATIC_DRAW);

	GLintptr offset = 0;

	glBufferSubData(GL_ARRAY_BUFFER, offset, 12 * sizeof(GLfloat) * fSets.size(), points);
	offset += 12 * sizeof(GLfloat) * fSets.size();

	glBufferSubData(GL_ARRAY_BUFFER, offset, 6 * sizeof(GLfloat) * vtIndexSets.size(), textures);
	offset += 6 * sizeof(GLfloat) * vtIndexSets.size();


	glBufferSubData(GL_ARRAY_BUFFER, offset, 9 * sizeof(GLfloat) * vnIndexSets.size(), normals);


	glUseProgram(program);


	// set up vertex arrays
	offset = 0;
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += 12 * sizeof(GLfloat) * fSets.size();

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += 6 * sizeof(GLfloat) * vtIndexSets.size();

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));

	// Initialize shader lighting parameters
	color4 light_ambient(0.1, 0.1, 0.1, 1.0);	// 环境光反射系数
	color4 light_diffuse(0.5, 0.5, 0.5, 1.0);	// 漫反射反射系数
	color4 light_specular(0.5, 0.5, 0.5, 1.0);	// 镜面反射系数

	color4 material_ambient(0.5, 0.5, 0.5, 1.0);
	color4 material_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 material_specular(0.8, 0.8, 0.8, 1.0);
	float  material_shininess = 50.0;	// 发光值


	
}

void ObjLoader::display()
{

}

void ObjLoader::move(GLfloat x, GLfloat y, GLfloat z)
{
	adjust_pos = vec3(x, y, z);
}
