#ifndef PRIME_INPUT
#define PRIME_INPUT
#include <Windows.h>
#include <WinUser.h>
#include <string>

using namespace std;
namespace mine {
	enum act{up,down};
	enum button{l,m,r};
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
	void wheel(act a, int n)
	{
		if (a == down)
			n = -n;
		n *= WHEEL_DELTA;
		INPUT input = { 0 };
		input.type = INPUT_MOUSE;
		input.mi.time = 0;
		input.mi.dwFlags = MOUSEEVENTF_WHEEL;
		input.mi.mouseData = n;
		SendInput(1, &input, sizeof(INPUT));
	}
	void move(int x, int y)
	{
		int scwidth = 1920;
		int scheight = 1080;
		INPUT input = { 0 };
		input.type = INPUT_MOUSE;
		input.mi.time = 0;
		input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
		input.mi.dx = 0xffff * x / 1920;
		input.mi.dy = 0xffff * y / 1080;
		SendInput(1, &input, sizeof(INPUT));
	}
	void input(wstring s)
	{

	}
}
#endif