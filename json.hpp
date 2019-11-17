#ifndef JSON_HPP
#define JSON_HPP
#include <map>
#include <vector>
#include <string>
#include <cassert>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
using std::ifstream;
using std::ofstream;
using sstream = std::stringstream;
using std::map;
using std::string;
using std::vector;
using std::runtime_error;
using std::to_string;
enum json_type{JSON_ARRAY,JSON_BOOL,JSON_NUMBER_FLOAT,JSON_NUMBER_INT,JSON_NULL,JSON_OBJECT,JSON_STRING};
class json;
typedef vector<json> json_array_data;
typedef map<string,json> json_object_data;
class json {
	json_type type_flag;
	//here use pointer to avoid memory leaking 
	union json_data{
		string *s;
		json_array_data *a;
		int i;
		double d;
		json_object_data *o;
		bool b;
	}data;
public:
	//constructors
	json()
	{
		type_flag = JSON_NULL;
		data.s = nullptr;
	}
	json(json_array_data a)
	{
		type_flag = JSON_ARRAY;
		data.a = new json_array_data(a);
	}
	json(bool b)
	{
		type_flag = JSON_BOOL;
		data.b = b;
	}
	json(double d)
	{
		type_flag = JSON_NUMBER_FLOAT;
		data.d = d;
	}
	json(int i)
	{
		type_flag = JSON_NUMBER_INT;
		data.i = i;
	}
	json(json_object_data o)
	{
		type_flag = JSON_OBJECT;
		data.o = new json_object_data(o);
	}
	json(string s)
	{
		type_flag = JSON_STRING;
		data.s = new string(s);
		
	}
	json(const char* s)
	{
		type_flag = JSON_STRING;
		data.s = new string(s);
	}
	//memory control
	json(const json& j)
	{
		type_flag = j.type_flag;
		switch (type_flag) {
		case JSON_ARRAY:
			data.a = new json_array_data(*j.data.a);
			break;
		case JSON_BOOL:
			data.b = j.data.b;
			break;
		case JSON_NULL:
			data.a = nullptr;
			break;
		case JSON_NUMBER_FLOAT:
			data.d = j.data.d;
			break;
		case JSON_NUMBER_INT:
			data.i = j.data.i;
			break;
		case JSON_OBJECT:
			data.o = new json_object_data(*j.data.o);
			break;
		case JSON_STRING:
			data.s = new string(*j.data.s);
			break;
		default:
			throw runtime_error("error_type");
			break;
		}
	}
	json& operator=(const json& j)
	{
		switch (type_flag) {
		case JSON_ARRAY:
			delete data.a;
			break;
		case JSON_OBJECT:
			delete data.o;
			break;
		default:
			break;
		}
		type_flag = j.type_flag;
		switch (j.type_flag) {
		case JSON_ARRAY:
			data.a = new json_array_data(*j.data.a);
			break;
		case JSON_BOOL:
			data.b = j.data.b;
			break;
		case JSON_NULL:
			data.a = nullptr;
			break;
		case JSON_NUMBER_FLOAT:
			data.d = j.data.d;
			break;
		case JSON_NUMBER_INT:
			data.i = j.data.i;
			break;
		case JSON_OBJECT:
			data.o = new json_object_data(*j.data.o);
			break;
		case JSON_STRING:
			data.s = new string(*j.data.s);
			break;
		default:
			throw runtime_error("error_type");
			break;
		}
	}
	~json()
	{
		switch (type_flag) {
		case JSON_ARRAY:
			delete data.a;
			break;
		/*case JSON_BOOL:
		case JSON_NULL:
		case JSON_NUMBER_FLOAT:
		case JSON_NUMBER_INT:*/
		case JSON_OBJECT:
			delete data.o;
			break;
		default:
			break;
		}
	}

	//array and object methods just calling the methods of vector and map
	json& operator[](int index)
	{
		assert(type_flag == JSON_ARRAY);
		return data.a->operator[](index);
	}
	json& operator[](string key)
	{
		assert(type_flag == JSON_OBJECT);
		return data.o->operator[](key);
	}
	json& operator[](const char* key)
	{
		return operator[](string(key));
	}
	json_array_data::iterator begin()
	{
		assert(type_flag == JSON_ARRAY);
		return data.a->begin();
	}
	json_array_data::iterator end()
	{
		assert(type_flag == JSON_ARRAY);
		return data.a->end();
	}
	vector<string> keys()
	{
		assert(type_flag == JSON_OBJECT);
		vector<string>ks;
		for (auto i : *data.o) {
			ks.push_back(i.first);
		}
		return ks;
	}
	size_t size()
	{
		assert(type_flag == JSON_ARRAY || type_flag == JSON_OBJECT);
		if (type_flag == JSON_ARRAY)
			return data.a->size();
		else if (type_flag == JSON_OBJECT)
			return data.o->size();
	}
	//convertor and type
	string type()
	{
		string answer;
		switch (type_flag){
		case JSON_ARRAY:
			answer = "array";
			break;
		case JSON_BOOL:
			answer = "bool";
			break;
		case JSON_NULL:
			answer = "null";
			break;
		case JSON_NUMBER_FLOAT:
		case JSON_NUMBER_INT:
			answer = "number";
			break;
		case JSON_OBJECT:
			answer = "object";
			break;
		case JSON_STRING:
			answer = "string";
			break;
		default:
			answer = "error_type";
			break;

		}
		return answer;
	}

	operator json_array_data()
	{
		assert(type_flag == JSON_ARRAY);
		return *data.a;
	}
	operator bool()
	{
		assert(type_flag == JSON_BOOL);
		return data.b;
	}
	operator double()
	{
		assert(type_flag == JSON_NUMBER_FLOAT);
		return data.d;
	}
	operator int()
	{
		assert(type_flag == JSON_NUMBER_INT);
		return data.i;
	}
	operator map<string, json>()
	{
		assert(type_flag == JSON_OBJECT);
		return *data.o;
	}
	operator string()
	{
		return tostring();
	}
	//io
	void write(string path)
	{
		ofstream fo(path);
		fo << tostring();
		fo.close();
	}
	void read(string path)
	{
		ifstream fi(path);
		sstream buf;
		buf << fi.rdbuf();
		string json_string = buf.str();
		fromstring(json_string);
	}
	//formatter
	string tostring()
	{
		string json_string;
		switch (type_flag) {
		case JSON_ARRAY: {
			json_string = "[";
			json_array_data* a = data.a;
			if (!a->empty()) {
				json_string += a->begin()->tostring();
				for (auto i = a->begin() + 1; i != a->end(); ++i) {
					json_string += "," + i->tostring();
				}
			}
			json_string += "]";
		}
					   break;
		case JSON_BOOL:
			json_string = data.b ? "true" : "false";
			break;
		case JSON_NUMBER_FLOAT:
			json_string = to_string(data.d);
			break;
		case JSON_NUMBER_INT:
			json_string = to_string(data.i);
			break;
		case JSON_OBJECT: {
			json_string = "{";
			json_object_data* o = data.o;
			if (!o->empty()) {
				json_string += "\""+o->begin()->first + "\":" + o->begin()->second.tostring();
				auto i = o->begin();
				++i;
				for (; i != o->end(); ++i) {
					json_string += ",\"" + i->first + "\":" + i->second.tostring();
				}
				json_string += "}";
			}
		}
			break;
		case JSON_NULL:
			json_string = "null";
			break;
		case JSON_STRING:
			json_string = "\""+*data.s+"\"";
			break;
		default:
			json_string = "error_type";
		}
		return json_string;
	}
	void fromstring(const char* pjson_string)
	{
		string json_string(pjson_string);
		fromstring(json_string);
	}
	void fromstring(string& json_string)
	{
		string::iterator i = json_string.begin();
		fromstring(json_string, i,*this);
	}
	void fromstring(string& json_string,string::iterator& i,json& to_fill)
	{
		movewhile(i, isspace);
		//now parse it using stringstream
		if (i == json_string.end() || *i == 'n'||*i==','||*i=='}'||*i==']') {
			to_fill.type_flag = JSON_NULL;
			to_fill.data.a = nullptr;
			if(*i=='n')
				i += 4;
			else if (i != json_string.end()) {
				++i;
			}
		}
		else if (*i == '[') {
			++i;
			to_fill.type_flag = JSON_ARRAY;
			to_fill.data.a = new json_array_data();
			json child;
			for (;;) {
				movewhile(i, isspace);
				if (*i == ']') {
					++i;
					break;
				}
				if (*i == ',') {
					++i;
					movewhile(i, isspace);
				}
				fromstring(json_string, i, child);
				to_fill.data.a->push_back(child);
			}
		}
		else if (*i == 't' || *i == 'f') {
			to_fill.type_flag = JSON_BOOL;
			to_fill.data.b = *i == 't' ? true : false;
			i += *i == 't' ? 4 : 5;
		}
		else if (isdigit(*i)||*i=='-') {
			size_t L = 0;
			size_t start = i - json_string.begin();
			if (json_string.find('.',start)==-1||json_string.find('.',start)>json_string.find(',',start)||json_string.find('.',start)>json_string.find(']',start)) {
				to_fill.type_flag = JSON_NUMBER_INT;
				to_fill.data.i = stoi(json_string.substr(i-json_string.begin()),&L);
				i += L;
			}
			else {
				to_fill.type_flag = JSON_NUMBER_FLOAT;
				to_fill.data.d = stod(json_string.substr(i - json_string.begin()),&L);
				i += L;
			}

		}
		else if (*i == '{') {
			++i;
			to_fill.type_flag = JSON_OBJECT;
			to_fill.data.o = new json_object_data();
			string key;
			json value;
			for (;;) {
				movewhile(i, isspace);
				if (*i == '}') {
					++i;
					break;
				}
				if (*i == ',') {
					++i;
					movewhile(i, isspace);
				}
				assert(*i == '\"'||*i=='\'');
				size_t L = json_string.find(*i, i - json_string.begin() + 1);
				++i;
				key =json_string.substr(i - json_string.begin(), L - (i - json_string.begin()));
				i = json_string.begin() + L + 1;
				movewhile(i, isspace);
				assert(*i == ':');
				++i;
				movewhile(i, isspace);
				fromstring(json_string, i, value);
				to_fill[key] = value;
			}
		}
		else if (*i == '\"' || *i == '\'') {
			size_t L = json_string.find(*i, i - json_string.begin() + 1);
			++i;
			to_fill.type_flag = JSON_STRING;
			to_fill.data.s = new string(json_string.substr(i - json_string.begin(), L - (i-json_string.begin())));
			i = json_string.begin() + L+1;

		}
		else {
			throw runtime_error("error_string");
		}
	}
private:
	static int isspace(int c)
	{
		if (c == '\r' || c == '\n' || c == ' '||c=='\t')
			return true;
		return false;
	}
	void movewhile(string::iterator& i,int (*f)(int))
	{
		for(;;){
			if (!f(*i))
				break;
			++i;
		}
	}
};
#endif