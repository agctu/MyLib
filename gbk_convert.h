#ifndef GBK_CONVERT
#define GBK_CONVERT
#include <string>
#include <codecvt>

namespace mine {
	using namespace std;
	class codecvt_gbk :public codecvt_byname<wchar_t,char,mbstate_t> {
	public:
		codecvt_gbk() :codecvt_byname("chs") {}
	};
	wstring dc_gbk(string s)
	{
		wstring_convert<codecvt_gbk> gbk_converter;
		return gbk_converter.from_bytes(s);
	}
	string ec_gbk(wstring ws)
	{
		wstring_convert<codecvt_gbk>gbk_converter;
		return gbk_converter.to_bytes(ws);
	}
	wstring dc_utf8(string s)
	{
		wstring_convert<codecvt_utf8<wchar_t>> utf8_converter;
		return utf8_converter.from_bytes(s);
	}
	string ec_utf8(wstring ws)
	{
		wstring_convert<codecvt_utf8<wchar_t>>utf8_converter;
		return utf8_converter.to_bytes(ws);
	}
}

#endif