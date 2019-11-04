#ifndef GL_UI
#define GL_UI
#include <glad\glad.h>
#include <vector>
#include <gldraw.hpp>
#include <gltext.hpp>
#include <functional>
#include <handle.hpp>
#include <GLFW\glfw3.h>
#include <geometry.hpp>
namespace mine {
	using std::vector;

	enum evtype { evbutton, evmove, evkey };
	struct message {
		message(evtype tp, vec2 pos) :type(tp), pos(pos) {}
		message(evtype tp, int button, int action) :type(tp), key(button), action(action) {}
		message(const message & m) :type(m.type), pos(m.pos) {}
		evtype type;
		union {
			vec2 pos;
			struct {
				int key;
				int action;
			};
		};
	};
	class widget;
	
	typedef jptr<widget> hwidget;
	class widget {
		vector<hwidget> subwidgets;
		//hwidget root;
	public:

		int x, y;
		int width, height;
		rect wrect;
		widget(int x, int y, int width, int height) :x(x), y(y), width(width), height(height), wrect(x, y + height, x + width, y) {}
		widget() {}
		virtual void draw()
		{
			throw runtime_error("have nothing to draw");
		};
		vec2 getpos()
		{
			return vec2(x, y);
		}
		virtual operator hwidget ();//此函数及其子函数应该new一个实例出来而不是返回this,但是这里我并没有这样做
		virtual void dispathm(message m) {}
	};

	
	widget::operator hwidget ()
	{
		return this;
	}
	class textview :public widget {
		string s;
		unsigned int background = 0;
	public:
		int charw, charh;
		vec4 backcolor;
		vec4 charcolor;
		textview(int x, int y, int w, int h, int charw, int charh, vec4 backc = vec4(1.f, 1.f, 1.f, 1.f), vec4 charc = vec4(0.f, 0.f, 0.f, 1.f)) :widget(x, y, w, h), charw(charw), charh(charh), backcolor(backc), charcolor(charc) {}
		textview() {}
		void setpicture(unsigned int texid)
		{
			background = texid;
		}
		void setstring(const string& text)
		{
			s = text;
		}
		string getstring()
		{
			return s;
		}
		void draw()
		{
			glEnable(GL_SCISSOR_TEST);
			glScissor(x, y, width, height);
			if (background == 0)
				drawrect::draw(vec2(x + width / 2, y + height / 2), width, height, backcolor);
			else
				drawrect::draw(vec2(x, y), width, height, background);
			//cout << "textview::draw" << endl;

			text::drawline(x + 10, y + charh / 2, s, charh, charcolor);

			glDisable(GL_SCISSOR_TEST);
		}
		operator hwidget()
		{
			return this;
		}

	};

	class button :public widget {

		string s;
		typedef function<void(int,int)> ONCLICK;
	public:
		/*class onclicklistener {
		public:
			virtual void onclick()=0 ;
		};*/
		ONCLICK onclick = 0;
		button() {}
		button(int x, int y, int w, int h, string s = "") :widget(x, y, w, h), s(s) {}
		void draw()
		{
			glEnable(GL_SCISSOR_TEST);
			glScissor(x, y, width, height);

			drawrect::draw(vec2(x + width / 2, y + height / 2), width, height, vec4(vec3(0.4f), 1.f));
			text::drawline(x + 10, y + (height - 6) / 2, s, height - 6);

			glDisable(GL_SCISSOR_TEST);
		}
		void setstring(string str)
		{
			s = str;
		}
		void dispathm(message m)
		{
			if (m.type == evbutton)
				onclick(m.key,m.action);
		}
		void setonclick(ONCLICK ocl)
		{
			onclick = ocl;
		}
		operator hwidget()
		{
			return *this;
		}
	};
	class UImanager {
	public:
		point cursorpos;
		int scrwidth, scrheight;
		static UImanager UI;
		vector<hwidget> collection;
		hwidget active;
		hwidget spacewidget=new widget();
		UImanager() 
		{
			active = spacewidget;
		}
		void regist(hwidget hw)
		{
			collection.push_back(hw);
		}
		void seten(int width, int height)
		{
			scrwidth = width;
			scrheight = height;
		}
		void draw()
		{
			for (int i = 0; i < collection.size(); i++)
				collection[i]->draw();
		}
		void mouse_button(int button, int action)
		{
			//鼠标点击确定当前活动窗口
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				active = spacewidget;
				for (int i = 0; i < collection.size(); i++) {
					if (relaof(cursorpos, collection[i]->wrect) == inter) {
						active = collection[i];
						break;
					}
				}
			}
			active->dispathm(message(evbutton, button, action));
		}
		void mouse_move(int x, int y)
		{
			cursorpos = point(x, y);
			active->dispathm(message(evmove, vec2(x, y)));
		}
		void key(int key, int action)
		{
			active->dispathm(message(evkey, key, action));
		}
	};
	UImanager UImanager::UI;
}
#define UI mine::UImanager::UI
#endif