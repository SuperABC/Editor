#include "editor.h"

Editor *editor;

void callBack() {

}
void openFile() {
	char *src = new char[256];

	selectFile(src, NULL, NULL);
	if (src[0] == '\0')return;

	editor->open(src);
	editor->refresh();
}
void saveFile() {
	editor->save();
}
void closeFile() {
	editor->close();
}
void exitEditor() {
	exit(0);
}

void sgSetup() {
	initWindow(640, 480, "Editor", BIT_MAP);
	initKey();
	initMouse(SG_COORDINATE);

	initMenu();
	int idFile = addMenuList("�ļ�", 0);
	int idEdit = addMenuList("�༭", 0);
	int idHelp = addMenuList("����", 0);
	addMenuItem("��\tCtrl + O", idFile, openFile);
	addMenuItem("����\tCtrl + S", idFile, saveFile);
	addMenuItem("�ر�", idFile, closeFile);
	addMenuItem("�˳�\tF4", idFile, exitEditor);
	addMenuItem("����\tCtrl + Z", idEdit, callBack);
	addMenuItem("����\tCtrl + Y", idEdit, callBack);
	addMenuSeparator(idEdit);
	addMenuItem("����\tCtrl + C", idEdit, callBack);
	addMenuItem("����\tCtrl + X", idEdit, callBack);
	addMenuItem("ճ��\tCtrl + V", idEdit, callBack);
	addMenuItem("���", idEdit, callBack);
	int idAbout = addMenuList("����", idHelp);
	addMenuItem("�汾", idAbout, callBack);
	addMenuItem("����", idAbout, callBack);
	addMenuItem("��˾", idAbout, callBack);
	int idInst = addMenuList("˵��", idHelp);
	addMenuItem("�༭�����", idInst, callBack);
	addMenuItem("ʹ�÷���", idInst, callBack);

	editor = new Editor();
}
void sgLoop() {
	int key;
	vecThree mouse;

	static int first = 1;
	if (first) {
		first = 0;
		setColor(64, 64, 64);
		clearScreen();
	}

	if (biosKey(1)) {
		key = biosKey(0);
		editor->key(key);
	}
	if (biosMouse(1).m) {
		mouse = biosMouse(0);
		editor->mouse(mouse);
	}

	clearKeyBuffer();
	clearMouseBuffer();
}
