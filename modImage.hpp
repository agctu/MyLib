#ifndef MODIMAGE
#define _CRT_SECURE_NO_WARNINGS
#define MODIMAGE
#include<lzw.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <random>
#include <windows.h>
#include <io.h>
#include <bitset>
#include <cstdio>
#pragma pack(1)

namespace mine {
	using namespace std;
	struct gifheader {
		char signature[3];
		char version[3];
	};
	
	struct lsdescriptor{
		unsigned short width;
		unsigned short height;
		unsigned char ctsize : 3;
		unsigned char sorted : 1;
		unsigned char colordepth : 3;
		unsigned char hasgctable : 1;
		unsigned char bgcolor;
		unsigned char paratio;
	};
	struct imgdescriptor {
		unsigned short left;
		unsigned short top;
		unsigned short width;
		unsigned short height;
		unsigned char ctsize : 3;
		unsigned char reserved : 2;
		unsigned char sorted :1 ;
		unsigned char interlaced : 1;
		unsigned char haslctable : 1;
	};
	struct gcextension {
		unsigned char trcflag : 1;
		unsigned char useriflag : 1;
		unsigned char dismthd : 3;
		unsigned char reserved : 3;
		unsigned short delaytime;
		unsigned char trcindex;
	};
	struct bmpheader {
		char type[2];
		unsigned int size;
		unsigned short reserved1;
		unsigned short reserved2;
		unsigned int offbytes;
	};
	struct bmpinfoheader {
		unsigned int size;
		long width;
		long height;
		unsigned short planes;
		unsigned short sizeimage;
		long xpelspermeter;
		long ypelspermeter;
		unsigned int clrused;
		unsigned int ctimportant;
	};
	int clamp(int value, int min = 0, int max = 255)
	{
		if (value > max)
			return max;
		if (value < min)
			return min;
		return value;
	}
	
	struct color {
		static color red, green, blue, yellow, purple, black, white;
		unsigned char r, g, b;
		color(int r, int g, int b) :r(r), g(g), b(b) {}
		color() :r(0), g(0), b(0) {}
		color(int c) :r(c), g(c), b(c) {}
		color reverse()
		{
			return color(0xff - r, 0xff - g, 0xff - b);
		}
		color togrey()
		{
			return color(0.2126 * r + 0.7152 * g + 0.0722 * b);
		}
		color& operator*=(float scl)
		{
			r = clamp(r * scl), g = clamp(g * scl), b = clamp(b * scl);
			return *this;
		}
		color& operator+=(const color& o)
		{
			r = clamp(r + o.r), g = clamp(g + o.g), b = clamp(b + o.b);
			return *this;
		}
		
	};
	color color::red = color(0xff, 0, 0);
	color color::green = color(0, 0xff, 0);
	color color::blue = color(0, 0, 0xff);
	color color::yellow = color(0xff, 0xff, 0);
	color color::purple = color(0xff, 0, 0xff);
	color color::black = color(0);
	color color::white = color(0xff);
	color operator+(const color& a,const color& b)
	{
		return color(a) += b;
	}
	color operator*(const color&a, float scl)
	{
		return color(a) *= scl;
	}
	istream& operator>>(istream& fi,color& c) {
		fi.read((char*)&c, sizeof(c));
		return fi;
	}
	ostream& operator<<(ostream& of, color c) {
		of << "red: " << (int)c.r << " green: " << (int)c.g << " blue: " << (int)c.b;
		return of;
	}
	class bitmap {
	public:
		static bitmap asmixof(const vector<bitmap>& maps)
		{//seems this function has nothing for use
			bitmap mixture(maps[0].height, maps[1].height);
			for (int i = 0; i < mixture.bsize; i++) {
				long long result = 0;
				for (bitmap map : maps) {
					result += map.data[i];
				}
				mixture.data[i] = clamp(result / maps.size());
			}
			return mixture;
		}
		static bitmap whitenoise(int width=512,int height=316)
		{
			height = width*(sqrt(5)-1)/2.f;
			bitmap whitenoise(width, height);
			for (int i = 0; i < whitenoise.bsize; i++) {
				whitenoise.data[i] = rand();
			}
			return whitenoise;
		}
		static bitmap purecolor(color clr, int width = 512, int height = -1)
		{
			if(height==-1)
				height = width * (sqrt(5) - 1) / 2.f;
			bitmap puremap(width,height);
			for (int i = 0; i < puremap.size; i++) {
				puremap.setpixel(clr,i,0);
			}
			return puremap;
		}
		static vector<bitmap> fromgif(const string path,bool debug=false)
		{
			vector<bitmap>maps;
			ifstream input;
			gifheader header;
			lsdescriptor lsdcp;
			vector<color> gctable;
			input.open(path, ifstream::binary);
			/*stringstream databuffer;
			databuffer << input.rdbuf();
			string datas = databuffer.str();*/
			input.seekg(0);
			input.read((char*)&header, sizeof(header));
			input.read((char*)&lsdcp, sizeof(lsdcp));
			if (debug) {
				cout << "signature: " << header.signature << " header: " << header.version << endl;
				cout << "width: " << lsdcp.width << " height: " << lsdcp.height << endl;
				cout << "hasglobal color table: " << (bool)lsdcp.hasgctable << endl;
				cout << "golbal color table size: " << (int)lsdcp.ctsize << endl;
				cout << "background color: " << (int)lsdcp.bgcolor << " pixel aspect ratio: " << (int)lsdcp.paratio << endl;
			}
			if (lsdcp.hasgctable) {
				for (int i = 0; i < pow(2, lsdcp.ctsize + 1); i++) {
					color c;
					input >> c;
					gctable.push_back(c);
				}
			}
			/*if (debug) {
				cout << "		<global color table>" << endl;
				for (color c : gctable)
					cout << c << endl;
			}*/
			unsigned char size;
			unsigned char flag;
			//fordebug//while (input.read((char*)&size,1), printf("%x ", size), true)getchar();

			vector<color>lctable;
			bitmap curframe(lsdcp.width,lsdcp.height);
			gcextension gcontrol;
			imgdescriptor imdcp;
			while (input >> flag, flag != 0x3b) {
				if (flag == 0x21) {//extensions
					unsigned char label;
					input >> label;
					switch (label) {
					case 0xf9:
						input.read((char*)&size,1);
						input.read((char*)&gcontrol, size);
						if (debug) {
							cout << "		<graphic control extension>" << endl;
							cout << "transparent color flag: " << (int)gcontrol.trcflag << " user input flag: " << (int)gcontrol.useriflag << endl;
							cout << "disposalmethod: " << (int)gcontrol.dismthd << " reserved: " << gcontrol.reserved << " delaytime: " << gcontrol.delaytime <<"/100s"<< endl;
							cout << "transparent color index: " << (int)gcontrol.trcindex << endl;
						}
						break;
					case 0xfe: {
						input.read((char*)&size,1);
						char *t = new char[size + 1];
						t[size] = 0;
						input.read(t, size);
						if (debug) {
							cout << "		<comment extension>" << endl;
							cout << t << endl;
							
						}
						delete[] t;
					}
						break;
					case 0xff: {
						input.read((char*)&size,1);
						char *t = new char[size + 1];
						t[size] = 0;
						input.read(t, size);

						if (debug) {
							cout << "		<application extension>" << endl;
							cout << t << endl;
						}
						delete[] t;
					}
						break;
					case 0x01:
						cout << "		<plain text extension>" << endl;
						break;
					}
					while(input>>size,size!=0)
						input.seekg(size, ios::cur);
					
				}
				else if (flag == 0x2c){ //image descriptor and data block
					
					input.read((char *)&imdcp, sizeof(imdcp));
					if (debug) {
						cout << "left: " << imdcp.left << " top: " << imdcp.top << endl;
						cout << "width: " << imdcp.width << " height: " << imdcp.height << endl;
						cout << "color table size: " << (int)imdcp.ctsize << endl;
						cout << "is color table sorted: " << (int)imdcp.sorted << endl;
						cout << "is data interlaced: " << (imdcp.interlaced ? "yes" : "no") << endl;
						cout << "does local color table exist: " << (imdcp.haslctable ? "yes" : "no") << endl;
					}
					if (imdcp.haslctable) {
						for (int i = 0; i < pow(2, imdcp.ctsize + 1); i++) {
							color c;
							input >> c;
							lctable.push_back(c);
						}
						/*if (debug) {
							cout << "		<local color table>" << endl;
							for (color i : lctable) {
								cout << i << endl;
							}
						}*/
					}
					


					unsigned char lzwsize;
					input.read((char*)&lzwsize, 1);
					if (debug) {
						cout << "lzw initial bits size: " << lzwsize + 1;
						cout << "code clear: " << (int)pow(2, lzwsize) << " code end: " << (int)pow(2, lzwsize) + 1 << endl;
					}
					cvector codes;
					while (input.read((char*)&size, 1), size != 0) {
						char *t = new char[size];
						input.read(t, size);
						codes.insert(codes.end(), t, t + size);
						delete[] t;
					}
					//seek -1 cur to adapt to the 0x21 block
					input.seekg(-1, ios::cur);
					cvector indexes = lzwdecode(codes, lzwsize + 1);
					
					if (imdcp.haslctable) {
						curframe = bitmap(curframe.width, curframe.height);
						for (int y = imdcp.top; y < imdcp.top + imdcp.height; y++) {
							for (int x = imdcp.left; x < imdcp.left + imdcp.width; x++) {
								int indindex = (y - imdcp.top)*imdcp.width + (x - imdcp.left);
								if (indindex >= indexes.size())
									break;
								if (gcontrol.trcflag||indexes[indindex] != gcontrol.trcindex)
									curframe.setpixel(lctable[indexes[indindex]], x, y);
							}
						}
					}
					else {
						curframe = bitmap::purecolor(gctable[lsdcp.bgcolor], lsdcp.width, lsdcp.height);
						//pixels output
						for (int y = imdcp.top; y < imdcp.top + imdcp.height; y++) {
							for (int x = imdcp.left; x < imdcp.left + imdcp.width; x++) {
								int indindex = (y - imdcp.top)*imdcp.width + (x - imdcp.left);
								if (indindex >= indexes.size())
									break;
								if(gcontrol.trcflag||indexes[indindex]!=gcontrol.trcindex)
									curframe.setpixel(gctable[indexes[indindex]], x, y);
							}
						}
					}
					
					maps.push_back(curframe.flipy());
					//curframe.save("fgif\\curframe.bmp");
					while (input.read((char*)&size,1), size != 0) {
						input.seekg(size, ios::cur);
					}

				}
			}

		
			

			
			
			input.close();
			return maps;
		}
		int width, height, bwidth, size,bsize;
		unsigned  char * data;
		bitmap(int width, int height) :width(width), height(height),
			bwidth(3 * width),size(width*height),bsize(size * 3),data(new unsigned char[bsize]()) {}
		bitmap(string path,bool search=true) :width(0), height(0) {
			ifstream file;
			if (_access(path.c_str(),0)) {
				if (search) {
					cout << "file not exit,and modified" << endl;
					path += ".bmp";

					if (_access(path.c_str(),0)) {
						cout << "file not found" << endl;

					}
				}
				else
					cout << "file not found" << endl;
			}
			file.open(path, ios::binary);
			if (!file) {
				cout << "wrong file" << endl;
				return;
			}
			file.seekg(0x12);
			file.read((char *)&width, 4);
			file.read((char *)&height, 4);
			bwidth = 3 * width;
			
			size = width * height;
			bsize = size * 3;
			file.seekg(0x36);
			data = new unsigned char[bwidth * height];
			//file.read((char *)data, bwidth * height);
			int mod = bwidth % 4;
			//字节对齐
			for (int i = 0; i < height; i++) {
				file.read((char *)data + bwidth * i, bwidth);
				if (mod != 0)
					file.seekg(4 - mod, ios::cur);
			}
			file.close();
		}
		bitmap(const bitmap& bm):height(bm.height),width(bm.width),size(bm.size),bwidth(bm.bwidth),bsize(bm.bsize)
		{
			try {
				data = new unsigned char[bm.bsize];
			}
			catch (bad_alloc e) {
				cout <<"bitmap(const bitmap& bm) when new is invoked"<< e.what() << endl;
			}
			memcpy(data, bm.data, bm.bsize);
		}
		void save(string path,bool havetail=true)
		{
			//自动添加文件后缀
			if (havetail&&(path.size()<4||path.substr(path.size() - 4, path.size()) != ".bmp"))
				path.append(".bmp");
			ofstream of;
			char head[0x36];
			of.open(path, ios::binary);
			{
				ifstream file;
				file.open("D:/code/mylib/test.bmp", ios::binary);
				if (!file)
					cout << "filehead not found" << endl;
				file.read(head, 0x36);
				file.close();
			}
			if (!of)
				cout << "wrong opening file" << endl;
			of.write(head, sizeof(head));
			of.seekp(0x12);
			of.write((char *)&width, sizeof(width));
			of.write((char *)&height, sizeof(height));

			of.seekp(0, ios::end);
			//of.write((char*)data, bwidth * height);
			int mod = bwidth % 4;
			const char nodata[3] = { 0 };
			for (int i = 0; i < height; i++) {
				of.write((char*)data+bwidth*i, bwidth);
				//四字节补齐
				if (mod != 0)
					of.write(nodata, 4 - mod);
			}

			of.flush();
			of.close();

		}
		void copy(void *start, unsigned int count = -1)
		{
			if (count == -1)
			{
				count = bsize;
			}
			for (int i = 0; i < bsize; i++)
				_memccpy(data, start, sizeof(char), count);
		}
		bitmap& reverse() {
			;
			for (int i = 0; i < size; i++) {
				setpixel(getpixel(i, 0).reverse(), i, 0);
			}
			return *this;
		}
		
		bitmap& togrey()
		{
			for (int i = 0; i < size; i++) {
				setpixel(getpixel(i, 0).togrey(), i, 0);
			}
			return *this;
		}
		bitmap& setpixel(color clr, int x, int y)
		{
			int bloc = 3 * (y*width + x);
			data[bloc + 0] = clr.b, data[bloc + 1] = clr.g, data[bloc + 2] = clr.r;
			return *this;
		}
		bitmap& setpixel(unsigned int r, unsigned int g, unsigned int b, int x, int y)
		{
			return setpixel(color(r, g, b), x, y);
		}
		color getpixel(int x, int y)const
		{
			int bloc = 3 * (y*width + x);
			return color(data[bloc + 2], data[bloc + 1], data[bloc + 0]);
		}
		//this series of scales return a new instance scaled
		bitmap scale(float xscale, float yscale)
		{//this hasn't been done well need extra work on it 
		//a bitmap is returned might be impropriate;
			int nwidth = width * xscale, nheight = height * yscale;
			return scale(nwidth, nheight);
		}
		bitmap scale(int nwidth, int nheight)
		{
			if (nwidth < 0 || nheight < 0)
				throw runtime_error("need a propriate scalevalue");
			bitmap nbitmap(nwidth, nheight);
			for (int x = 0; x < nwidth; x++)
				for (int y = 0; y < nheight; y++)
					nbitmap.setpixel(getpixel((float)x / nwidth * width, (float)y / nheight * height), x, y);
			return nbitmap;

		}
		bitmap scale(float xyscale)
		{
			return scale(xyscale, xyscale);
		}
		bitmap scaleas(const bitmap& bm)
		{
			return scale(bm.width, bm.height);
		}
		bitmap& setbitmap(const bitmap& b, int x, int y)
		{
			for (int i = 0; i < b.width; i++)
				for (int j = 0; j < b.height; j++)
					setpixel(b.getpixel(i, j), i + x, j + y);
			return *this;
		}
		bitmap& mixedwith(color clr, float rate = 0.5)
		{
			clr *= rate / (1 + rate);
			for (int i = 0; i < size; i++) {
				setpixel(getpixel(i, 0)*(1 / (1 + rate))+clr, i, 0);
			}
			return *this;
		}
		bitmap& mixedwith(const bitmap& o, float rate=0.5)
		{
			if (o.width != width || o.height != height) {
				throw runtime_error("the two bitmaps should have the same width and height");
			}
			if (rate > 1 || rate < 0)
				throw runtime_error("rate should be between 0 and 1");
			for (int i = 0; i < o.bsize; i++) {
				data[i] = data[i] / (1 + rate) + o.data[i] * rate / (1 + rate);
			}
			return *this;
		}
		
		bitmap& operator+=(const bitmap object)
		{
			if (object.width > width || object.height > height) {
				throw runtime_error("the plused bitmap should be smaller both in height and width than the plusing bitmap");
			}
			for(int x=0;x<object.width;x++)
				for (int y = 0; y < object.height; y++) {
					color a = getpixel(x, y), b = object.getpixel(x, y);
					setpixel(clamp(a.r + b.r), clamp(a. g + b.g), clamp(a.b, b.b), x, y);
				}
			
			return *this;
		}
		bitmap& operator*=(float s)
		{
			for (int i = 0; i < bsize;i++) {
				data[i] = clamp(data[i] * s);
			}
			return *this;
		}
		bitmap& operator/=(float s)
		{
			return *this *= 1 / s;
		}
		bitmap& flipy()
		{
			unsigned char * line = new unsigned char[bwidth];
			for (int i = 0; i < height / 2; i++) {
				memcpy(line, data + bwidth * i, bwidth);
				memcpy(data + bwidth * i, data + bwidth * (height - 1 - i), bwidth);
				memcpy(data + bwidth * (height - 1 - i), line, bwidth);
			}
			return *this;
		}
		~bitmap() {
			//cout << "~bitmap() called" << endl;
			delete[] data;
		}
		bitmap& operator=(const bitmap& bm)  
		{
			if (this == &bm)
				return *this;
			height = bm.height, width = bm.width, size = bm.size, bwidth = bm.bwidth, bsize = bm.bsize, data = new unsigned char[bm.bsize]();
			memcpy(data, bm.data, bm.bsize);
			return *this;
		}
		operator string()
		{
			string danwei = "BKMGT";
			stringstream temp;
			temp << "width: " << width << "height: " << height << "sizeofdata: ";
			int count = 0;
			float size = bwidth * height;
			while (size > 1024) {
				size /= 1024;
				++count;
			}
			temp << size << danwei[count];
			return temp.str();
		}
		color *begin() { return (color*)data; }
		color *end() { return (color*)(data+bsize); }
		vector<color>getline(int y)
		{
			vector <color> line(width);
			memcpy(&line[0], data + bwidth * y,bwidth);
			return line;
		}
		vector<color>getcolomn(int x)
		{
			vector<color> colomn(height);
			for (int y = 0; y < height; y++) {
				colomn.push_back(getpixel(x, y));
			}
			return colomn;
		}
		void setline(vector<color> line,int y)
		{
			memcpy(data + bwidth * y, &line[0], bwidth);
		}
		void setcolomn(vector<color> colomn, int x)
		{
			for (int y = 0; y < height; y++) {
				setpixel(colomn[y], x, y);
			}
		}
	private:
		

	};
	bitmap operator+(const bitmap& a,const bitmap& b) {
		if (a.width <= b.width&&a.height <= b.height)
			return bitmap(b) += a;
		else if (a.width >= b.width&&a.height >= b.height)
			return bitmap(a) += b;
		else
			throw runtime_error("one of the bitmaps should be large than the other both on width and height");
		
	}
	bitmap operator*(const bitmap& a, float b)
	{
		return bitmap(a) *= b;
	}
	template<class T> T mix(T a, T b, float rate=0.5f)
	{
		return a * (rate / (1 + rate)) + b * (1 / (1 + rate));
	}

}
#pragma pack()
#endif