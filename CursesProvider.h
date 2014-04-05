#include <iostream>
#include <curses.h>

using namespace std;

#ifndef _CURSES_H
#define _CURSES_H

class CursesProvider{
        public:
                CursesProvider();
                void create_menu(const map<string, string> *labels); 
        private:
                void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);
};

#endif
