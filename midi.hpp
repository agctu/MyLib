#ifndef MIDI_H
#define MIDI_H
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include <vector>
#include <string>
#include <map>

using std::map;
using std::string;
using std::vector;
using std::cout;
using std::endl;
namespace mine {
	enum midistatus {
		open=0x90,close=0x80
	};
	map<char, int> notemap = {
		{'c',0},{'C',1},{'d',2},{'D',3},
		{'e',4},{'E',5},{'f',5},{'F',6},
		{'g',7},{'G',8},{'a',9-12},{'A',10-12},
		{'b',11 - 12},{'B',12 - 12},{'0',-1},{'+',-2} };//{ 0,2,4,5,7,9,11 };//from Do to Xi
	HMIDIOUT midihandle;
	
	void initmidi(void)
	{
		midiOutOpen(&midihandle, MIDIMAPPER, 0, 0, CALLBACK_NULL);

	}
	void endmidi(void)
	{
		//pause midi 
		midiOutReset(midihandle);
		midiOutClose(midihandle);
	}
	void setvolume(unsigned short vol)
	{
		midiOutSetVolume(midihandle, vol | vol << 16);
	}
	unsigned long getvolume()
	{
		unsigned long result=0;
		midiOutGetVolume(midihandle, &result);
		return result;
	}
	void midioutmessage(int status,int channel, int data1, int data2)
	{
		/**param: vol 0~127
		**param: note 0~127
		*
		*
		*/
		DWORD message;
		message = status | channel | (data1 << 8) | (data2 << 16);
		midiOutShortMsg(midihandle, message);
	}
	void sendnote(int note,float time = 0.3f, int vol=127)
	{
		midioutmessage(open, 0, note, vol);
		Sleep(time * 1000);
		midioutmessage(close, 0, note, vol);
	}
	void changeinstr(int instr)
	{
		midioutmessage(0xc0, 0, instr, 0);
	}
	struct note {
		int value;
		float time;
		note() {}
		note(int value, float time) :value(value), time(time) {}
	};
	void sendnote(note n, int vol = 127)
	{
		sendnote(n.value, n.time, vol);
	}
	class midiseq {

		vector<note> notes;
		int volume=127;
	public:
		float pai=0.5;
		midiseq(string textnote,float pai):pai(pai)
		{
			int i = 0;
			int ismall, ibig, ilen,value;
			char csmall, cbig, clen,state;
			bool conti = false;
			while (i+3<= textnote.length()) {
				state = textnote[i];
				if (state == '+') {
					++i;
					conti = true;
				}
				else {
					conti = false;
				}
				csmall = textnote[i];
				cbig = textnote[i + 1];
				clen = textnote[i + 2];
				ismall = notemap[csmall];
				ibig = cbig - '0';
				ilen = clen - '0';
				value = ibig * 12 + ismall;
				if (conti) {
					notes[notes.size() - 1].time += pai / ilen;
				}
				else
					notes.push_back(note(value, pai / ilen));
				i+=3;
			}
		}
		
		void play()
		{
			for (auto i : notes) {
			
				cout << i.value << endl;
				if (i.value == -1)
					Sleep(i.time*1000);
				else {
					sendnote(i.value, i.time, volume);
				}
			}
		}
		void setvol(int vol)
		{
			volume = vol;
		}
		string to_string()
		{
			return string();
		}
		int getvol(void)
		{
			return volume;
		}
	};
}

#endif