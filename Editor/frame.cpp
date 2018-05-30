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
	addMenuItem("资源管理器", idDir, explorerDir);
	addMenuItem("命令行", idDir, cmdDir);
	addMenuItem("保存\tCtrl + S", idFile, saveFile);
	addMenuItem("另存为", idFile, resaveFile);
	addMenuItem("重命名", idFile, renameFile);
	addMenuItem("关闭", idFile, closeFile);
	addMenuItem("全部关闭", idFile, closeAll);
	addMenuSeparator(idFile);
	addMenuItem("退出\tF4", idFile, exitEditor);

	addMenuItem("撤销\tCtrl + Z", idEdit, undoOperate);
	addMenuItem("重做\tCtrl + Y", idEdit, redoOperate);
	addMenuItem("操作记录", idEdit, callBack);
	addMenuSeparator(idEdit);
	addMenuItem("复制\tCtrl + C", idEdit, textCopy);
	addMenuItem("剪切\tCtrl + X", idEdit, textCut);
	addMenuItem("粘贴\tCtrl + V", idEdit, textPaste);
	addMenuItem("清除", idEdit, textClear);
	addMenuSeparator(idEdit);
	addMenuItem("添加/删除注释", idEdit, inverseComment);
	addMenuItem("大小写转换", idEdit, inverseCapital);

	addMenuItem("解释/编译\tCtrl + R", idComp, interpRun);
	addMenuItem("调试\tCtrl + D", idComp, interpDebug);
	addMenuItem("逆向", idComp, interpRev);
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
