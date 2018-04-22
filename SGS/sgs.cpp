#include "sgsmach.h"
#include "memory.h"
#include <fstream>
#include "../editor/editor.h"

extern  Editor *editor;

void sgsInterprete() {
	SgsLex l = SgsLex();
	SgsSyntax s = SgsSyntax();
	SgsMachine m = SgsMachine();

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
	catch (SGSLexemeException *le) {
		//cout << l.get() << endl;
		alertInfo(le->message(), "Lexeme error", 0);
		delete le;
	}
	catch (SyntaxException *se) {
		//cout << l.get() << endl;
		alertInfo(se->message(), "Syntax error", 0);
		delete se;
	}
	catch (MachineException *me) {
		//cout << l.get() << endl;
		alertInfo(me->message(), "Machine error", 0);
		delete me;
	}

	s.clearMem();
	m.clearMem();
}

void sgsDebug() {

}