#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>

#define N 9

typedef int Tab2d[N][N];
typedef int Tab1d[N];

#define COLOR_DEFAULT 1
#define COLOR_HIGHLIGHT 2
#define COLOR_TITLE 3
#define COLOR_GRID_BLUE 4
#define COLOR_GRID_RED 5
#define COLOR_NUMBERS 6
#define COLOR_ERROR 7
#define COLOR_SUCCESS 8
#define COLOR_CREDITS 9

void init_colors() {
    start_color();
    init_pair(COLOR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_HIGHLIGHT, COLOR_BLACK, COLOR_YELLOW);
    init_pair(COLOR_TITLE, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_GRID_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_GRID_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_NUMBERS, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_ERROR, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_SUCCESS, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_CREDITS, COLOR_MAGENTA, COLOR_BLACK);
}

bool pas_doublons_vecteur(Tab1d T) {
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (T[i] == T[j] && T[i] != 0)
                return false;
        }
    }
    return true;
}

bool est_ligne_valide(Tab2d T, int ligne) {
    Tab1d V;
    for (int i = 0; i < N; i++)
        V[i] = T[ligne][i];
    return pas_doublons_vecteur(V);
}

bool est_colonne_valide(Tab2d T, int colonne) {
    Tab1d V;
    for (int i = 0; i < N; i++)
        V[i] = T[i][colonne];
    return pas_doublons_vecteur(V);
}

bool est_bloc_valide(Tab2d T, int x, int y) {
    Tab1d V;
    int k = 0;
    for (int i = x; i < x + 3; i++) {
        for (int j = y; j < y + 3; j++) {
            V[k++] = T[i][j];
        }
    }
    return pas_doublons_vecteur(V);
}

bool est_sudoku_valide(Tab2d T) {
    for (int i = 0; i < N; i++) {
        if (!est_ligne_valide(T, i) || !est_colonne_valide(T, i))
            return false;
    }
    for (int i = 0; i < N; i += 3) {
        for (int j = 0; j < N; j += 3) {
            if (!est_bloc_valide(T, i, j))
                return false;
        }
    }
    return true;
}

bool resoudre_sudoku(Tab2d T) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (T[i][j] == 0) {
                for (int num = 1; num <= 9; num++) {
                    T[i][j] = num;
                    if (est_ligne_valide(T, i) && est_colonne_valide(T, j) &&
                        est_bloc_valide(T, (i / 3) * 3, (j / 3) * 3)) {
                        if (resoudre_sudoku(T))
                            return true;
                    }
                    T[i][j] = 0;
                }
                return false;
            }
        }
    }
    return true;
}

void initialiser_zero(Tab2d T) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            T[i][j] = 0;
}

void draw_sudoku_grid(Tab2d T, int cur_x, int cur_y) {
    int start_y = 5;
    int start_x = 10;

    attron(COLOR_PAIR(COLOR_GRID_BLUE));
    mvprintw(start_y, start_x, "+---+---+---+---+---+---+---+---+---+");

    for (int i = 0; i < N; i++) {
        if (i % 3 == 0 && i != 0) {
            attron(COLOR_PAIR(COLOR_GRID_RED));
            mvprintw(start_y + 1 + i*2, start_x, "+---+---+---+---+---+---+---+---+---+");
            attroff(COLOR_PAIR(COLOR_GRID_RED));
        } else if (i != 0) {
            attron(COLOR_PAIR(COLOR_GRID_BLUE));
            mvprintw(start_y + 1 + i*2, start_x, "+---+---+---+---+---+---+---+---+---+");
            attroff(COLOR_PAIR(COLOR_GRID_BLUE));
        }

        for (int j = 0; j < N; j++) {
            if (i == cur_y && j == cur_x) {
                attron(COLOR_PAIR(COLOR_HIGHLIGHT));
                mvprintw(start_y + 2 + i*2, start_x + 2 + j*4, "[%c]",
                        T[i][j] ? T[i][j] + '0' : ' ');
                attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
            } else {
                attron(COLOR_PAIR(COLOR_NUMBERS));
                mvprintw(start_y + 2 + i*2, start_x + 2 + j*4, " %c ",
                        T[i][j] ? T[i][j] + '0' : '.');
                attroff(COLOR_PAIR(COLOR_NUMBERS));
            }
        }

        if (i % 3 == 0) {
            attron(COLOR_PAIR(COLOR_GRID_RED));
        } else {
            attron(COLOR_PAIR(COLOR_GRID_BLUE));
        }

        mvaddch(start_y + 2 + i*2, start_x, '|');
        for (int k = 1; k < N; k++) {
            if (k % 3 == 0) {
                attron(COLOR_PAIR(COLOR_GRID_RED));
            } else {
                attron(COLOR_PAIR(COLOR_GRID_BLUE));
            }
            mvaddch(start_y + 2 + i*2, start_x + k*4, '|');
        }
        attron(COLOR_PAIR(COLOR_GRID_BLUE));
        mvaddch(start_y + 2 + i*2, start_x + N*4, '|');
    }

    attron(COLOR_PAIR(COLOR_GRID_BLUE));
    mvprintw(start_y + 1 + N*2, start_x, "+---+---+---+---+---+---+---+---+---+");
    attroff(COLOR_PAIR(COLOR_GRID_BLUE));
}

void afficher_sudoku_interactif(Tab2d T, int cur_x, int cur_y) {
    clear();
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvprintw(2, 15, "SUDOKU SOLVER");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    draw_sudoku_grid(T, cur_x, cur_y);
    attron(COLOR_PAIR(COLOR_DEFAULT));
    mvprintw(25, 5, "Utilisez les fleches pour naviguer, chiffres 1-9 pour inserer, 0 pour effacer, ESC pour quitter");
    refresh();
}

void remplir_grille_par_curseur(Tab2d T) {
    int x = 0, y = 0;
    int ch;
    afficher_sudoku_interactif(T, x, y);

    while (1) {
        ch = getch();
        switch (ch) {
            case KEY_UP: if (y > 0) y--; break;
            case KEY_DOWN: if (y < 8) y++; break;
            case KEY_LEFT: if (x > 0) x--; break;
            case KEY_RIGHT: if (x < 8) x++; break;
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9':
                T[y][x] = ch - '0'; break;
            case '0':
                T[y][x] = 0; break;
            case 27: return;
        }
        afficher_sudoku_interactif(T, x, y);
    }
}

void show_centered_message(int y, const char* msg, int color_pair) {
    int x = (COLS - strlen(msg)) / 2;
    attron(COLOR_PAIR(color_pair));
    mvprintw(y, x, "%s", msg);
    attroff(COLOR_PAIR(color_pair));
}

void menu(Tab2d sudoku) {
    int choix = 0;
    bool quitter = false;
    const char* options[] = {
        "1. Entrer une grille",
        "2. Afficher la grille",
        "3. Resoudre le Sudoku",
        "4. Reinitialiser la grille",
        "5. Vider le Sudoku",
        "6. Quitter"
    };
    const int nb_options = 6;

    while (!quitter) {
        clear();
        attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
        show_centered_message(2, "MENU PRINCIPAL", COLOR_TITLE);
        attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
        for (int i = 0; i < nb_options; i++) {
            int y = 6 + i;
            int x = (COLS - strlen(options[i])) / 2;
            if (i == choix) {
                attron(COLOR_PAIR(COLOR_HIGHLIGHT));
                mvprintw(y, x-2, "> %s <", options[i]);
                attroff(COLOR_PAIR(COLOR_HIGHLIGHT));
            } else {
                attron(COLOR_PAIR(COLOR_DEFAULT));
                mvprintw(y, x, "%s", options[i]);
                attroff(COLOR_PAIR(COLOR_DEFAULT));
            }
        }
        attron(COLOR_PAIR(COLOR_DEFAULT));
        show_centered_message(15, "Utilisez les fleches pour selectionner et Entree pour valider", COLOR_DEFAULT);

        int ch = getch();
        switch (ch) {
            case KEY_UP: if (choix > 0) choix--; break;
            case KEY_DOWN: if (choix < nb_options-1) choix++; break;
            case 10:
                switch (choix) {
                    case 0:
                        remplir_grille_par_curseur(sudoku); break;
                    case 1:
                        afficher_sudoku_interactif(sudoku, -1, -1);
                        getch(); break;
                    case 2:
                        if (!est_sudoku_valide(sudoku)) {
                            show_centered_message(20, "Grille initiale invalide - des doublons sont presents!", COLOR_ERROR);
                            refresh(); sleep(2);
                        } else if (resoudre_sudoku(sudoku)) {
                            show_centered_message(20, "Sudoku resolu avec succes!", COLOR_SUCCESS);
                            refresh(); sleep(1);
                            afficher_sudoku_interactif(sudoku, -1, -1);
                            getch();
                        } else {
                            show_centered_message(20, "Aucune solution trouvee pour cette grille.", COLOR_ERROR);
                            refresh(); sleep(2);
                        }
                        break;
                    case 3:
                        initialiser_zero(sudoku);
                        show_centered_message(20, "Grille reinitialisee.", COLOR_SUCCESS);
                        refresh(); sleep(1); break;
                    case 4:
                        initialiser_zero(sudoku);
                        show_centered_message(20, "Sudoku vide avec succes!", COLOR_SUCCESS);
                        refresh(); sleep(1); break;
                    case 5:
                        quitter = true; break;
                }
                break;
        }
    }

    clear();
    attron(COLOR_PAIR(COLOR_CREDITS) | A_BOLD);
    show_centered_message(LINES/2 - 2, "SUDOKU SOLVER", COLOR_CREDITS);
    show_centered_message(LINES/2, "Realise par :", COLOR_CREDITS);
    show_centered_message(LINES/2 + 1, "TOUIL ABDEREZAK", COLOR_CREDITS);
    show_centered_message(LINES/2 + 2, "Etudiant 2CP ESI Alger", COLOR_CREDITS);
    attroff(COLOR_PAIR(COLOR_CREDITS) | A_BOLD);
    refresh();
    sleep(3);
}

int main() {
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    init_colors();
    Tab2d sudoku;
    initialiser_zero(sudoku);
    menu(sudoku);
    endwin();
    return 0;
}
