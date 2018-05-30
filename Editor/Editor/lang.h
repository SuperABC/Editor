#pragma once
#include <vector>
class Language {
public:
	Language() {}
	virtual ~Language() {}

	virtual Language * input(const char *str) = 0;
	virtual std::vector<std::pair<int, std::pair<int, int>>> highlight() = 0;
	virtual std::vector<std::string> complete(std::string tab) = 0;
};

class Debugger {
public:
	Debugger() {}
	virtual ~Debugger() {}

	virtual int line() = 0;
	virtual void show() = 0;
	virtual int step() = 0;
	virtual int process() = 0;
	virtual void stop() = 0;
};

class General : public Language {
private:
	std::string line;
public:
	General();
	virtual ~General() {}

	Language * input(const char *str) {
		line = str;
		return this;
	}
	std::vector<std::pair<int, std::pair<int, int>>> highlight() {
		std::vector<std::pair<int, std::pair<int, int>>> tmp;
		tmp.push_back(std::pair<int, std::pair<int, int>>(0, std::pair<int, int>(0, line.length())));
		return tmp;
	}
	std::vector<std::string> complete(std::string tab);
};