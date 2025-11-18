#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

unsigned width = 30;
unsigned speed = 5000;
bool continue_to_play = true;


void init(void);
void start_menu(void);
bool game(void);
void check_cheat(void);
void initialize_borders(unsigned *borders);
void update_borders(unsigned *borders, unsigned score);
void moov_cursor(int key, unsigned *x_pos, unsigned *borders);
void death(unsigned x_pos, unsigned long score);

int main() {
    setlocale(LC_ALL, "");
    init();

    start_menu();
    while (continue_to_play) {
        bool will_to_continue = game();
        if(!will_to_continue) {
            continue_to_play = false;
        } else {
            erase();
            speed = 5000;
            width = COLS/5;
        }    
    }

    endwin();
    return 0;
}

void init(void) {
    initscr(); 
    noecho();
    curs_set(0);
    start_color();
    use_default_colors();

    check_cheat();

    // refer to terminal colors
    init_pair(1, 2, -1);
    init_pair(2, 1, -1);
    init_pair(3, 6, -1);
    init_pair(4, 52, -1);
    init_pair(5, 43, -1);

    width = COLS/5;
    srand(time(NULL));
}

void start_menu(void) {
    timeout(-1);

    attron(A_BOLD | A_UNDERLINE | COLOR_PAIR(5));
    mvprintw(LINES/10, COLS/2 - 4, "The Infernal Canyon");
    attroff(A_BOLD | A_UNDERLINE | COLOR_PAIR(5));

    attron(A_ITALIC| COLOR_PAIR(5));
    mvprintw(LINES/10 + 1, COLS/2 - 1, "by @cocosilex");
    attroff(A_ITALIC | COLOR_PAIR(5));

    attron(A_ITALIC| COLOR_PAIR(5));
    mvprintw(LINES/2 - LINES/10 - 1, COLS/2 - 8, "Informations about the game :");
    mvprintw(LINES/2 - LINES/10, COLS/2 - 30, "Keybinds system : press 'd' to move to the right and 'q' to move to the left.");
    mvprintw(LINES/2 - LINES/10 + 1, COLS/2 - 35, "The speed of the game is progressive, starting at 50 mps or 'move per second' of the grid.");
    mvprintw(LINES/2 - LINES/10 + 2, COLS/2 - 13, "The score is incremented by one at each move of the grid.");
    mvprintw(LINES/2 - LINES/10 + 4, COLS/2 - 7, "Press 'r' to start playing");
    attroff(A_ITALIC | COLOR_PAIR(5));

    int key = getch();
    while(key != 'r' && key != 'R') {
        key = getch();
    }

    erase();
}

bool game(void) {
    check_cheat();
    timeout(0);
    unsigned long score = 0;

    unsigned x_pos = COLS/2;
    attron(COLOR_PAIR(1));
    mvaddch(LINES - 1, x_pos, '^');
    attroff(COLOR_PAIR(1));
    
    unsigned borders[LINES];
    initialize_borders(borders);

    refresh();

    while(true) {
        score++;
        speed++;
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(0, 0, "Score : %lu", score);
        mvprintw(1, 0, "Speed : %u mps", speed/100);
        attroff(COLOR_PAIR(3) | A_BOLD);
        
        update_borders(borders, score);

        if(x_pos <= borders[LINES - 1] || x_pos >= borders[LINES - 1] + width) {
            death(x_pos, score);
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
        if(input == 'r' || input == 'R') {
            return true;
        }
        input = getch();
    }

    return false;
}

void initialize_borders(unsigned *borders) {
    borders[LINES - 1] = COLS/2 - width/2;

    for(int i = LINES - 2; i >= 0; i--) {
        borders[i] = borders[i + 1]; 

        int rdn = rand() % 3;
        if(rdn == 1 && borders[i] + width < (unsigned)(COLS - 1)) {
            borders[i]++;
        } else if(rdn == 2 && borders[i] > 0) {
            borders[i]--;
        }
    }

    attron(COLOR_PAIR(2));
    for(int i = 0; i < LINES; i++) {
        mvaddch(i, borders[i], ACS_BLOCK);
        mvaddch(i, borders[i] + width, ACS_BLOCK);
    }
    attroff(COLOR_PAIR(2));
}

void update_borders(unsigned *borders, unsigned score) {
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

    if(COLS/5 - score/100 > (unsigned)COLS/6) {
        width = COLS/5 - score/500;
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

void check_cheat(void) {
    if(COLS > 190) {
        endwin();
        printf("Stop trying to cheat!\n");
        printf("To play, please zoom in using ctrl + <3\n");
        printf("Currently trying to display %d/190 columns.\n", COLS);
        exit(0);
    } else if(COLS < 110) {
        endwin();
        printf("To play, please zoom out using ctrl - <3\n");
        printf("Currently trying to display %d/110 minimum columns (max is 190).\n", COLS);
        exit(0);
    }
}

void death(unsigned x_pos, unsigned long score) {
    attron(A_BOLD | COLOR_PAIR(4));
    mvaddch(LINES - 1, x_pos, 'X');
    attroff(A_BOLD | COLOR_PAIR(4));

    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(LINES/2, COLS/2, "Game Over!");
    mvprintw(LINES/2 + 1, COLS/2, "Score : %lu", score);
    attroff(A_BOLD | COLOR_PAIR(2));

    attron(A_ITALIC | COLOR_PAIR(2));
    mvprintw(LINES/2 + 2, COLS/2 - 16, "Press 'p' to close the game or 'r' to restart");
    attroff(A_ITALIC | COLOR_PAIR(2));
}