#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <iostream>
#include <log.hpp>
#pragma comment(lib,"ws2_32.lib")

using namespace std;
logger mlog(cout);
namespace mine {
	void showerror()
	{
		mlog << label << "wsaerror";
		int wsaerror = WSAGetLastError();
		char error[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, wsaerror, NULL, error, sizeof(error), NULL);
		mlog << label << "errorcode" << wsaerror<< lend << label << "error text" <<error<< lend << lend;
		
	}
	class msocket {
		static WSADATA wsadata;
		static int count;
		string sock_address;
		int sock_family;
		int sock_port;
		int sock_type;
		SOCKET sock;
		string log;
	public:
		msocket():log("Not inited") 
		{
			add();
		}
		msocket(const msocket& s) :sock_address(s.sock_address), sock_family(s.sock_family), sock_type(s.sock_type), sock_port(s.sock_port), sock(s.sock)
		{
			//this function only to add the total count?
			add();
			log = "copy from others";
		}
		msocket(int family,int type):sock_family(family),sock_type(type),sock_address(""),sock_port(0)
		{
			
			add();
			
			sock = ::socket(sock_family, sock_type, IPPROTO_TCP);
			
			log = "Successful";
		}
		/*msocket& operator=(const msocket& s)
		{
			sock_address = s.sock_address;
			sock_family = s.sock_family;
			sock_type = s.sock_type;
			sock_port = s.sock_port;
			sock = s.sock;

		}*/
		bool bind(string addr,int port)
		{
			sock_address = addr;
			sock_port = port;

			SOCKADDR_IN addrin;
			addrin.sin_family = sock_family;
			inet_pton(sock_family,sock_address.c_str(),&addrin.sin_addr);
			//addrin.sin_addr.s_addr = inet_addr(sock_address.c_str());
			addrin.sin_port = htons(sock_port);
			if (::bind(sock, (SOCKADDR*)&addrin, sizeof(addrin)) != 0) {
				int wsaerror = WSAGetLastError();
				char error[256];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
					wsaerror, NULL, error, sizeof(error), NULL);
				log = string() + "ERROR:[" + to_string(wsaerror) + "] " + error;
				return false;
			}
			log = "Successfully bind";
			return true;
		}
		bool connect(string addr, int port = 80)
		{
			SOCKADDR_IN saddr;
			saddr.sin_family = sock_family;
			inet_pton(sock_family, addr.c_str(), &saddr.sin_addr);
			saddr.sin_port = htons(port);
			return ::connect(sock, (SOCKADDR*)&saddr, sizeof(saddr)) == 0;
		}
		bool listen(int maxnum)
		{
			return 0 == ::listen(sock, maxnum);
		}
		msocket accept()
		{
			char buf[64];
			msocket ret;
			int len=sizeof(SOCKADDR);
			SOCKADDR_IN sockaddr; 
			
			SOCKET outsock = ::accept(sock, (SOCKADDR*)&sockaddr, &len);
			
			inet_ntop(sock_family, &sockaddr.sin_addr, buf, sizeof(buf));

			ret.sock_address = buf; 
			ret.sock_family = sock_family;
			ret.sock_port = htons(sockaddr.sin_port);
			ret.sock_type = sock_type;
			ret.sock = outsock;
			return ret;
		}
		int send(string bytes)
		{
			return send(&bytes[0], bytes.size());
		}
		int send(void* data, int len)
		{
			mlog << label << "SEND";
			mlog << label << "NEEDTOSEND" << len << lend;
			int retcode=::send(sock, (const char*)data, len, 0);
			if (retcode > 0)
				mlog << label << "SEND " << retcode << " b" << lend;
			else if (retcode < 0)
				showerror();
			else
				mlog << "UNKNOW CONDITION,YOU REALLY NEED TO LOOK UP THE DOCUMENTS";
			mlog << lend;
			return retcode;
		}
		string recv(int len=0)
		{
			if (len == 0)
				len = 1024;
			
			char *data = new char[len]();
			/*if (::recv(sock, data,len, 0))
				return string();*/
			int state;
			state=::recv(sock, data, len, 0);
			mlog << label << "RECV";
			if (state > 0)
				mlog << label << "length" << state <<endl<< lend;
			else if (len < 0)
				mlog << label << "error" << state << endl<<lend;
			else
				mlog << "completely receiving file" << endl;
			string rets = data;
			mlog << lend;
			delete[] data;
			return string(rets);
		}
		void close()
		{
			closesocket(sock);
		}
		~msocket()
		{
			dcrs();
		}
	private:
		static bool loadwsa()
		{
			//WSAStartup 调用成功时返回0
			return WSAStartup(0x202, &wsadata) == 0;
		}
		static void unloadwsa()
		{
			WSACleanup();
		}
		static void add()
		{
			if (count == 0)
				loadwsa();
			++count;
		}
		static void dcrs()
		{
			--count;
			if (count == 0)
				unloadwsa();
		}
	};
	WSADATA msocket::wsadata;
	int msocket::count = 0;
}