#include <string>
#include <vector>
#include <deque>
#include <iostream>
#include <cmath>
#include <utility>
#include <map>
using namespace std;
typedef vector<unsigned char> cvector;
cvector operator+(const cvector&a, const cvector&b)
{
	cvector t = a;
	t.insert(t.end(), b.begin(), b.end());
	return t;
}
cvector operator+(const cvector&a, unsigned char c)
{
	cvector t = a;
	t.push_back(c);
	return t;
}
cvector& operator+=(cvector &a,const cvector &b)
{
	a.insert(a.end(), b.begin(), b.end());
	return a;
}
cvector& operator+=(cvector &a, unsigned char c)
{
	a.push_back(c);
	return a;
}
class bitstream {
	deque<unsigned char> bitdata;
	int bitlen;
	int startbit;
public:
	bitstream():bitdata(deque<unsigned char>()) ,bitlen(0),startbit(0){}
	bitstream(void *start, int nbyte):startbit(0)
	{
		copy((unsigned char*)start, (unsigned char*)start + nbyte, back_inserter(bitdata));
		bitlen = 8 * nbyte;
	}
	bitstream(vector<unsigned char> s)
	{
		*this=bitstream(&s[0], s.size());
	}
	void push(int bits, int nbit)
	{
		while (startbit+bitlen + nbit > 8 * bitdata.size()) {
			bitdata.push_back('\0');
		}
		
		for (int i = bitlen+startbit,newbitlen=bitlen+nbit; i < startbit+newbitlen; i++) {
			unsigned char bt = bits % 2;
			/*++bitlen;
			cout << (int)bt;
			if (bitlen % 8 == 0)
				cout << " ";*/
			bits >>= 1;
			if (bt == 0) {
				bitdata[i / 8] &= ~(1 << i % 8);
			}
			else {
				bitdata[i / 8] |= (1 << i % 8);
			}
		}
		bitlen += nbit;
	}
	int pop(int nbit)
	{
		int bits=0;
		for (int i = startbit; i < startbit + nbit; i++) {
			int move = i - startbit - i % 8;
			int bit = bitdata[i / 8] & (1 << i % 8);
			
			if (move >= 0) {
				bit <<= move;
			}
			else {
				bit >>= -move;
			}
			bits |= bit;
			/*bits |= (i - startbit - i % 8)>=0
				?(bitdata[i / 8] & (1 << i % 8))<< (i - startbit - i % 8)
				: (bitdata[i / 8] & (1 << i % 8))>> -(i - startbit - i % 8);*/
		}
		startbit += nbit;
		bitlen -= nbit;
		if (startbit >= 8) {
			int time = startbit / 8;
			for (int i = 0; i < time; i++) {
				bitdata.pop_front();
			}
			startbit %= 8;
			
		}
		return bits;
	}
	cvector getcvector()
	{//return is not bit stream in bytes there's a offset
		cvector t;
		copy(bitdata.begin(), bitdata.end(), back_inserter(t));
		return t;
	}
};
cvector lzwencode(string charstream,int rinitsize)
{//initsize : the bitlength
	int bsize = rinitsize;
	
	bitstream bcodestream;
	bcodestream.push(pow(2, bsize-1), bsize);
	vector<string> dict;
	for (int i = 0,size=pow(2,bsize); i < size; i++) {
		dict.push_back(""+unsigned char(i));
	}
	dict.push_back("");
	dict.push_back("");
	string p = "";
	for (int i = 0; i < charstream.length(); i++) {
		char c = charstream[i];
		if (dict.size() > pow(2, bsize)) {
			/*if (dict.size() > pow(2, 12)) {
				codestream.push
			}
			else {

			}*/
				++bsize;
		}
		
		if (find(dict.begin(), dict.end(), p + c)!=dict.end()) {
			p += c;
		}
		else {
			dict.push_back(p + c);
			bcodestream.push(find(dict.begin(),dict.end(),p)-dict.begin(),bsize);
			p = c;

		}
	}
	bcodestream.push(find(dict.begin(), dict.end(), p) - dict.begin(), bsize);
	bcodestream.push(pow(2,rinitsize - 1) + 1, bsize);
	
	return bcodestream.getcvector();
}
cvector lzwdecode(cvector codestream,int rinitsize)
{
	int bsize = rinitsize;
	cvector charstream;
	bitstream bcodestream(codestream);
	vector<cvector> dict;
	const int clear = pow(2, rinitsize - 1);
	const int end = clear + 1;
	cvector p, s;
	int c;
	while ((c = bcodestream.pop(bsize)) != end ) {
		if (c == clear) {
			dict.clear();
			for (int i = 0, size = pow(2, rinitsize-1); i < size; i++) {
				cvector t;
				t.push_back((unsigned char)i);
				dict.push_back(t);
			}
			dict.push_back(cvector());
			dict.push_back(cvector());
			bsize = rinitsize;
			p.clear(), s.clear();
			continue;
		}
		if (c == clear + 1) {
			break;
		}
		if (c >= dict.size()) {
			
			dict.push_back(p+p[0]);
			charstream += p;
			p = dict[c];
		}
		else if (c < dict.size()) {
			if (p .size()!=0)
				dict.push_back(p + dict[c][0]);
			charstream += p;
			p = dict[c];
		}
		else {
			cout << "bad encode" << endl;
			return cvector();
		}
		if (bsize!=12 && dict.size() + 1 > pow(2, bsize)) {
			
				++bsize;
			
		}
	}
	charstream += p;
	return charstream;
}