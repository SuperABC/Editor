#pragma once
#include <iostream>
#include "winsgl.h"

class Line {
private:
	char content;
	Line *next;

	friend class Block;
public:
	Line(char c) :content(c), next(NULL) {}
	
	char get();

	void append(char c);
	void change(int pos, char c);
	void remove(int pos);
};