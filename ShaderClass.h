#ifndef SHADER_H
#define SHADER_H
#include <glad\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <Windows.h>
using namespace std;
using namespace glm;

class Shader
{
public:

	unsigned int id;
	//for default construct
	Shader() {}
	Shader(const GLchar *vertexPath, const GLchar *fragmentPath,const GLchar *geometryPath = NULL)
	{
		stringstream sstrm;
		string temp;
		ifstream ifstrm;
		unsigned int vshader, fshader,gshader;
		int success = -1;
		const char *codep;
		char log[2048];
		
		ifstrm.open(vertexPath, ifstream::in);
		
		sstrm << ifstrm.rdbuf();
		if (ifstrm.fail()) {
			cout << "Shader Source Read Failed" << endl;
			Sleep(3000);
			exit(-1);
		}
		temp = sstrm.str();
		codep = temp.c_str();
		vshader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vshader, 1, &codep, NULL);
		glCompileShader(vshader);
		
		ifstrm.close();
		ifstrm.clear();
		sstrm.str("");

		ifstrm.open(fragmentPath, ifstream::in );
		sstrm << ifstrm.rdbuf();

		
		if (ifstrm.fail()){
			cout << "Shader Source Read Failed" << endl;
			Sleep(3000);
			exit(-1);
		}
		temp = sstrm.str();
		codep = temp.c_str();

		fshader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fshader, 1, &codep, NULL);
		glCompileShader(fshader);
		
		ifstrm.close();
		ifstrm.clear();
		sstrm.str("");

		if (geometryPath != NULL) {
			ifstrm.open(geometryPath, ifstream::in );
			
			sstrm << ifstrm.rdbuf();
			if (ifstrm.fail()) {
				cout << "Shader Source Read Failed" << endl;
				Sleep(3000);
				exit(-1);
			}

			temp = sstrm.str();
			codep = temp.c_str();
			gshader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(gshader, 1, &codep, NULL);
			glCompileShader(gshader);

			ifstrm.close();
			ifstrm.clear();
			sstrm.str("");
		}
		id = glCreateProgram();
		glAttachShader(id, vshader);
		glAttachShader(id, fshader);
		if (geometryPath != NULL)
			glAttachShader(id, gshader);
		glLinkProgram(id);

		glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
		cout << string(vertexPath) << "   :" << endl;
		cout << "VertexShader" << "Compile     :    ";
		if (success == 1)
			cout << "Success" << endl;
		else {
			glGetShaderInfoLog(vshader, sizeof(log), 0, log);
			cout << "Failed" << endl << "Error   :   " << log << endl;
		}
		glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
		cout << string(fragmentPath) << "   :" << endl;
		cout << "FragmentShader" << "Compile     :    ";
		if (success == 1)
			cout << "Success" << endl;
		else {
			glGetShaderInfoLog(fshader, sizeof(log), 0, log);
			cout << "Failed" << endl << "Error   :   " << log << endl;
		}
		if (geometryPath != NULL) {
			glGetShaderiv(gshader, GL_COMPILE_STATUS, &success);
			cout << string(geometryPath) << "   :" << endl;
			cout << "GeometryShader" << "Compile     :    ";
			if (success == 1)
				cout << "Success" << endl;
			else {
				glGetShaderInfoLog(gshader, sizeof(log), 0, log);
				cout << "Failed" << endl << "Error   :   " << log << endl;
			}
		}
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		cout << "ShaderProgram" << "Link      :    ";
		if (success == 1)
			cout << "Success" << endl;
		else {
			glGetProgramInfoLog(id, sizeof(log), 0, log);
			cout << "Failed" << endl << "Error   :   " << log << endl;
		}

		glDeleteShader(vshader);
		glDeleteShader(fshader);
		if (geometryPath != NULL)
			glDeleteShader(gshader);
	}
	unsigned int getID()
	{
		return id;
	}
	void use()
	{
		glUseProgram(id);
	}
	void setBool(const char *name, bool value)
	{
		glUniform1i(glGetUniformLocation(id, name), (int)value);
	}
	void setInt(const char *name, int value)
	{
		glUniform1i(glGetUniformLocation(id, name), (int)value);
	}
	void setFloat(const char *name, float value)
	{
		glUniform1f(glGetUniformLocation(id, name), value);
	}
	void setVec2(const char *name, vec3 value)
	{
		glUniform2f(glGetUniformLocation(id, name), value.x, value. y);
	}
	void setVec3(const char *name, vec3 value)
	{
		glUniform3f(glGetUniformLocation(id, name), value.x, value.y, value.z);
	}
	void setVec4(const char *name, vec4 value)
	{
		glUniform4f(glGetUniformLocation(id, name), value.x, value.y, value.z, value.w);
	}
	void setMat4(const char *name,mat4 value)
	{
		glUniformMatrix4fv(glGetUniformLocation(id,name), 1, GL_FALSE, value_ptr(value));
	}
	void setMat3(const char *name,mat3 value)
	{
		glUniformMatrix3fv(glGetUniformLocation(id,name), 1, GL_FALSE, value_ptr(value));
	}
	void setMat2(const char *name,mat2 value)
	{
		glUniformMatrix2fv(glGetUniformLocation(id,name), 1, GL_FALSE, value_ptr(value));
	}

};
#endif





