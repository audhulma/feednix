#include <iostream>
#include <curses.h>

using namespace std;

class CursesProvider{
        public:
                CursesProvider();
                void create_menu(); 
        private:
                void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);
};
