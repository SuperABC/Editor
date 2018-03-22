#pragma once
#include <vector>

class Popup {
private:
	std::vector<std::string>content;

public:
	Popup();
	~Popup();

	void layout();
	void clear();
	void add(std::string input);
	void add(std::vector<std::string>);
	void prepare();
	void show(int x, int y);
	void cease();
};