#pragma once
#include "winsgl.h"

class Input {
private:
	widgetObj *box, *ok, *combined;
public:

	Input(void (*confirm)(std::string content),
		vecTwo inputPos, vecTwo inputSize, vecTwo okSize);
	~Input();
};