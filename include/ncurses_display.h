#ifndef NCURSES_DISPLAY_H
#define NCURSES_DISPLAY_H

#include <curses.h>

#include "process.h"
#include "system.h"

namespace NCursesDisplay {
void Display(const System& system, int n = 10);
void DisplaySystem(const System& system, WINDOW* window);
void DisplayProcesses(const std::vector<Process>& processes, WINDOW* window,
                      int n);
std::string ProgressBar(float percent);
};  // namespace NCursesDisplay

#endif