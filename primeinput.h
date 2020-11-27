#ifndef PRIME_INPUT
#define PRIME_INPUT
#include <Windows.h>
#include <WinUser.h>
#include <string>

using namespace std;
namespace mine {
	enum act{up,down};
	enum button{l,m,r};
	const int pixelwidth = 1920;
	const int pixelheight = 1080;
	void key(act a, int vkc)
	{
		INPUT input = { 0 };
		input.type = INPUT_KEYBOARD;
		input.ki.time = 0;
		input.ki.wVk = vkc;
		if (a == up)
			input.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &input,sizeof(INPUT));
	}
	void mouse(button b, act a)
	{
		INPUT input = { 0 };
		input.type = INPUT_MOUSE;
		input.mi.time = 0;
		switch (b) {
		case l:
			if (a == up)
				input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			else if (a == down)
				input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			break;
		case r:
			if (a == up)
				input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			else if (a == down)
				input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			break;
		case m:
			if (a == up)
				input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			else if (a == down)
				input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			break;
		}
		SendInput(1, &input, sizeof(INPUT));
	}

	void wheel(int n) {
		n *= WHEEL_DELTA;
		INPUT input = { 0 };
		input.type = INPUT_MOUSE;
		input.mi.time = 0;
		input.mi.dwFlags = MOUSEEVENTF_WHEEL;
		input.mi.mouseData = n;
		SendInput(1, &input, sizeof(INPUT));
	}

	void wheel(act a, int n)
	{
		if (a == down)
			n = -n;
		wheel(n);
	}

	void move(int x, int y)
	{
		
		bool absolute = true;
		int scwidth = pixelwidth;
		int scheight = pixelheight;
		INPUT input = { 0 };
		input.type = INPUT_MOUSE;
		input.mi.time = 0;
		if (absolute) {
			input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
			input.mi.dx = 0xffff * x / pixelwidth;
			input.mi.dy = 0xffff * y / pixelheight;
		}
		else {
			int inchwidth = GetSystemMetrics(SM_CXSCREEN);
			int inchheight = GetSystemMetrics(SM_CYSCREEN);
			input.mi.dwFlags = MOUSEEVENTF_MOVE;
			input.mi.dx = (float)x*pixelwidth/inchwidth;
			input.mi.dy = (float)y*pixelwidth / inchwidth;
		}
		
		SendInput(1, &input, sizeof(INPUT));
	}
	
	void input(wstring s)
	{

	}
}
#endif