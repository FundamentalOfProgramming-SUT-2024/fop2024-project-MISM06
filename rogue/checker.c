#include <ncurses.h>
#include <locale.h>
#include <string.h>

int main() {
    // Enable Unicode support
    setlocale(LC_ALL, "");

    initscr();           // Initialize ncurses
    noecho();            // Disable echo
    curs_set(FALSE);     // Hide the cursor

    // Draw a middle dot
    // Unicode middle dot (U+00B7)

    char *ch = "";
    printw("%s", ch);
    int x = strlen(ch);
    mvprintw(10, 10, "%d", x);

    refresh();           // Refresh the screen
    getch();             // Wait for input
    endwin();            // End ncurses mode

    return 0;
}
