#include <glm/glm.hpp>
#include <vector>

using std::vector;
#pragma once
namespace mine {
	struct vertex {
		vertex(vec3 pos, vec3 color, vec3 norm, vec2 coord):pos(pos),color(color),norm(norm),coord(coord) {}
		vertex() {}
		vec3 color;
		vec3 norm;
		vec3 pos;
		vec2 coord;
	};
	const float cubedata[] = {
		//Front Face
		-1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f ,1.0f,  1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
		//Back Face
		-1.0f, -1.0f, -1.0f,  -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,  -1.0f, 1.0f, -1.0f,  1.0f, 1.0f, -1.0f,
		//Left Face
		-1.0f, -1.0f, -1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,
		//Right Face
		1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,  1.0f, 1.0f, -1.0f,  1.0f, 1.0f, 1.0f,
		//Up Face
		-1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, -1.0f,
		//Down Face
		1.0f, -1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f
	};

	vector<vertex> &cube(float a, vector<vertex> &v)
	{
		vertex temp;
		int len = sizeof(cubedata) / sizeof(float);
		//load position data
		for (int i = 0; i < len / 3; ++i) {
			temp.pos.x = cubedata[3 * i] / 2 * a;
			temp.pos.y = cubedata[3 * i + 1] / 2 * a;
			temp.pos.z = cubedata[3 * i + 2] / 2 * a;
			v.push_back(temp);
		}
		//calculate norm
		for (int i = 0; i < v.size() / 3; i++) {
			vec3 first = v[3 * i + 1].pos - v[3 * i].pos;
			vec3 second = v[3 * i + 2].pos - v[3 * i + 1].pos;
			vec3 norm = normalize(cross(first, second));
			v[3 * i].norm = v[3 * i + 1].norm = v[3 * i + 2].norm = norm;
			v[3 * i].color = v[3 * i + 1].color = v[3 * i + 2].color = (norm + vec3(2.f) / 3.f);
		}
		return v;
	}

	vector <vertex> &square(float a, vector<vertex> &v)
	{
		vertex temp;
		int len = 18;
		//load position data
		for (int i = 0; i < len / 3; ++i) {
			temp.pos.x = cubedata[3 * i] / 2 * a;
			temp.pos.y = cubedata[3 * i + 1] / 2 * a;
			temp.pos.z = 0.f;
			v.push_back(temp);
		}
		//calculate norm
		for (int i = 0; i < v.size() / 3; i++) {
			vec3 first = v[3 * i + 1].pos - v[3 * i].pos;
			vec3 second = v[3 * i + 2].pos - v[3 * i + 1].pos;
			vec3 norm = normalize(cross(first, second));
			v[3 * i].norm = v[3 * i + 1].norm = v[3 * i + 2].norm = norm;
			v[3 * i].color = v[3 * i + 1].color = v[3 * i + 2].color = (norm + vec3(2.f) / 3.f);
		}
		return v;
	}

	class model_obj {
		vector<vertex> vertexarray;
		vector<unsigned int> elementarray;
	};
}