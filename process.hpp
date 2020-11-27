#ifndef PROCESS_HPP
#define PROCESS_HPP
#include <Windows.h>
#include <string>
namespace mine {
	class pipe {
		HANDLE hRead, hWrite;
		SECURITY_ATTRIBUTES sa;
		static const int ioSize = 1 << 10;
	public:
		bool created = false;
		//easy approach with no parameters. create an unnamed pipe.
		pipe()
		{
			fillSA();
			created = CreatePipe(&hRead, &hWrite, &sa, 0);
		}

		HANDLE getReadHandle()
		{
			return hRead;
		}
		HANDLE getWriteHandle()
		{
			return hWrite;
		}
		void close()
		{
			CloseHandle(hRead);
			CloseHandle(hWrite);

		}
		//blocking when nothing to read at first time
		size_t read(char *s, size_t len)
		{
			DWORD nRead = ioSize;
			size_t sumRead = 0;
			while (sumRead < len) {
				ReadFile(hRead, s, len, &nRead, NULL);
				sumRead += nRead;
				s += nRead;
				if (nLeftToRead() == 0)break;
			}
			return sumRead;
		}
		size_t write(char *s, size_t len)
		{
			DWORD nWrite = ioSize;
			size_t sumWrite = 0;
			while (sumWrite < len) {
				WriteFile(hWrite, s, len, &nWrite, NULL);
				sumWrite += nWrite;
				s += nWrite;
			}
			return sumWrite;
		}
		//read the size of bytes left to read in the pipe not read.
		size_t nLeftToRead()
		{
			DWORD dwAvail;
			PeekNamedPipe(hRead, NULL, 0, NULL, &dwAvail, NULL);
			return dwAvail;
		}
		~pipe()
		{
			close();
		}
	private:
		//define copy constructor as private to avoid ~pipe called in parameter transmission.
		//because i am lazy to write the code about copy control.
		pipe(const pipe&) {}
		void fillSA()
		{
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = true;
			sa.lpSecurityDescriptor = NULL;
		}
	};

	class process {
		PROCESS_INFORMATION pi = { 0 };
		STARTUPINFO si;
		std::string name;
	public:
		bool created = false;
		//create a process with the output as writepipe,and input as readpipe
		process(std::string name, pipe& read_pipe, pipe& write_pipe) :name(name)
		{
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			//window not showed or the parameter does not work?
			si.wShowWindow = SW_HIDE;
			//assure the subprocess use the inherited handles.
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.hStdOutput = write_pipe.getWriteHandle();
			si.hStdError = write_pipe.getWriteHandle();
			si.hStdInput = read_pipe.getReadHandle();
			created = CreateProcess(NULL, (char*)this->name.c_str(), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi);
		}
		bool isrun()
		{
			DWORD dwExitCode;
			GetExitCodeProcess(pi.hProcess, &dwExitCode);
			return dwExitCode == STILL_ACTIVE;
		}
	private:
		process(const process&) {}
	};
}
#endif