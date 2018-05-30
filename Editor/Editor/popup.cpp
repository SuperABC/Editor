#include "popup.h"
#include "winsgl.h"
#include "editor.h"
#include <algorithm>

extern Editor *editor;

bool probComp(string &a, string &b) {

#ifndef DICT_THREE
	float proba = 1.f, probb = 1.f;
	if (a.length() < 2)return false;
	if (b.length() < 2)return true;
	for (int i = 1; i < (int)a.length(); i++) {
		proba *= (float)editor->ratio[a[i - 1]][a[i]] / 256;
	}
	for (int i = 1; i < (int)b.length(); i++) {
		probb *= (float)editor->ratio[b[i - 1]][b[i]] / 256;
	}
	return proba > probb;
#else
	float proba = 1.f, probb = 1.f;
	if (a.length() < 3)return false;
	if (b.length() < 3)return true;
	for (int i = 2; i < a.length(); i++) {
		proba *= editor->ratio[a[i - 2]][a[i - 1]][a[i]];
	}
	for (int i = 2; i < b.length(); i++) {
		probb *= editor->ratio[b[i - 2]][b[i - 1]][b[i]];
	}
	return proba > probb;
#endif
}

void compListClick(widgetObj *w, int x, int y, int status) {
	mouseClickOption(w, x, y, status);
	if (status == (SG_RIGHT_BUTTON | SG_BUTTON_DOWN))
		w->status = 0;
	if (w->status & WIDGET_SELECTED && 
		(status == SG_MIDDLE_BUTTON_UP || status == SG_MIDDLE_BUTTON_DOWN)) {
		if (biosMouse(1).m)biosMouse(0);
	}
}
void compListKey(widgetObj *w, int key) {
	keyOption(w, key);
	if (key == SG_UP || key == SG_DOWN || key == SG_ENTER) {
		if (biosKey(1))biosKey(0);
	}
}

Popup::Popup() {
	layout();
}
Popup::~Popup() {

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
	option->keyPress = (keyCall)compListKey;
	registerWidget(option);
	free(option);
}
void Popup::add(std::string input) {
	content.push_back(input);
}
void Popup::add(std::vector<std::string> input) {
	for (auto s : input)content.push_back(s);
}
std::string Popup::get(int i) {
	return content[i];
}
void Popup::prepare() {
	std::sort(content.begin(), content.end(), probComp);
	if (content.size() > 32)content = vector<string>(content.begin(), content.begin() + 32);

	widgetObj *option = getWidgetByName("option");
	option->hide = (int)content.size();
	int offset = 0;
	for (int i = 0; i < option->hide; i++) {
		memcpy(option->content + offset, content[i].data(), content[i].length() + 1);
		offset += content[i].length() + 1;
	}
}
void Popup::show(int x, int y) {
	widgetObj *option = getWidgetByName("option");
	if (y + option->hide * 20 + 40 > editor->editorBase.y + editor->editorSize.y)
		y = editor->editorBase.y + editor->editorSize.y - option->hide * 20 - 40;
	if (y < 0)y = 0;
	if(option)mouseClickOption(option, x, y, SG_RIGHT_BUTTON | SG_BUTTON_UP);
	showWidget("option");
	showing = true;
}
void Popup::clear() {
	widgetObj *tmp = getWidgetByName("option");
	if (tmp->status & WIDGET_SELECTED) {
		tmp->status &= 0xFF ^ WIDGET_SELECTED;
		ceaseWidget("option");
	}
	content.clear();
	showing = false;
}
