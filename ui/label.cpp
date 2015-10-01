#include <iostream>
#include "label.h"

void Label::draw(unsigned width) {
    unsigned i;
    for (i = 0; i < std::min(width, (unsigned)text.length()); i++) {
	std::cout << text[i];
    }
    for (; i < width; i++) {
	std::cout << ' ';
    }
}

void Label::setText(std::string str) {
    text = str;
    screen->refresh();
}
