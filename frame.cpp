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
	int idFile = addMenuList("文件", 0);
	int idEdit = addMenuList("编辑", 0);
	int idHelp = addMenuList("帮助", 0);
	addMenuItem("打开\tCtrl + O", idFile, openFile);
	addMenuItem("保存\tCtrl + S", idFile, saveFile);
	addMenuItem("关闭", idFile, closeFile);
	addMenuItem("退出\tF4", idFile, exitEditor);
	addMenuItem("撤销\tCtrl + Z", idEdit, callBack);
	addMenuItem("重做\tCtrl + Y", idEdit, callBack);
	addMenuSeparator(idEdit);
	addMenuItem("复制\tCtrl + C", idEdit, callBack);
	addMenuItem("剪切\tCtrl + X", idEdit, callBack);
	addMenuItem("粘贴\tCtrl + V", idEdit, callBack);
	addMenuItem("清除", idEdit, callBack);
	int idAbout = addMenuList("关于", idHelp);
	addMenuItem("版本", idAbout, callBack);
	addMenuItem("作者", idAbout, callBack);
	addMenuItem("公司", idAbout, callBack);
	int idInst = addMenuList("说明", idHelp);
	addMenuItem("编辑器简介", idInst, callBack);
	addMenuItem("使用方法", idInst, callBack);

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
