#pragma once
#include <vector>

class Popup {
private:
	std::vector<std::string>content;

	void layout();

public:
	Popup();
	~Popup();

	void add(std::string input);
	void add(std::vector<std::string> input);
	void prepare();
	void show(int x, int y);
	void clear();
};