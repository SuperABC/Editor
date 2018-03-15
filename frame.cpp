#include "editor.h"

Editor *editor;

void callBack() {

}

void newFile() {
	editor->create();
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
void saveFile() {
	editor->save();
}
void closeFile() {
	editor->close();
}
void exitEditor() {
	exit(0);
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
void editroInst() {
	alertInfo("����SGL��SGS��RWE����Ŀ�ı༭����ûʲô������", "�༭�����",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}
void userInst() {
	alertInfo("��TXT�÷�һ��", "ʹ�÷���",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}

void sgSetup() {
	initWindow(640, 480, "Editor", BIT_MAP);
	initKey();
	initMouse(SG_COORDINATE);

	initMenu();
	int idFile = addMenuList("�ļ�", 0);
	int idEdit = addMenuList("�༭", 0);
	int idHelp = addMenuList("����", 0);
	addMenuItem("�½�\tCtrl + N", idFile, newFile);
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
	addMenuItem("�汾", idAbout, versionInfo);
	addMenuItem("����", idAbout, authorInfo);
	addMenuItem("��˾", idAbout, companyInfo);
	int idInst = addMenuList("˵��", idHelp);
	addMenuItem("�༭�����", idInst, editroInst);
	addMenuItem("ʹ�÷���", idInst, userInst);

	editor = new Editor();
}
void sgLoop() {
	int key;
	vecThree mouse;
	static int time = clock();

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

	if ((clock() - time) % 800 < 400) {
		setColor(255, 255, 255);
		putQuad(editor->cursorPos.x - 1, editor->cursorPos.y + 1,
			editor->cursorPos.x, editor->cursorPos.y + editor->fontSize + 4 - 2, SOLID_FILL);
	}
	else {
		setColor(64, 64, 64);
		putQuad(editor->cursorPos.x - 1, editor->cursorPos.y + 1,
			editor->cursorPos.x, editor->cursorPos.y + editor->fontSize + 4 - 2, SOLID_FILL);
	}

	clearKeyBuffer();
	clearMouseBuffer();
}
