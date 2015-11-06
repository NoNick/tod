#pragma once
#include <vector>
#include <deque>
#include "widget.h"

#define ERR(STR) TextArea::print(STR)

#define LOG(STR) if (TextArea::mode == VERBOSE || \
		     TextArea::mode == DEBUG) { \
                     TextArea::print(STR); }
#define DEBUG(STR) if (TextArea::mode == DEBUG) {	\
                     TextArea::print(STR); }

enum OutMode {
    SILENT, VERBOSE, DEBUG
};

// displays last n written lines, no history
class TextArea : public Widget {
public:
    TextArea(Screen *s, unsigned width, unsigned height);
    void draw(unsigned width);
    // puts string in specified instance
    void putLn(std::string line);
    // puts string in last instantinated TextArea
    static void print(std::string line);
    static OutMode mode;
private:
    unsigned width, height;
    std::deque <std::string> text;

    static TextArea *last;
};
