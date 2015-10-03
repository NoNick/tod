#include <iostream>
#include "text_area.h"

#define ceilInt(x, y) (x % y ? x / y + 1 : x / y)

void TextArea::putLn(std::string str) {
    int len = str.length();
    for (unsigned i = 0; i < ceilInt(len, width); i++) {
	text.push_back(str.substr(i * width, width));
	if (text.size() > height) {
	    text.pop_front();
	}
    }
    screen->refresh();
}

void TextArea::draw(unsigned width) {
    std::cout << "\r\033[" << height << "A";
    for (std::deque<std::string>::iterator it = text.begin(); it != text.end(); it++) {
	std::cout << "\033[2K" << *it;
	if (it + 1 != text.end()) {
	    std::cout << "\n";
	}
    }
    if (height - text.size() > 1) {
	for (unsigned i = 0; i < height - text.size() - 1; i++) {
	    std::cout << "\n";
	}
    }
}
