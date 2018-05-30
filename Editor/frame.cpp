#include <sstream>
#include "Window\grid.h"
#include "Window\prime.h"
#include "Window\navigator.h"
#include "Window\console.h"
#include "Util\json.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

vector<Grid *> grids;
Editor *editor;

/* Menu call. */
void callBack() {
	
}

void newFile() {

}
void openFile() {

}
void explorerDir() {

}
void cmdDir() {

}
void saveFile() {

}
void resaveFile() {

}
void renameFile() {

}
void closeFile() {

}
void closeAll() {

}
void exitEditor() {

}
void undoOperate() {

}
void redoOperate() {

}
void textCopy() {

}
void textCut() {

}
void textPaste() {

}
void textClear() {

}
void textChoose() {

}
void inverseComment() {

}
void inverseCapital() {

}
void interpRun() {

}
void interpDebug() {

}
void interpRev() {

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
	addMenuItem("������", idFile, renameFile);
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

	addMenuItem("����/����\tCtrl + R", idComp, interpRun);
	addMenuItem("����\tCtrl + D", idComp, interpDebug);
	addMenuItem("����", idComp, interpRev);
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
void layoutGrids() {
	std::ifstream fin("layout.ini");
	if (fin.is_open())  {
		string stream, tmp;
		while (!fin.eof()) {
			getline(fin, tmp);
			stream += tmp;
		}

		JSON json((char *)stream.data());
		if (json["enprime"].b)
			grids.push_back(new Prime((int)json["prime"].array[0], (int)json["prime"].array[1],
			(int)json["prime"].array[2], (int)json["prime"].array[3]));
		if (json["ennavigator"].b)
			grids.push_back(new Navigator((int)json["navigator"].array[0], (int)json["navigator"].array[1],
			(int)json["navigator"].array[2], (int)json["navigator"].array[3]));
		if (json["enterminal"].b)
			grids.push_back(new Console((int)json["terminal"].array[0], (int)json["terminal"].array[1],
			(int)json["terminal"].array[2], (int)json["terminal"].array[3]));
	}
}

/* Main frame. */
void sgSetup() {
	initWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Editor", BIT_MAP);
	setResizeable();
	resizeFuntion(windowResize);
	initKey();
	initMouse(SG_COORDINATE);

	layoutMenu();
	layoutGrids();
	editor = new Editor("config.ini");
}
void sgLoop() {
	int key;
	vecThree mouse;

	static int first = 1;
	if (first) {
		first = 0;
		for (auto p : grids)p->showCont();
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

		for(auto p : grids)p->keyEvent(key & ~SG_SHIFTBIT);
	}
	if (biosMouse(1).m) {
		mouse = biosMouse(0);
		for (auto p : grids)p->mouseEvent(mouse.x, mouse.y, mouse.m);
	}

	int status = 0;
	if (mouseStatus(SG_LEFT_BUTTON) == SG_BUTTON_DOWN)
		status |= SG_LEFT_BUTTON;
	if (mouseStatus(SG_MIDDLE_BUTTON) == SG_BUTTON_DOWN)
		status |= SG_MIDDLE_BUTTON;
	if (mouseStatus(SG_RIGHT_BUTTON) == SG_BUTTON_DOWN)
		status |= SG_RIGHT_BUTTON;
	if(status)for (auto p : grids)p->mouseDrag(mousePos().x, mousePos().y, status);
	else for (auto p : grids)p->mouseMove(mousePos().x, mousePos().y);
}
