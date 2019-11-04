#ifndef MINE_LOG
#define MINE_LOG
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
using namespace std;
enum State { label, text ,lend};
	
class logger {

	ofstream fo;
	ostream&  o;
	logger(const logger& a):o(a.o) {}
	string indent="    ";
	int indenttime=0;
	vector<string> labelstack;
	State state=text;
public:
	logger(const char *path) :fo(path),o(fo) {}
	logger(ostream& o) :o(o) {}
	template <typename t>
	void log(t s)
	{
		o << s ;
	}
	template <typename t>
	logger& operator<<(t s)
	{
		
		o << s;
		return *this;
	}
	
	logger& operator<<(ostream& (*pfn)(ostream&))
	{
		//used for endl
		o << pfn;
		if (pfn == &endl<char,char_traits<char>>)
			logindent();
		return *this;
	}
	template<>
	logger& operator<<(const char *s)
	{
		return operator<<(string(s));
	}
	template<>
	logger& operator<<(string s)
	{
		if (state == label) {
			logl(s);
			state = text;
		}
		else {
			o << s;
		}
		return *this;
	}
	logger& operator<<(State s)
	{
		switch (s) {
		case lend:
			--indenttime;

			*this << endl;
			o << "</" << labelstack.back() << ">" ;
			*this << endl;
			labelstack.pop_back();
			break;
		default:
			state = s;
			break;
		}
		return *this;
	}
	void logl(string l)
	{
		if (!labelstack.empty() && labelstack.back() == l) {
			labelstack.pop_back();
			--indenttime;
			*this << endl;
			o << "</" << l << ">";
			*this << endl;
		}
		else{
			o << "<" << l << ">";
			++indenttime;
			*this << endl;
			labelstack.push_back(l);
		}
		
	}
	~logger()
	{
		if(fo.is_open())
			fo.close();
	}
private:
	void logindent()
	{
		for (int i = 0; i < indenttime; i++) {
			o << indent;
		}
	}
	
};
//string logger::indent = "\t";
//int logger::indenttime = 0;
//vector<string> logger::labelstack;
//State logger::state = text;
#endif