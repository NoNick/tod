#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include "screen.h"
#include "widget.h"

Screen::Screen() : first(true) {
    winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    width = w.ws_col;
    height = w.ws_row;
    layout.push_back(std::vector <std::pair <Widget*, unsigned> > ());
    std::cout << "\033c";
}

Screen::~Screen() {
    for (auto it1 = layout.begin(); it1 != layout.end(); it1++) {
	for (auto it2 = (*it1).begin(); it2 != (*it1).end(); it2++) {
	    delete (*it2).first;
	}
    }
}

void Screen::refresh() {
    if (!visible) {
	return;
    }
    
    if (!first) {
	std::cout << '\r';
	std::cout << "\033[" << layout.size() << "A";
    } else {
	first = false;
    }
    for (auto it1 = layout.begin(); it1 != layout.end(); it1++) {
	for (auto it2 = (*it1).begin(); it2 != (*it1).end(); it2++) {
	    (*it2).first->draw((*it2).second * width / 100);
	}
	if (it1 + 1 != layout.end()) {
	    std::cout << "\n";
	}
    }
}

void Screen::addWidget(Widget *widget, unsigned width) {
    layout.back().push_back(std::make_pair(widget, width));
}

void Screen::lineBreak() {
    layout.push_back(std::vector <std::pair <Widget*, unsigned> > ());
}

std::pair <unsigned, unsigned> Screen::getSize() {
    return std::make_pair(width, height);
}

void Screen::setVisible(bool v) {
    if (!visible && v) {
	refresh();
    }
    visible = v;
}
