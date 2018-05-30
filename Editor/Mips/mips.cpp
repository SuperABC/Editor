#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include "mipsasm.h"
#include "mipsemu.h"
#include "../editor/editor.h"

extern  Editor *editor;

void step(widgetObj *w, int x, int y, int status) {
	mouseClickDefault(w, x, y, status);
	if (w->status & WIDGET_SELECTED) {
		int line = editor->dbg->step();
		editor->actFile()->debugLine = line;
		editor->actFile()->setFocus(line, editor->editorSize.y, editor->fontSize);
		return;
	}
}
void proc(widgetObj *w, int x, int y, int status) {
	mouseClickDefault(w, x, y, status);
	if (w->status & WIDGET_SELECTED) {
		return;
	}
}
void stop(widgetObj *w, int x, int y, int status) {
	mouseClickDefault(w, x, y, status);
	if (w->status & WIDGET_SELECTED) {
		deleteWidgetByName("stop");
		deleteWidgetByName("proc");
		deleteWidgetByName("step");

		widgetObj *vertScroll = getWidgetByName("vert");
		editor->editorSize.x += DEBUGGER_WIDTH;
		vertScroll->pos.x = editor->editorBase.x + editor->editorSize.x - vertScroll->size.x;
		editor->debugging = false;
		editor->actFile()->debugLine = 0;
		return;
	}
}

void mipsAssemble() {
	MipsLex l = MipsLex();
	MipsAsm a = MipsAsm();

	string input;

	editor->save();
	string fn = editor->actFile()->getPath();
	std::ifstream fin(fn);

	string tmp;
	while (!fin.eof()) {
		getline(fin, tmp);
		input += tmp + '\n';
	}

	l.input(input.data())->parse();
	a.input(l.strId, l.output)->parse();

	string output;
	output = editor->actFile()->getPath();
	output.pop_back();
	output += "coe";
	a.write(output);
}
void mipsEmulate() {
	MipsLex l = MipsLex();
	editor->dbg = new MipsEmu();

	string input;

	editor->save();
	string fn = editor->actFile()->getPath();
	std::ifstream fin(fn);

	string tmp;
	while (!fin.eof()) {
		getline(fin, tmp);
		input += tmp + '\n';
	}

	l.input(input.data())->parse();
	((MipsEmu *)editor->dbg)->input(l.strId, l.output)->parse();
	((MipsEmu *)editor->dbg)->prepare();

	//UI setting.
	widgetObj *vertScroll = getWidgetByName("vert");
	editor->editorSize.x -= DEBUGGER_WIDTH;
	vertScroll->pos.x = editor->editorBase.x + editor->editorSize.x - vertScroll->size.x;

	int baseX = editor->editorBase.x + editor->editorSize.x;
	int baseY = editor->editorBase.y - 20;

	widgetObj *step, *proc, *stop;
	step = newWidget(SG_BUTTON, (SGstring)"step");
	step->pos.x = baseX + 40;
	step->pos.y = baseY + 20;
	step->size.x = 60;
	step->size.y = 24;
	strcpy((char *)step->content, "step/F7");
	step->mouseClick = (mouseClickCall)::step;
	registerWidget(step);
	free(step);
	proc = newWidget(SG_BUTTON, (SGstring)"proc");
	proc->pos.x = baseX + 130;
	proc->pos.y = baseY + 20;
	proc->size.x = 60;
	proc->size.y = 24;
	strcpy((char *)proc->content, "proc/F8");
	proc->mouseClick = (mouseClickCall)::proc;
	registerWidget(proc);
	free(proc);
	stop = newWidget(SG_BUTTON, (SGstring)"stop");
	stop->pos.x = baseX + 220;
	stop->pos.y = baseY + 20;
	stop->size.x = 60;
	stop->size.y = 24;
	strcpy((char *)stop->content, "stop/Esc");
	stop->mouseClick = (mouseClickCall)::stop;
	registerWidget(stop);
	free(stop);
}