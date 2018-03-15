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
	alertInfo("现在还没有完成第一版哟~再等等吧", "版本",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}
void authorInfo() {
	alertInfo("作者：大狱传奇", "作者",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}
void companyInfo() {
	alertInfo("浙江大学玉泉校区32舍420", "公司",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}
void editroInst() {
	alertInfo("适用SGL、SGS、RWE等项目的编辑器，没什么新特性", "编辑器简介",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}
void userInst() {
	alertInfo("与TXT用法一致", "使用方法",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}

void sgSetup() {
	initWindow(640, 480, "Editor", BIT_MAP);
	initKey();
	initMouse(SG_COORDINATE);

	initMenu();
	int idFile = addMenuList("文件", 0);
	int idEdit = addMenuList("编辑", 0);
	int idHelp = addMenuList("帮助", 0);
	addMenuItem("新建\tCtrl + N", idFile, newFile);
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
	addMenuItem("版本", idAbout, versionInfo);
	addMenuItem("作者", idAbout, authorInfo);
	addMenuItem("公司", idAbout, companyInfo);
	int idInst = addMenuList("说明", idHelp);
	addMenuItem("编辑器简介", idInst, editroInst);
	addMenuItem("使用方法", idInst, userInst);

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
