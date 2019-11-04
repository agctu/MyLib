#ifndef GL_DRAW
#define GL_DRAW
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ShaderClass.h>
#include <geometry.hpp>
#include <vector>

using mine::point;
using namespace glm;
using namespace std;
namespace mine {
	vector<unsigned int> vaos, vbos;
	class drawrect {

		static float square[12];
		unsigned int vao, vbo;
		//unsigned int textureid;
		Shader shd;
		drawrect() {}
		void setup()
		{
			shd = Shader("d:\\code\\mylib\\shaders\\plat.vsd", "d:\\code\\mylib\\shaders\\plat.fsd");
			
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(0);
			glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
			glBindVertexArray(0);
			vaos.push_back(vao);
			vbos.push_back(vbo);
		}
	public:
		void _draw(point pos, float width, float height, unsigned int textureid)
		{
			mat4 model;
			model = translate(model, vec3(pos.x, pos.y, 0.f));
			model = scale(model, vec3(width / 2, height / 2, 1));
			mat4 project = ortho(0.f, 800.f, 0.f, 800.f);
			mat4 view = mat4();
			shd.use();
			glBindVertexArray(vao);
			shd.setMat4("model", model);
			shd.setMat4("project", project);
			shd.setMat4("view", view);
			shd.setBool("hastex", true);
			glActiveTexture(0);
			glBindTexture(GL_TEXTURE_2D, textureid);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDrawArrays(GL_TRIANGLES, 0, sizeof(square));
			glDisable(GL_BLEND);
			glBindVertexArray(0);
		}
		void _draw(point pos, float width, float height, vec4 c = vec4(0.f))
		{
			mat4 model;
			model = translate(model, vec3(pos.x, pos.y, 0.f));
			model = scale(model, vec3(width / 2, height / 2, 1));
			mat4 project = ortho(0.f, 800.f, 0.f, 800.f);
			mat4 view = mat4();
			shd.use();
			glBindVertexArray(vao);
			shd.setMat4("model", model);
			shd.setMat4("project", project);
			shd.setMat4("view", view);
			shd.setVec4("color", c);
			shd.setBool("hastex", false);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDrawArrays(GL_TRIANGLES, 0, sizeof(square));
			glDisable(GL_BLEND);
			glBindVertexArray(0);
		}
		static bool inited;
		static drawrect d;
		static void draw(point pos, float width, float height, unsigned int textureid)
		{
			d._draw(pos, width, height, textureid);
		}
		static void draw(point pos, float width, float height, vec4 c)
		{
			d._draw(pos, width, height, c);
		}
		static void init()
		{
			if (!inited) {
				d.setup();
				inited = true;
				
			}
			else
				cout << "already inited" << endl;
		}
		
	};
	float drawrect::square[12] = { -1.f,-1.f, 1.f,-1.f, -1.f,1.f, 1.f,1.f, -1.f,1.f, 1.f, -1.f };
	drawrect drawrect::d;
	bool drawrect::inited = false;
	class drawcircle {
	public:
		static unsigned int vao, vbo;
		static bool inited;
		static Shader shd;
		static int edges;
		static void draw(int x, int y, float r,vec4 color=vec4(vec3(),1.f))
		{
			glBindVertexArray(vao);
			mat4 model, view, project;
			project = ortho(0.f, 800.f, 0.f, 800.f);
			view = mat4();
			model = translate(model, vec3(x, y, 0.f));
			model = scale(model, vec3(r, r, 0));
			shd.use();
			shd.setMat4("project", project);
			shd.setMat4("view", view);
			shd.setMat4("model", model);
			shd.setBool("hastex", false);
			shd.setVec4("color", color);
			glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(float)*(edges+1));
			glBindVertexArray(0);
		}
		static void init(int edges = 32)
		{
			if (inited) {
				cout << "already inited" << endl;
			}
			else {
				setup(edges);
				inited = true;
				vaos.push_back(vao);
				vbos.push_back(vbo);
			}
		}
		static void setup(int edges)
		{
			
			shd=Shader("d:\\code\\mylib\\shaders\\plat.vsd", "d:\\code\\mylib\\shaders\\plat.fsd");
			drawcircle::edges = edges;
			vector<point> points;
			points.push_back(point(0.f));
			for (int i = 0; i < edges; i++) {
				points.push_back(point(cos(2*pi<float>() * i / edges), sin(2*pi<float>()*i / edges)));
				points.push_back(point(cos(2*pi<float>()* (i + 1) / edges), sin(2*pi<float>()* (i+1)/ edges)));
				cout << "degree: "<<360.f*i/edges<<" radiance: " << 2 * pi<float>() * i / edges << endl;
				cout << "x: " << cos(2 * pi<float>() * (i+1) / edges) << "y: " << sin(2 * pi<float>()*(i+1) / edges) << endl;
			}
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(0);
			glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float)*points.size(), &points[0], GL_STATIC_DRAW);
			glBindVertexArray(0);
			
		}
	};
	bool drawcircle::inited = false;
	Shader drawcircle::shd;
	int drawcircle::edges;
	unsigned int drawcircle::vao, drawcircle::vbo;
	class drawlines {
		const static int maxlinenum;
		static unsigned int vao, vbo;
		static bool inited;
		static Shader shd;
	public:
		static void init()
		{
			if (!inited) {
				setup();
				inited = true;
				vaos.push_back(vao);
				vbos.push_back(vbo);
			}
			else
				cout << "already inited" << endl;
		}
		static void setup()
		{
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glVertexAttribPointer(0, sizeof(float) * 2, GL_FLOAT, GL_FALSE, sizeof(float)*(3 + 2), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, sizeof(float) * 3, GL_FLOAT, GL_FALSE, sizeof(float)*(3+2),(void*)(sizeof(float)*2));
			glEnableVertexAttribArray(1);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*maxlinenum * 4, NULL, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

		}
		static void draw(vector<float>data)
		{
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*data.size(), &data[0]);
			shd.use();
			shd.setMat4("projection", ortho(0, 800, 0, 800));
			glDrawArrays(GL_LINE, 0, data.size());
			glBindVertexArray(0);
		}
	};
	bool drawlines::inited = false;
	const int drawlines::maxlinenum = 100;
	unsigned int drawlines::vao, vbo;
	Shader drawlines::shd;
	int deleteall()
	{
		glDeleteVertexArrays(vaos.size(), &vaos[0]);
		glDeleteVertexArrays(vbos.size(), &vbos[0]);
		return vaos.size() + vbos.size();
	}
}
#endif