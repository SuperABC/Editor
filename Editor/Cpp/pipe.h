#ifndef CPP_GCC_PIPE
#define CPP_GCC_PIPE

#include <windows.h>
#include <string>

class Pipe {
public:
	Pipe();
	~Pipe();

	bool RunProcess(const std::string &process);
	bool StopProcess(void);
	bool GetOutput(const std::string &endStr, int timeout, std::string &outstr);
	bool SetInput(const std::string &cmd);
private:
	HANDLE m_hChildInputWrite;  //用于重定向子进程输入的句柄  
	HANDLE m_hChildInputRead;
	HANDLE m_hChildOutputWrite; //用于重定向子进程输出的句柄    
	HANDLE m_hChildOutputRead;
	PROCESS_INFORMATION m_cmdPI;//cmd进程信息  
};

#endif
