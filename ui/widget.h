#pragma once
#include <iostream>
#include "screen.h"

// class for displayable in terminal objects
// requires screen for callbacks
class Widget {
public:
    Widget(Screen *screen) : screen(screen) {};
    // draws a line with given length
    virtual void draw(unsigned width) {
	std::cout << "Oops...\n";
    };
    virtual ~Widget() {};
protected:
    Screen *screen;
};
