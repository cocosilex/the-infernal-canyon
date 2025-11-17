#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

unsigned width = 30;
unsigned speed = 5000;
bool running = true;

void init(void);
void game(void);
void initialize_borders(unsigned *borders);
void update_borders(unsigned *borders);
void moov_cursor(int key, unsigned *x_pos, unsigned *borders);

int main() {
    setlocale(LC_ALL, "");
    init();
    game();
    return 0;
}

void init(void) {
    initscr(); 
    noecho();
    curs_set(0);
    timeout(0);
    start_color();
    use_default_colors();

    // refer to terminal colors
    init_pair(1, 2, -1);
    init_pair(2, 1, -1);
    init_pair(3, 6, -1);
    init_pair(4, 52, -1);

    width = COLS/5;
    srand(time(NULL));
}

void game(void) {
    unsigned long score = 0;

    unsigned x_pos = COLS/2;
    attron(COLOR_PAIR(1));
    mvaddch(LINES - 1, x_pos, '^');
    attroff(COLOR_PAIR(1));
    
    unsigned borders[LINES];
    initialize_borders(borders);

    refresh();

    while(running) {
        score++;
        speed++;
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(0, 0, "Score : %lu", score);
        mvprintw(1, 0, "Speed : %u mps", speed/100);
        attroff(COLOR_PAIR(3) | A_BOLD);
        
        update_borders(borders);

        if(x_pos <= borders[LINES - 1] || x_pos >= borders[LINES - 1] + width) {
            attron(A_BOLD | COLOR_PAIR(4));
            mvaddch(LINES - 1, x_pos, 'X');
            attroff(A_BOLD | COLOR_PAIR(4));

            attron(A_BOLD | COLOR_PAIR(2));
            mvprintw(LINES/2, COLS/2, "Game Over!");
            mvprintw(LINES/2 + 1, COLS/2, "Score : %lu", score);
            attroff(A_BOLD | COLOR_PAIR(2));

            attron(A_ITALIC | COLOR_PAIR(2));
            mvprintw(LINES/2 + 2, COLS/2 - 8, "Press p to close the game.");
            attroff(A_ITALIC | COLOR_PAIR(2));
            running = false;
            break;
        }

        int key = getch();
        moov_cursor(key, &x_pos, borders);

        refresh();
        napms(100000/speed);
    }

    timeout(-1);
    int input = getch();
    while(input != 'p') {
        input = getch();
        refresh();
    }

    endwin();
}

void initialize_borders(unsigned *borders) {
    attron(COLOR_PAIR(2));
    for(unsigned long i = 0; i < (unsigned)LINES; i++) {
        borders[i] = COLS/2 - width/2;

        mvaddch(i, COLS/2 - width/2, ACS_BLOCK);
        mvaddch(i, COLS/2 + width/2, ACS_BLOCK);
    }
    attroff(COLOR_PAIR(2));
}

void update_borders(unsigned *borders) {
    for(int i = 0; i < LINES; i++) {
         mvaddch(i, borders[i], ' ');
        mvaddch(i, borders[i] + width, ' ');
    }

    for(int i = LINES - 1; i > 0; i--) {
        borders[i] = borders[i - 1];
    }

    int rdn = rand() % 3;
    if(rdn == 1 && borders[0] + width < (unsigned)(COLS - 1)) {
        borders[0]++;
    } else if(rdn == 2 && borders[0] > 0) {
        borders[0]--;
    }

    attron(COLOR_PAIR(2));
    for(int i = 0; i < LINES; i++) {
        mvaddch(i, borders[i], ACS_BLOCK);
        mvaddch(i, borders[i] + width, ACS_BLOCK);
    }
    attroff(COLOR_PAIR(2));
}

void moov_cursor(int key, unsigned *x_pos, unsigned *borders) {
    if((key == 'q' || key == 'Q') && *x_pos - 1 > borders[LINES - 1]) {
            mvaddch(LINES - 1, *x_pos, ' ');
            (*x_pos)--;
            attron(COLOR_PAIR(1));
            mvaddch(LINES - 1, *x_pos, '^');
            attroff(COLOR_PAIR(1));
        } else if((key == 'd' || key == 'D') && *x_pos + 1 < borders[LINES - 1] + width) {
            mvaddch(LINES - 1, *x_pos, ' ');
            (*x_pos)++;
            attron(COLOR_PAIR(1));
            mvaddch(LINES - 1, *x_pos, '^');
            attroff(COLOR_PAIR(1));
        }
}