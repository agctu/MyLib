#include<Windows.h>
#include <iostream>
#include <string>
using namespace std;

class Hold
{
public:
	COORD outpoint;
	Hold() {}
	void setup()
	{
		if (outpoint.X != 0 || outpoint.Y != 0)
			return;
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO a;
		GetConsoleScreenBufferInfo(handle, &a);
		outpoint = a.dwCursorPosition;
		COORD next = outpoint;
		next.Y++;
		SetConsoleCursorPosition(handle, next);
	}
	void plus(int X = 0, int Y = 0)
	{
		outpoint.X += X;
		outpoint.Y += Y;
	}
	void holdoutput(string s)
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO a;
		GetConsoleScreenBufferInfo(handle, &a);
		COORD cc = a.dwCursorPosition;
		if (cc.Y - outpoint.Y > 20) {
			outpoint.Y = cc.Y;
		}
		SetConsoleCursorPosition(handle, outpoint);
		cout << "                                              ";
		SetConsoleCursorPosition(handle, outpoint);
		cout << s << endl;
		SetConsoleCursorPosition(handle, cc);
	}
};