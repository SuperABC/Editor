#include "gcc.h"
#include "../editor/editor.h"

extern  Editor *editor;

std::string Gcc::compile(std::string src) {
	int pos = src.rfind('.');
	std::string dst = std::string(src.begin(), src.begin() + pos) + ".exe";

	std::string output;
	pipe.RunProcess("cmd.exe");
	pipe.GetOutput("", 3000, output);
	pipe.SetInput(std::string("gcc ") + src + " -o " + dst + '\n');
	pipe.GetOutput("", 3000, output);
	while (*output.rbegin() == '>')
		if (output.rfind('\r') == -1)
			output = string(output.begin(), output.begin() + output.rfind('\n'));
		else
			output = string(output.begin(), output.begin() + output.rfind('\r'));

	pipe.StopProcess();
	editor->logPrint(output.data());

	return dst;
}
void Gcc::run(std::string exe) {
	std::string outstr;

	pipe.RunProcess("cmd.exe");
	pipe.GetOutput("", 3000, outstr);

	pipe.SetInput(exe + '\n');
	while (1) {
		pipe.GetOutput("", 201, outstr);
		if (outstr.empty())break;

		outstr = string(outstr.begin() + outstr.find('\n') + 1, outstr.end());
		while(*outstr.rbegin() == '>')
			if(outstr.rfind('\r') == -1)
				outstr = string(outstr.begin(), outstr.begin() + outstr.rfind('\n'));
			else
				outstr = string(outstr.begin(), outstr.begin() + outstr.rfind('\r'));
		editor->logPrint((outstr + '\n').data());
	}

	pipe.StopProcess();
	editor->logPrint("Process end.\n");
}

void cppRun() {
	Gcc gcc;

	editor->save();
	gcc.run(gcc.compile(editor->actFile()->getPath()));
}
void cppDebug() {

}