#include "editor/editor.h"
#include "sgs/machine.h"

Editor *editor;

int ctrl = 0;

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
void exitEditor() {
	exit(0);
}
void interpRun() {
	createThread(interprete);
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
	addMenuItem("��Դ������", idDir, callBack);
	addMenuItem("������", idDir, callBack);
	addMenuItem("����\tCtrl + S", idFile, saveFile);
	addMenuItem("���Ϊ", idFile, resaveFile);
	addMenuItem("�ر�", idFile, closeFile);
	addMenuItem("ȫ���ر�", idFile, callBack);
	addMenuSeparator(idFile);
	addMenuItem("�˳�\tF4", idFile, exitEditor);

	addMenuItem("����\tCtrl + Z", idEdit, callBack);
	addMenuItem("����\tCtrl + Y", idEdit, callBack);
	addMenuSeparator(idEdit);
	addMenuItem("����\tCtrl + C", idEdit, callBack);
	addMenuItem("����\tCtrl + X", idEdit, callBack);
	addMenuItem("ճ��\tCtrl + V", idEdit, callBack);
	addMenuItem("���", idEdit, callBack);
	addMenuSeparator(idEdit);
	addMenuItem("���/ɾ��ע��", idEdit, callBack);
	addMenuItem("��Сдת��", idEdit, callBack);

	addMenuItem("��������", idComp, interpRun);
	addMenuItem("���͵���", idComp, callBack);
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

		if (key == SG_CTRL) {
			ctrl = 1;
			return;
		}
		else if (key == (SG_CTRL | 0X8000)) {
			ctrl = 0;
			return;
		}

		if (key == SG_F4)exitEditor();

		if (ctrl) {

		}
		else {
			editor->key(key);
		}
	}
	if (biosMouse(1).m) {
		mouse = biosMouse(0);
		editor->mouse(mouse);
	}

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
