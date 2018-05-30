#include "lang.h"

char keymap[128] = { 0 };

General::General() {
	keymap['a'] = ';';
	keymap['b'] = '/';
	keymap['c'] = 'm';
	keymap['d'] = 'k';
	keymap['e'] = 'i';
	keymap['f'] = 'j';
	keymap['g'] = 'h';
	keymap['h'] = '\\';
	keymap['n'] = '\'';
	keymap['q'] = 'p';
	keymap['r'] = 'u';
	keymap['t'] = 'y';
	keymap['s'] = 'l';
	keymap['v'] = 'n';
	keymap['w'] = 'o';
	keymap['x'] = ',';
	keymap['y'] = '=';
	keymap['z'] = '.';
	keymap['1'] = '0';
	keymap['2'] = '9';
	keymap['3'] = '8';
	keymap['4'] = '7';
	keymap['5'] = '6';
	keymap['A'] = ':';
	keymap['B'] = '?';
	keymap['C'] = 'M';
	keymap['D'] = 'K';
	keymap['E'] = 'I';
	keymap['F'] = 'J';
	keymap['G'] = 'H';
	keymap['H'] = '|';
	keymap['N'] = '\"';
	keymap['Q'] = 'P';
	keymap['R'] = 'U';
	keymap['S'] = 'L';
	keymap['T'] = 'Y';
	keymap['V'] = 'N';
	keymap['W'] = 'O';
	keymap['X'] = '<';
	keymap['Y'] = '+';
	keymap['Z'] = '>';
	keymap['!'] = ')';
	keymap['@'] = '(';
	keymap['#'] = '*';
	keymap['$'] = '&';
	keymap['%'] = '^';
	keymap['~'] = '_';
}

std::vector<std::string> General::complete(std::string tab) {
	std::vector<std::string>ret = std::vector<std::string>();
	std::vector<std::pair<char, char>> list;

	if (tab.empty() || tab == " ")return ret;

	for (auto c : tab) {
		list.push_back(std::pair<char, char>(c, keymap[c]));
	}
	for (int i = 0; i < pow(2, list.size()); i++) {
		std::string tmp;
		int j;
		for (j = 0; j < (int)list.size(); j++) {
			if ((i >> j) & 1) {
				if (list[j].second == 0)break;
				tmp.push_back(list[j].second);
			}
			else tmp.push_back(list[j].first);
		}
		if (j < (int)list.size())continue;
		ret.push_back(tmp);
	}
	return ret;
}