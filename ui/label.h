#pragma once
#include <string>
#include "widget.h"

// Printable text string
// Either cut last characters from string or adds spaces
// if width doesn't match string length
class Label : public Widget {
public:
    Label(std::string str) : text(str) {};
    void draw(unsigned width);
    void setText(std::string str);
private:
    std::string text;
};
