#pragma once
#include <vector>
#include "widget.h"

// displays widgets
// deletes them in destructor
class Screen {
public:
    // empty layout, takes terminal line's width using env vars
    Screen();
    ~Screen();
    // draws widgets over previous widget's text
    void refresh();
    // adds widget and reserve width percents of terminal line for it
    void addWidget(Widget *widget, unsigned width);
    // adds line break, i.e. new widgets will be placed at a new line
    void lineBreak();
private:
    unsigned width;
    std::vector < std::vector < std::pair <Widget*, unsigned> > > layout;
    bool first; // shoudn't do left and up at first refresh
};
