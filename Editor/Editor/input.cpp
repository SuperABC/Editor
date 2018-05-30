#include "input.h"

void(*confirm)(std::string content);
void boxClick(widgetObj *w, int x, int y, int status) {
	mouseClickInput(w, x, y, status);
	if (w->status & WIDGET_PASS) {
		while (biosMouse(1).m)biosMouse(0);
	}
}
void okClick(widgetObj *w, int x, int y, int status) {
	mouseClickDefault(w, x, y, status);
	if (w->status & WIDGET_SELECTED) {
		while (biosMouse(1).m)biosMouse(0);
		widgetObj *text = getWidgetByName("input")->next;
		confirm((char *)text->content);
	}
}
void okTake(widgetObj *w, int x, int y, int status) {
	mouseClickDefault(w, x, y, status);
	if (w->status & WIDGET_PASS) {
		while (biosMouse(1).m)biosMouse(0);
	}
}
void boxKey(widgetObj *w, int code) {
	keyInput(w, code);
	while (biosKey(1))biosKey(0);
}

Input::Input(void(*confirm)(std::string content),
	vecTwo inputPos, vecTwo inputSize, vecTwo okSize) {
	box = newWidget(SG_INPUT, (SGstring)"box");
	ok = newWidget(SG_BUTTON, (SGstring)"ok");

	box->size = inputSize;
	box->keyPress = (keyCall)boxKey;
	box->mouseClick = box->mouseDown = box->mouseUp = (mouseClickCall)boxClick;
	ok->pos.x = inputSize.x;
	ok->size = okSize;
	strcpy((char *)ok->content, "х╥хо");
	ok->mouseClick = (mouseClickCall)okClick;
	ok->mouseDown = ok->mouseUp = (mouseClickCall)okTake;
	::confirm = confirm;

	combined = newCombinedWidget(2, (SGstring)"input", box, ok);
	combined->pos = inputPos;
	combined->size.x = box->size.x + ok->size.x;
	combined->size.y = box->size.y;

	registerWidget(combined);
}
Input::~Input() {

}