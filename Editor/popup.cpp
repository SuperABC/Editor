#include "popup.h"
#include "winsgl.h"

void compListClick(widgetObj *w, int x, int y, int status) {
	if (!inWidget(w, x, y)) {
		w->hide = 0;
	}
	mouseClickOption(w, x, y, status);
}

Popup::Popup() {

}
Popup::~Popup() {

}

void Popup::add(std::string input) {
	content.push_back(input);
}
void Popup::add(std::vector<std::string> input) {
	for (auto s : input)content.push_back(s);
}
void Popup::layout() {
	widgetObj *option;
	option = newWidget(SG_OPTION, (SGstring)"option");
	option->size.x = 80;
	option->hide = (int)content.size();
	int offset = 0;
	for (int i = 0; i < option->hide; i++) {
		memcpy(option->content + offset, content[i].data(), content[i].length() + 1);
		offset += content[i].length() + 1;
	}
	option->mouseClick = (mouseClickCall)compListClick;
	registerWidget(option);
	free(option);
}
void Popup::prepare() {
	layout();
}
void  Popup::show(int x, int y) {
	widgetObj *option = getWidgetByName("option");
	mouseClickOption(option, x, y, SG_RIGHT_BUTTON | SG_BUTTON_UP);
}
void Popup::clear() {
	getWidgetByName("option")->hide = 0;
	content.clear();
}
