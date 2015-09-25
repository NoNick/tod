#pragma once
#include <vector>
#include <deque>
#include "widget.h"

// displays last n written lines, no history
class TextArea : public Widget {
public:
    TextArea(Screen *s, unsigned width, unsigned height) : Widget(s), width(width), height(height) {};
    void draw(unsigned width);
    void putLn(std::string line);
private:
    unsigned width, height;
    std::deque <std::string> text;
};
