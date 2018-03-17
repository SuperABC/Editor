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
void editorInst() {
	alertInfo("适用SGL、SGS、RWE等项目的编辑器，没什么新特性", "编辑器简介",
		ALERT_ICON_INFORMATION | ALERT_BUTTON_OK);
}
void userInst() {
	alertInfo("与TXT用法一致", "使用方法",
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

	int idFile = addMenuList("文件", 0);
	int idEdit = addMenuList("编辑", 0);
	int idComp = addMenuList("编译器", 0);
	int idTool = addMenuList("工具", 0);
	int idHelp = addMenuList("帮助", 0);

	addMenuItem("新建\tCtrl + N", idFile, newFile);
	addMenuItem("打开\tCtrl + O", idFile, openFile);
	int idDir = addMenuList("打开文件夹", idFile);
	addMenuItem("资源管理器", idDir, callBack);
	addMenuItem("命令行", idDir, callBack);
	addMenuItem("保存\tCtrl + S", idFile, saveFile);
	addMenuItem("另存为", idFile, resaveFile);
	addMenuItem("关闭", idFile, closeFile);
	addMenuItem("全部关闭", idFile, callBack);
	addMenuSeparator(idFile);
	addMenuItem("退出\tF4", idFile, exitEditor);

	addMenuItem("撤销\tCtrl + Z", idEdit, callBack);
	addMenuItem("重做\tCtrl + Y", idEdit, callBack);
	addMenuSeparator(idEdit);
	addMenuItem("复制\tCtrl + C", idEdit, callBack);
	addMenuItem("剪切\tCtrl + X", idEdit, callBack);
	addMenuItem("粘贴\tCtrl + V", idEdit, callBack);
	addMenuItem("清除", idEdit, callBack);
	addMenuSeparator(idEdit);
	addMenuItem("添加/删除注释", idEdit, callBack);
	addMenuItem("大小写转换", idEdit, callBack);

	addMenuItem("解释运行", idComp, interpRun);
	addMenuItem("解释调试", idComp, callBack);
	addMenuSeparator(idComp);
	addMenuItem("编译器设置", idComp, callBack);

	int idOption = addMenuList("选项", idTool);
	addMenuItem("显示设置", idOption, callBack);
	addMenuItem("启动设置", idOption, callBack);
	addMenuItem("操作设置", idOption, callBack);
	int idPlugin = addMenuList("插件", idTool);
	addMenuItem("管理器", idPlugin, callBack);

	int idAbout = addMenuList("关于", idHelp);
	addMenuItem("版本", idAbout, versionInfo);
	addMenuItem("作者", idAbout, authorInfo);
	addMenuItem("公司", idAbout, companyInfo);
	int idInst = addMenuList("说明", idHelp);
	addMenuItem("编辑器简介", idInst, editorInst);
	addMenuItem("使用方法", idInst, userInst);

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
