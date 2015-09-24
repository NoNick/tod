#pragma once
#include <iostream>

// class for displayable in terminal objects
class Widget {
public:
    // draws a line with given length
    virtual void draw(unsigned width) {
	std::cout << "Oops...\n";
    };
    virtual ~Widget() {};
};
