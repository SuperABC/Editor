#include <sstream>
#include "editor/editor.h"
#include "sgs/machine.h"

Editor *editor;

/* Menu call. */
void callBack() {
	
}

void newFile() {
	char dir[256] = { 0 };
	selectDir(dir, NULL);
	if (dir[0] == 0)return;
	editor->create(dir);
	editor->refresh();
}
void openFile() {
	char *src = new char[256];
	memset(src, 0, 256);

	selectFile(src, NULL, NULL);
	if (src[0] == '\0')return;

	editor->open(src);
	editor->refresh();
}
void explorerDir() {
	string tmp = string("explorer.exe ") + editor->actFile()->getDir();
	WinExec(tmp.data(), WM_CREATE);
}
void cmdDir() {
	string tmp = string("cmd.exe /k cd ") + editor->actFile()->getDir();
	WinExec(tmp.data(), WM_CREATE);
}
void saveFile() {
	editor->save();
}
void resaveFile() {
	char *src = new char[256];
	memset(src, 0, 256);

	selectSave(src, NULL, (char *)"sgs");
	if (src[0] == '\0')return;

	editor->save(src);
	editor->refresh();
}
void closeFile() {
	editor->close();
}
void closeAll() {
	while (editor->getNum() > 0)
		editor->close();
}
void exitEditor() {
	exit(0);
}
void undoOperate() {

}
void redoOperate() {

}
void textCopy() {
	vector<string> cont = editor->copy();
	string str = "";
	for (auto s : cont)str += s;
	copyText(str.data());
}
void textCut() {
	textCopy();
	editor->cut();
}
void textPaste() {
	char *str = (char *)pasteText();
	std::istringstream sin(str);
	vector<string> input;
	string tmp;
	while (!sin.eof()) {
		getline(sin, tmp);
		input.push_back(tmp);
	}
	editor->paste(input);
}
void textClear() {
	editor->cut();
}
void textChoose() {
	editor->choose();
}
void inverseComment() {

}
void inverseCapital() {

}
void interpRun() {
	createThread(interprete);
}
void interpDebug() {

}
void versionInfo() {
	alertInfo("���ڻ�û����ɵ�һ��Ӵ~�ٵȵȰ�", "�汾",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}
void authorInfo() {
	alertInfo("���ߣ���������", "����",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}
void companyInfo() {
	alertInfo("�㽭��ѧ��ȪУ��32��420", "��˾",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}
void editorInst() {
	alertInfo("����SGL��SGS��RWE����Ŀ�ı༭����ûʲô������", "�༭�����",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}
void userInst() {
	alertInfo("��TXT�÷�һ��", "ʹ�÷���",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}

/* Widget call. */
void windowResize(int x, int y) {
	editor->resize(x, y);
	editor->refresh();
}
void logClearCall(widgetObj *w, int x, int y, int status) {
	mouseClickDefault(w, x, y, status);
	if(w->status & WIDGET_SELECTED)editor->logClear();
}
void logCopyCall(widgetObj *w, int x, int y, int status) {
	mouseClickDefault(w, x, y, status);
	if (w->status & WIDGET_SELECTED)copyText((char *)getWidgetByName("log")->content);
}
void scrollVertMove(widgetObj *w, int x, int y) {
	mouseMoveScrollVert(w, x, y);
	if (w->status & WIDGET_PRESSED) {
		if (editor->actFile()->startLine() != w->value) {
			editor->actFile()->setStart(w->value);
			editor->refresh();
		}
	}
}
void scrollVertClick(widgetObj *w, int x, int y, int status) {
	mouseClickScrollVert(w, x, y, status);
	if (w->status & WIDGET_PRESSED) {
		editor->actFile()->setStart(w->value);
		editor->refresh();
	}
}
void scrollHorizMove(widgetObj *w, int x, int y) {
	mouseMoveScrollHoriz(w, x, y);
}
void scrollHorizClick(widgetObj *w, int x, int y, int status) {
	mouseClickScrollHoriz(w, x, y, status);
}

/* Initialize func. */
void layoutMenu() {
	initMenu();

	int idFile = addMenuList("�ļ�", 0);
	int idEdit = addMenuList("�༭", 0);
	int idComp = addMenuList("������", 0);
	int idTool = addMenuList("����", 0);
	int idHelp = addMenuList("����", 0);

	addMenuItem("�½�\tCtrl + N", idFile, newFile);
	addMenuItem("��\tCtrl + O", idFile, openFile);
	int idDir = addMenuList("���ļ���", idFile);
	addMenuItem("��Դ������", idDir, explorerDir);
	addMenuItem("������", idDir, cmdDir);
	addMenuItem("����\tCtrl + S", idFile, saveFile);
	addMenuItem("���Ϊ", idFile, resaveFile);
	addMenuItem("�ر�", idFile, closeFile);
	addMenuItem("ȫ���ر�", idFile, closeAll);
	addMenuSeparator(idFile);
	addMenuItem("�˳�\tF4", idFile, exitEditor);

	addMenuItem("����\tCtrl + Z", idEdit, undoOperate);
	addMenuItem("����\tCtrl + Y", idEdit, redoOperate);
	addMenuItem("������¼", idEdit, callBack);
	addMenuSeparator(idEdit);
	addMenuItem("����\tCtrl + C", idEdit, textCopy);
	addMenuItem("����\tCtrl + X", idEdit, textCut);
	addMenuItem("ճ��\tCtrl + V", idEdit, textPaste);
	addMenuItem("���", idEdit, textClear);
	addMenuSeparator(idEdit);
	addMenuItem("���/ɾ��ע��", idEdit, inverseComment);
	addMenuItem("��Сдת��", idEdit, inverseCapital);

	addMenuItem("��������\tCtrl + R", idComp, interpRun);
	addMenuItem("���͵���\tCtrl + D", idComp, interpDebug);
	addMenuSeparator(idComp);
	addMenuItem("����������", idComp, callBack);

	int idOption = addMenuList("ѡ��", idTool);
	addMenuItem("��ʾ����", idOption, callBack);
	addMenuItem("��������", idOption, callBack);
	addMenuItem("��������", idOption, callBack);
	int idPlugin = addMenuList("���", idTool);
	addMenuItem("������", idPlugin, callBack);

	int idAbout = addMenuList("����", idHelp);
	addMenuItem("�汾", idAbout, versionInfo);
	addMenuItem("����", idAbout, authorInfo);
	addMenuItem("��˾", idAbout, companyInfo);
	int idInst = addMenuList("˵��", idHelp);
	addMenuItem("�༭�����", idInst, editorInst);
	addMenuItem("ʹ�÷���", idInst, userInst);

}

/* Main frame. */
void sgSetup() {
	initWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Editor", BIT_MAP);
	setResizeable();
	resizeFuntion(windowResize);
	initKey();
	initMouse(SG_COORDINATE);

	editor = new Editor("config.ini");
	layoutMenu();
}
void sgLoop() {
	int key;
	vecThree mouse;
	static int time = clock();

	static int first = 1;
	if (first) {
		first = 0;
		editor->refresh();
	}
	
	if (biosKey(1)) {
		key = biosKey(0);
		if (key == SG_F4)exitEditor();
		if (key == (SG_CTRLBIT | 'n'))newFile();
		if (key == (SG_CTRLBIT | 'o'))openFile();
		if (key == (SG_CTRLBIT | 's'))saveFile();
		if (key == (SG_CTRLBIT | 'z'))undoOperate();
		if (key == (SG_CTRLBIT | 'y'))redoOperate();
		if (key == (SG_CTRLBIT | 'c'))textCopy();
		if (key == (SG_CTRLBIT | 'x'))textCut();
		if (key == (SG_CTRLBIT | 'v'))textPaste();
		if (key == (SG_CTRLBIT | 'a'))textChoose();
		if (key == (SG_CTRLBIT | 'r'))interpRun();
		if (key == (SG_CTRLBIT | 'd'))interpDebug();

		editor->key(key);
	}
	if (biosMouse(1).m) {
		mouse = biosMouse(0);
		editor->mouse(mouse);
	}
	if (mouseStatus(SG_LEFT_BUTTON) == SG_BUTTON_DOWN) {
		editor->drag(mousePos());
	}

	if (editor->cursorPos.y >= 0 && editor->cursorPos.y + editor->fontSize + 4 < editor->editorSize.y) {
		if ((clock() - time) % 800 < 400) {
			setColor(editor->cursorColor.r, editor->cursorColor.g, editor->cursorColor.b);
			putQuad(editor->editorBase.x + editor->cursorPos.x - 1,
				editor->editorBase.y + editor->cursorPos.y + 1,
				editor->editorBase.x + editor->cursorPos.x,
				editor->editorBase.y + editor->cursorPos.y + editor->fontSize + 4 - 4,
				SOLID_FILL);
		}
		else {
			setColor(64, 64, 64);
			putQuad(editor->editorBase.x + editor->cursorPos.x - 1,
				editor->editorBase.y + editor->cursorPos.y + 1,
				editor->editorBase.x + editor->cursorPos.x,
				editor->editorBase.y + editor->cursorPos.y + editor->fontSize + 4 - 4,
				SOLID_FILL);
		}
	}
}
