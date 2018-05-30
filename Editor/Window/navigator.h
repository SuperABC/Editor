#pragma once
#include "Window/grid.h"

class Navigator :public Grid {
private:
	class Node {
	public:
		string name;
		vector<Node *> childs;

		Node(string n) :name(n) {}
		void push(Node *child) { childs.push_back(child); }
	} *root;

public:
	Navigator(int x, int y, int w, int h) : Grid("navigator", x, y, w, h), root(new Node("root")) {}

	virtual void moveGrid() {}
	virtual void showCont() {}
	virtual void keyEvent(int key) {}
	virtual void mouseEvent(int x, int y, int status) {}
	virtual void mouseMove(int x, int y) {}
	virtual void mouseDrag(int x, int y, int button) {}
	virtual void idelLoop() {}
};