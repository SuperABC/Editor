#include "editor.h"

Editor::Editor() {
	content = NULL;
}
Editor::~Editor() {

}

void Editor::open(char *filename) {
	if (content == NULL) {
		content = new File(filename);
	}
	else {
		File *tmp = content;
		while (tmp->next) {
			tmp = tmp->next;
		}
		tmp->next = new File(filename);
	}
}

void Editor::key(int key) {

}
void Editor::mouse(vecThree mouse) {

}