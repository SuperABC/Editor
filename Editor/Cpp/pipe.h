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
	HANDLE m_hChildInputWrite;  //�����ض����ӽ�������ľ��  
	HANDLE m_hChildInputRead;
	HANDLE m_hChildOutputWrite; //�����ض����ӽ�������ľ��    
	HANDLE m_hChildOutputRead;
	PROCESS_INFORMATION m_cmdPI;//cmd������Ϣ  
};

#endif
