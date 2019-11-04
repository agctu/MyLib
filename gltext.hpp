#ifndef GL_TEXT
#define GL_TEXT
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <map>
#include <iostream>
#include <glad\glad.h>
#include <log.hpp>
#include <gbk_convert.h>
#include <modImage.hpp>
#include <gldraw.hpp>
#include <io.h>
#include <string>

using std::wcout;
using std::string;
using std::map;
using std::cout;
using std::endl;
class text {
public:
	static FT_Library ft;
	static FT_Face fc;
	static bool inited;
	static Shader shd;
	static int width, height;
	static mat4 projection;
	static unsigned int vao, vbo;
	struct chartex {
		unsigned int texid;
		int width;
		int height;
		int bearingx;//字体左上角距离原点的位置
		int bearingy;//字体右上角距离原点的位置
		unsigned int advancex;//下一个字体原点距离该字体的位置
		unsigned int advancey;
		chartex() {}
		chartex(unsigned int id, int width, int height, int bearingx, int bearingy, unsigned int advancex,unsigned int advancey) :texid(id), width(width), height(height), advancex(advancex),advancey(advancey), bearingx(bearingx), bearingy(bearingy) {}
	};
	static map<wchar_t, chartex> chartexmap;
	static void setup(int w,int h)
	{
		string fontname = "msyh.ttc";
		//fontname = "arial.ttf";
		//fontname = "Dengl.ttf";
		//init freetype library and generate a face and save the reference
		if (FT_Init_FreeType(&ft)) {
			cout << "freetype init failed" << endl;
			FT_Done_FreeType(ft);
		}
		if (_access(("fonts/"+fontname).c_str(), 0) != 0) {//文件不存在
			if (_access("fonts", 0) != 0) {//文件夹不存在
				system("md fonts");
			}
			system(("copy c:\\windows\\fonts\\"+fontname+" fonts\\").c_str());
		}
		if (FT_New_Face(ft, ("fonts/"+fontname).c_str(), 0, &fc)) {
			cout << "face create failed" << endl;
			FT_Done_FreeType(ft);
			FT_Done_Face(fc);
		}
		FT_Set_Char_Size(fc, w<<6, h << 6, 1920, 1080);
		FT_Set_Pixel_Sizes(fc, w,h);//设置生成的位图大小,指定为0,则在运行时动态生成
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//改变默认字节对齐值4 为 1
		for (wchar_t c = 0; c < 128; c++) {//默认初始化时只生成ascii字符(控制字符可能无法生成
			loadchar(c);
		}
		//shader used to render text
		shd = Shader("d:\\code\\mylib\\shaders\\text.vsd", "d:\\code\\mylib\\shaders\\text.fsd");

		projection = ortho(0.f, 800.f, 0.f, 800.f);
		
		

		//generate vao and vbo
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));//第二个参数是分量的个数
		glEnableVertexAttribArray(1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, NULL, GL_STATIC_DRAW);
		
		
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);
	}	
	static void loadchar(wchar_t c)
	{
		if (chartexmap.find(c) != chartexmap.end()) {
			return;
		}
		if (FT_Load_Char(fc, c, FT_LOAD_RENDER|FT_LOAD_COLOR|FT_LOAD_FORCE_AUTOHINT)) {
			cout << "char: " << c << " load failed" << endl;
			return;
		}
		unsigned int texid;


		FT_Bitmap bm = fc->glyph->bitmap;
		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_2D, texid);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bm.width, bm.rows, 0, GL_RED, GL_UNSIGNED_BYTE, bm.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		chartexmap.insert(make_pair(c, chartex(texid, bm.width, bm.rows, fc->glyph->bitmap_left, 
			fc->glyph->bitmap_top, fc->glyph->advance.x, fc->glyph->advance.y)));
		/*Log << "char: " << c << " width: " << bm.width << " height: " << bm.rows << " left: " 
			<< fc->glyph->bitmap_left << " top: " << fc->glyph->bitmap_top << endl;*/
	}
	static void init(int w = 0,int h = 100)//因为懒才这样写的
	{
		if (!inited) {
			width = w;
			height = h;
			setup(w,h);
			inited = true;
		}
		else {
			cout << "already inited" << endl;
		}
	}
	static void drawchar(int x, int y, wchar_t c,float size,vec4 color=vec4(vec3(),1.f),bool check=true)
	{
		//cout << "x: " << x << " y: " << y<<endl;
		
		if (check)
			loadchar(c);
		
		//caculate the metrices
		chartex tch = chartexmap[c];
		int w = tch.width * size/height, h = tch.height * size/height;
		int px = x + tch.bearingx * size/height, py =y -(tch.height - tch.bearingy) * size/height;
		//cout << "width: " << w << " height: " << h << " px: " << px << " py: " << py << endl;
		float data[] = {	
				px,		py,		0.f,	1.f,
			px + w,		py,		1.f,	1.f,
			px + w,	py + h,		1.f,	0.f,
				px,		py,		0.f,	1.f,
			px + w,	py + h,		1.f,	0.f,
				px,	py + h,		0.f,	0.f
		};
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		shd.use();
		shd.setMat4("projection", projection);
		shd.setVec4("color", color);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, tch.texid);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glDisable(GL_BLEND);
	}
	static void drawline(int x, int y, wstring s,float size, vec4 color = vec4(0.f, 0.f, 0.f, 1.f),unsigned int maxsize=-1)
	{
		int cx = x;
		/*wcout.imbue(locale(""));
		wcout <<s << endl;*/
		for (int i = 0; i < s.size(); i++) {
			
			
			drawchar(cx, y, s[i], size,color);
			cx += size*(chartexmap[s[i]].advancex>>6)/height;//单位是1/64像素
			//截断
			if (cx - x +size * (chartexmap[s[i]].advancex>>6)/height> maxsize) {
				break;
			}
		}
	}
	static void drawline(int x, int y, string s, float size,vec4 color=vec4(0.f,0.f,0.f,1.f),unsigned int maxsize=-1)
	{
		wstring ws = mine::dc_gbk(s);
		drawline(x, y, ws, size,color,maxsize);
	}
	static void drawcolomn(int x, int y, wstring s, float size, vec4 color = vec4(0.f, 0.f, 0.f, 1.f))
	{
		
		for (int i = 0; i < s.size(); i++) {
			drawchar(x, y, s[i], size,color);
			y -= size * 1.1*(fc->size->metrics.height)/height;
			
		}
	}
	static void drawcolomn(int x, int y, string s, float size, vec4 color = vec4(0.f, 0.f, 0.f, 1.f))
	{
		wstring ws = mine::dc_gbk(s);
		drawcolomn(x, y, ws, size,color);
	}
	static void terminate()
	{
		for (auto i : chartexmap) {
			unsigned int id = i.second.texid;
			glDeleteTextures(1, &id);
		}
		FT_Done_Face(fc);
		FT_Done_FreeType(ft);
	}
}; 
FT_Library text::ft;
FT_Face text::fc;
bool text::inited=false;
map<wchar_t, text::chartex> text::chartexmap;
Shader text::shd;
mat4 text::projection;
unsigned int text::vao, text::vbo;
int text::width, text::height;
#endif