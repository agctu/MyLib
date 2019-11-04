#include <WinUser.h>
#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <ctime>
#include <iostream>
using std::vector;
using std::string;
using std::map;
using std::cout;
using std::endl;
using std::wstring;
using std::time;
using std::ostream;

namespace mine {
	class pinput;
	class dowhile;
	class inputchain;
	class pos {
	public:
		int x, y;
		pos(int x, int y) :x(x), y(y) {}
		pos():x(0),y(0) {};
	};
	
	class input_base {
	public:
		const static int scwidth;
		const static int scheight;
		static map<string, pos> positions;
		static pos last;
		static string error;
		static void addalias(string alias, pos p)
		{
			if (positions.find(alias) != positions.end())
				return;
			positions[alias] = p;
		}

		float time;
		vector<INPUT>inputs;
		input_base() :inputs(vector<INPUT>(2))
		{
			INPUT empty = { 0 };
			fill(inputs.begin(), inputs.end(), empty);
		}
		operator bool()
		{
			return !inputs.empty();
		}
		virtual void commit()=0;
		virtual input_base* clone()const = 0;
		
	};
	const int input_base::scwidth = GetSystemMetrics(SM_CXSCREEN);
	const int input_base::scheight = GetSystemMetrics(SM_CYSCREEN);
	class pinput {
		input_base *p;
		friend inputchain;
		friend dowhile;
		
		
	public:pinput(input_base *pib):p(pib){}
		~pinput()
		{
			delete p;
		}
		pinput(const pinput& a):p(a.p->clone()){}
	
		operator bool()
		{
			return p->operator bool();
		}
		input_base *operator->()
		{
			return p;
		}
		pinput& operator=(const pinput& a)
		{
			if (this == &a) {
				return *this;
			}
			p = a.p->clone();
		}
	};
	
	class mi_base:public input_base{
	public:
		pos p;
		mi_base():input_base() {
			inputs[0].type = inputs[1].type = INPUT_MOUSE;
			inputs[0].mi.time = inputs[1].mi.time = 0;
		}
	};
	class ki_base :public input_base{
	public:
		
		ki_base() :input_base()
		{
			for (int i = 0; i < inputs.size(); i++) {
				inputs[i].type = INPUT_KEYBOARD;
				inputs[i].ki.time = 0;
			}
		}
	};
	class twi_base :public input_base {
	public:
		vector<INPUT> first, last;
		twi_base() :input_base() {}
		void sendfirst()
		{
			for (auto i : first) {
				INPUT in = i;
				SendInput(1, &in, sizeof(INPUT));
			}
					}
		void sendlast()
		{
			for (auto i : last) {
				INPUT in = i;
				SendInput(1, &in, sizeof(INPUT));
			}
					}

	};
	class wait :public input_base {
	public:
		wait(float second)
		{
			time = second;
		}
		wait *clone()const
		{
			return new wait(*this);
		}
		void commit()
		{
			Sleep(time * 1000);
		}
	};
	class stroke :public ki_base{
	public:
		stroke(unsigned char vkc):ki_base()
		{
			inputs[0].ki.wVk = vkc;
			inputs[1].ki.wVk = vkc;
			inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
		}
		stroke *clone()const
		{
			return new stroke(*this);
		}
		void commit()
		{
			SendInput(1, &inputs[0], sizeof(INPUT));
			SendInput(1, &inputs[1], sizeof(INPUT));
		}
	};

	class instr :public ki_base {
	public:
		string s;
		instr(string s) :ki_base()
		{
			this->s = s;
			inputs[0].ki.dwFlags = KEYEVENTF_UNICODE;

		}
		instr *clone()const
		{
			return new instr(*this);
		}
		void commit()
		{
			for (int i = 0; i < s.length(); i++) {
				unsigned char c = s[i];
				inputs[0].ki.wScan = c;
				SendInput(1, &inputs[0], sizeof(INPUT));
				wait(0.01).commit();
			}
		}
	};
	class inwstr :public ki_base {
	public:
		wstring s;
		inwstr(wstring s) :ki_base()
		{
			this->s = s;
			inputs[0].ki.dwFlags = KEYEVENTF_UNICODE;
		}
		inwstr *clone()const
		{
			return new inwstr(*this);
		}
		void commit()
		{

			for (int i = 0; i < s.length(); i++) {
				wchar_t c = s[i];
				inputs[0].ki.wScan = c;
				SendInput(1, &inputs[0], sizeof(INPUT));
			}
		}
	};
	
	class waittil :public input_base {
		float second;
		int h, m, s;
	public:
		waittil(int hour, int minute, int second):h(hour),m(minute),s(second)
		{
			
			
		}
		waittil *clone()const
		{
			return new waittil(*this);
		}
		void commit()
		{
			time_t tstamp = ::time(0);
			tm *t = localtime(&tstamp);
			this->second = ((h - t->tm_hour) * 60 + (m - t->tm_min)) * 60 + (s - t->tm_sec);
			if (this->second < 0)
				second += 24 * 60 * 60;
			//cout << second-1 << endl;
			wait(second - 1).commit();
			
			while (true) {

				tstamp = ::time(0);
				t = localtime(&tstamp);
				second = ((h - t->tm_hour) * 60 + (m - t->tm_min)) * 60 + (s - t->tm_sec);
				if (second > 10)
					wait(second - 1).commit();
				else if (second <= 0)
					break;
			}
		}
	};
	class click : public mi_base {

	public:
		enum button{left,right,middle};
		click(button b=left):mi_base()
		{
			switch (b) {
			case left:

				inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
				break;
			case right:
				inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
				inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
				break;
			case middle:
				inputs[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
				inputs[1].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			}
		}
		click *clone()const
		{
			return new click(*this);
		}
		void commit()
		{
			SendInput(1, &inputs[0], sizeof(INPUT));
			SendInput(1,&inputs[1],sizeof(INPUT));
		}
	};
	class rclick :public mi_base {
	public:
		rclick():mi_base()
		{
			inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		}
		rclick *clone()const
		{
			return new rclick(*this);
		}
		void commit()
		{
			SendInput(1, &inputs[0], sizeof(INPUT));
			wait(0.2).commit();
			SendInput(1, &inputs[1], sizeof(INPUT));
		}
	};
	class dclick :public click {
	public :
		dclick(button b) :click(b) {}
		dclick *clone()const
		{
			return new dclick(*this);
		}
		void commit()
		{
			click::commit();
			click::commit();
		}
	};
	class drclick :public rclick {
	public:
		drclick *clone()const
		{
			return new drclick(*this);
		}
		void commit()
		{
			rclick::commit();
			rclick::commit();
		}
	};
	class moveto :public mi_base {
		
	public:
		
		moveto(int x, int y) :mi_base()
		{
			p = pos(x, y);
			setinput();
		}
		moveto(pos p) :mi_base()
		{
			this->p = p;
			setinput();
		}
		moveto(string alias):mi_base()
		{
			if (positions.find(alias) == positions.end()) {
				error = "alias not found";
				return;
			}
			p = positions[alias];
			setinput();
		}
		moveto *clone()const
		{
			return new moveto(*this);
		}
		void commit()
		{
			SendInput(1, &inputs[0], sizeof(INPUT));
		}
	private:
		void setinput()
		{
			inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
			inputs[0].mi.dx = 0xffff * p.x / scwidth;
			inputs[0].mi.dy = 0xffff * p.y / scheight;
		}
	};
	class moveby :public mi_base {
	public:
		moveby(int dx, int dy) :mi_base()
		{
			p = pos(dx, dy);

			setinput();
		}
		moveby *clone()const
		{
			return new moveby(*this);
		}
		void commit()
		{
			SendInput(1, &inputs[0], sizeof(INPUT));
		}
	private:
		void setinput()
		{
			inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE;
			inputs[0].mi.dx = p.x*scwidth / 1920;
			inputs[0].mi.dy = p.y*scheight / 1080;
		}
	};
	class dowhile :public twi_base {
		pinput act, lasted;
	public:
		dowhile(const input_base& act, const input_base& lasted) :act(act.clone()), lasted(lasted.clone()),twi_base()
		{
			if (typeid(lasted).name() == typeid(*this).name()) {
				twi_base &realt = dynamic_cast<twi_base &>(const_cast<input_base&>(lasted));
				first = realt.first;
				last = realt.last;
				
			}
			else {
				first.push_back(lasted.inputs[0]);
				last.push_back(lasted.inputs[1]);
			}
				
			
			insert(act);
		}
		dowhile(const pinput& act, const pinput& lasted) :act(act), lasted(lasted) ,twi_base()
		{
			if (typeid(*lasted.p).name() == typeid(*this).name()) {
				twi_base &realt = dynamic_cast<twi_base &>(const_cast<input_base&>(*lasted.p));
				first = realt.first;
				last = realt.last;
				
			}
			else {
				first.push_back(lasted.p->inputs[0]);
				last.push_back(lasted.p->inputs[1]);
			}
			
			insert(*act.p);
		}
		dowhile *clone()const
		{
			return new dowhile(*this);
		}
		void commit()
		{
			sendfirst();
			wait(0.2).commit();
			sendlast();
			/*SendInput(1, &(lasted->inputs[0]), sizeof(INPUT));
			wait(0.1).commit();
			act->commit();
			wait(0.1).commit();
			SendInput(1, &(lasted->inputs[1]), sizeof(INPUT));*/
		}
	private:
		void insert(const input_base& in)
		{
			first.push_back(in.inputs[0]);
			last.insert(last.begin(), in.inputs[1]);
		}
	};
	class dragto :public mi_base {
		click::button b;
	public:
		
		dragto(int x,int y,click::button b=click::left):mi_base(),b(b)
		{
			p = pos(x, y);
		}
		dragto *clone()const
		{
			return new dragto(*this);
		}
		void commit()
		{
			dowhile(moveto(p), click(b)).commit();
			/*SendInput(1, &inputs[0], sizeof(INPUT));
			wait(0.2).commit();
			moveto(p).commit();
			wait(0.2).commit();
			SendInput(1, &inputs[1], sizeof(INPUT));*/

		}
	
	};
	class rdragto :public mi_base {
	public:

		rdragto(int x, int y) :mi_base()
		{
			p = pos(x, y);
			inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		}
		rdragto *clone()const
		{
			return new rdragto(*this);
		}
		void commit()
		{
			SendInput(1, &inputs[0], sizeof(INPUT));
			wait(0.2).commit();
			moveto(p).commit();
			wait(0.2).commit();
			SendInput(1, &inputs[1], sizeof(INPUT));

		}
	
	};
	class dragby :public mi_base {
	public:
		click::button b;
		dragby(int dx, int dy, click::button b) :mi_base(), b(b)
		{
			p = pos(dx, dy);
		}
		dragby *clone()const
		{
			return new dragby(*this);
		}
		void commit()
		{
			dowhile(moveby(p.x,p.y), click(b)).commit();
		}
	};
	class loop :public input_base {
		int time;
		pinput pi;
	public:
		loop(const input_base *i, int time) :time(time), pi(i->clone()) {}
		loop *clone()const
		{
			return new loop(*this);
		}
		loop(const pinput pi, int time) :time(time), pi(pi) {}
		void commit()
		{
			for (int i = 0; i < time; i++) {
				pi->commit();
			}
		}
	};
	class errinput :public input_base {
	public:
		string what;
		ostream* file;
		errinput(string what="") :input_base(),what(what),file(&cout)
		{
			
			inputs.clear();
		}
		void setfile(ostream &f)
		{
			file = &f;
		}
		errinput *clone()const
		{
			return new errinput(*this);
		}
		void commit() 
		{
			*file << what << endl;
		}
	};
	class inputchain :public input_base{
	public:
		class action {
			pinput input;
			
		public:
			string name;
			action(const input_base& in, string name=" ") :input(in.clone()), name(name) {}
			action(const pinput& in, string name) :input(in), name(name) {}
			pinput operator->()
			{
				return input;
			}
		};
		vector<action> movements;
		float pendingtime;
	public:
		
		inputchain(float pendingsecond = 0.25) :input_base(),pendingtime(pendingsecond) {}
		inputchain& click(click::button b=click::left)
		{
			movements.push_back(action(mine::click(b)));
			return *this;
		}
		inputchain& rclick()
		{
			movements.push_back(action(mine::rclick()));
			return *this;
		}
		inputchain& dclick(click::button b=click::left)
		{
			movements.push_back(action(mine::dclick(b)));

			return *this;
		}
		inputchain& drclick()
		{
			movements.push_back(action(mine::drclick()));
			return *this;
		}
		
		inputchain& wait(int second)
		{
			movements.push_back(action(mine::wait(second)));
			return *this;
		}
		inputchain& moveto(int x, int y)
		{
			movements.push_back(action(mine::moveto(x, y)));
			return *this;
		}
		inputchain& dragto(int x, int y,click::button b=click::left)
		{
			movements.push_back(action(mine::dragto(x, y,b)));
			return *this;
		}
		inputchain& append(inputchain ic)
		{
			movements.push_back(action(mine::inputchain(ic)));
			return *this;
		}
		inputchain& stroke(unsigned char vkcode)
		{
			movements.push_back(action(mine::stroke(vkcode)));
			return *this;
		}
		inputchain& instr(string s)
		{
			movements.push_back(action(mine::instr(s)));
			return *this;
		}
		inputchain& inwstr(wstring s)
		{
			movements.push_back(action(mine::inwstr(s)));
			return *this;
		}
		inputchain& dowhile(const input_base& act, const input_base& lasted)
		{
			movements.push_back(action(mine::dowhile(act, lasted)));
			return *this;
		}
		inputchain& push(const input_base& i, const string& name=" ")
		{
			movements.push_back(action(i, name));
			return *this;
		}
		inputchain& push(const pinput& pi, const string& name=" ")
		{
			movements.push_back(action(pi, name));
			return *this;
		}
		inputchain& clear()
		{
			movements.clear();
			return *this;
		}
		action pop()
		{
			if (movements.empty()) {
				//error = "empty inputchain can't be pop";只要加上这句就会出错无法解析的外部符号
				return errinput();
			}
			action t = movements.back();
			movements.pop_back();
			return t;
		}
		void commit()
		{
			for (action i : movements) {
				i->commit();
				mine::wait(pendingtime).commit();
				
			}
		}
		inputchain * clone()const
		{
			return new inputchain(*this);
		}
		int size()
		{
			return movements.size();
		}
		action& operator[](int index)
		{
			return movements[index];
		}
	};
	
}