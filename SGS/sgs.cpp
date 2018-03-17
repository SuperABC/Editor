#include "machine.h"
#include "memory.h"
#include <fstream>
#include "../editor/editor.h"

extern  Editor *editor;

void interprete() {
	Lexeme l = Lexeme();
	Syntax s = Syntax();
	Machine m = Machine();

	string input;

	editor->save();
	string fn = editor->actFile()->getPath();
	std::ifstream fin(fn);

	string tmp;
	while (!fin.eof()) {
		getline(fin, tmp);
		input += tmp + '\n';
	}
	try {
		l.input(input.c_str())->parse();
		s.input(l.strId, l.output)->parse();
		m.input(s.output, s.globeClassType, s.globeFunc, s.globeVar)->execute();
	}
	catch (LexemeException *le) {
		//cout << l.get() << endl;
		alertInfo(le->message(), "Lexeme error", 0);
	}
	catch (SyntaxException *se) {
		//cout << l.get() << endl;
		alertInfo(se->message(), "Syntax error", 0);
	}
	catch (MachineException *me) {
		//cout << l.get() << endl;
		alertInfo(me->message(), "Machine error", 0);
	}

	s.clearMem();
	m.clearMem();
}