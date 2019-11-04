//这个类还没到能用的时候
#include "glad\glad.h"
#include <string>
#include <fstream>
#include "modImage.hpp"
#include <map>
#include <stb\stb_image.h>
using std::ifstream;
using std::string;
//using std::map;
class texture2d {
	unsigned int id;
	static map<unsigned int, int> id_c_map;
public:
	texture2d(string path)
	{
		/*mine::bitmap bm(path);
		vector<mine::color>cv(bm.width);*/
		/*fill(cv.begin(), cv.end(),mine::color::purple);
		for (int i = 0; i < bm.height / 2; i++)
			bm.setline(cv, i);*/
		int x, y, nrchannel;
		stbi_set_flip_vertically_on_load(true);
		unsigned char * data = stbi_load(path.c_str(), &x, &y, &nrchannel, 0);
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bm.width, bm.height, 0, GL_BGR, GL_UNSIGNED_BYTE, bm.data);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x,y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		id_c_map[id] += 1;
	}
	texture2d(const texture2d& c)
	{
		id = c.id;
		id_c_map[c.id] += 1;
	}
	unsigned int getid()
	{
		return id;
	}
	texture2d& operator=(const texture2d& c)
	{
		id_c_map[c.id] += 1;
		id_c_map[c.id] -= 1;
		if (id_c_map[id] == 0)
			glDeleteTextures(1, &id);
		//~texture2d();用不成?
		id = c.id;
	}
	void bind(unsigned int slot)
	{
		glActiveTexture(slot);
		glBindTexture(GL_TEXTURE_2D, id);
	}
	~texture2d()
	{
		id_c_map[id] -= 1;
		if (id_c_map[id] == 0) {
			glDeleteTextures(1, &id);
			id_c_map.erase(id);
		}
	}
private:
	void set_texture()
	{//TODOORNOTTODO?

	}
};
map<unsigned int,int> texture2d::id_c_map;