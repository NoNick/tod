#pragma once
#include <vector>
#include <deque>
#include "widget.h"

// displays last n written lines, no history
class TextArea : public Widget {
public:
    TextArea(unsigned width, unsigned height);
    void draw(unsigned width);
    void putLn(std::string line);
private:
    unsigned width, height;
    std::deque <std::string> text;
};
