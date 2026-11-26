#include <curses.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

const int   LEBAR           = 40;
const int   TINGGI          = 20;
const int   JUMLAH_PIPA     = 3;
const int   LEBAR_PIPA      = 4;
const int   TINGGI_CELAH    = 7;
const int   JARAK_PIPA      = 16;
const int   BURUNG_X        = 8; 
const float GRAVITASI       = 0.35f;
const float KEKUATAN_LOMPAT = -1.5f;
const int   KECEPATAN       = 80;

enum Keadaan { MENU, BERMAIN, SELESAI };


int   pipaX[JUMLAH_PIPA];
int   celahAtas[JUMLAH_PIPA];
bool  sudahLewat[JUMLAH_PIPA];
float burungY;
float kecepatanBurung;
int   skor = 0;
int   skorTertinggi = 0;

int acakCelah() {
    return 2 + rand() % (TINGGI - TINGGI_CELAH - 3);
}

void mulaiUlang() {
    burungY = TINGGI / 2;
    kecepatanBurung = KEKUATAN_LOMPAT;
    skor = 0;
    for (int i = 0; i < JUMLAH_PIPA; i++) {
        pipaX[i] = LEBAR + 6 + i * JARAK_PIPA;
        celahAtas[i] = acakCelah();
        sudahLewat[i] = false;
    }
}

void tulisTengah(int baris, const char* teks) {
    int panjang = 0;
    while (teks[panjang] != '\0') panjang++;
    mvprintw(baris, (COLS - panjang) / 2, "%s", teks);
}

void gambarPermainan() {
    int geserX = (COLS - LEBAR) / 2;
    int geserY = (LINES - TINGGI - 2) / 2;
    int barisBurung = (int)burungY;

    erase();
    mvprintw(geserY, geserX, "Skor: %d   Tertinggi: %d", skor, skorTertinggi);

    for (int baris = 0; baris < TINGGI; baris++) {
        for (int kolom = 0; kolom < LEBAR; kolom++) {
            char huruf = ' ';
            chtype warna = A_NORMAL;

            // pipa (awas nabrak)
            for (int i = 0; i < JUMLAH_PIPA; i++) {
                bool didalamPipa = kolom >= pipaX[i] && kolom < pipaX[i] + LEBAR_PIPA;
                bool diluarCelah = baris < celahAtas[i] || baris >= celahAtas[i] + TINGGI_CELAH;
                if (didalamPipa && diluarCelah && baris < TINGGI - 1) {
                    huruf = ' ';
                    warna = COLOR_PAIR(2) | A_REVERSE;   // blok hijau
                }
            }

            // tanah (dirt)
            if (baris == TINGGI - 1) {
                huruf = '=';
                warna = COLOR_PAIR(1);
            }

            // buwung puyuh
            if (baris == barisBurung && kolom == BURUNG_X) {
                huruf = 'O';
                warna = COLOR_PAIR(1) | A_BOLD;
            }
            if (baris == barisBurung && kolom == BURUNG_X + 1) {
                huruf = '>';
                warna = COLOR_PAIR(1) | A_BOLD;
            }

            attron(warna);
            mvaddch(geserY + 1 + baris, geserX + kolom, huruf);
            attroff(warna);
        }
    }
}

int main() {
    srand((unsigned)time(NULL));

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_GREEN,  COLOR_BLACK);
        init_pair(3, COLOR_RED,    COLOR_BLACK);
    }

    if (LINES < TINGGI + 4 || COLS < LEBAR + 4) {
        endwin();
        cout << "Jendela terminal terlalu kecil (" << COLS << "x" << LINES << ")." << endl;
        cout << "Perbesar jendela minimal " << LEBAR + 4 << "x" << TINGGI + 4
             << " lalu jalankan lagi." << endl;
        return 1;
    }

    Keadaan keadaan = MENU;
    mulaiUlang();

    while (true) {
        int tombol = getch();
        flushinp();
        if (tombol == 'q' || tombol == 'Q') break;

        if (keadaan == MENU) {
            if (tombol == ' ') {
                mulaiUlang();
                keadaan = BERMAIN;
            }
            erase();
            attron(COLOR_PAIR(1) | A_BOLD);
            tulisTengah(LINES / 2 - 4, "=== FLAPPY BIRD ===");
            attroff(COLOR_PAIR(1) | A_BOLD);
            tulisTengah(LINES / 2 - 1, "Tekan SPASI untuk mulai");
            tulisTengah(LINES / 2,     "SPASI = terbang");
            tulisTengah(LINES / 2 + 2, "Tekan Q untuk keluar");
            char teksTertinggi[40];
            snprintf(teksTertinggi, sizeof(teksTertinggi), "Skor tertinggi: %d", skorTertinggi);
            tulisTengah(LINES / 2 + 4, teksTertinggi);
        }

        else if (keadaan == BERMAIN) {
            if (tombol == ' ') kecepatanBurung = KEKUATAN_LOMPAT;

            kecepatanBurung += GRAVITASI;
            burungY += kecepatanBurung;
            if (burungY < 0) {
                burungY = 0;
                kecepatanBurung = 0;
            }

            int terjauh = 0;
            for (int i = 0; i < JUMLAH_PIPA; i++) {
                pipaX[i]--;
                if (pipaX[i] > terjauh) terjauh = pipaX[i];
            }
            for (int i = 0; i < JUMLAH_PIPA; i++) {
                if (!sudahLewat[i] && pipaX[i] + LEBAR_PIPA - 1 < BURUNG_X) {
                    sudahLewat[i] = true;
                    skor++;
                }
                if (pipaX[i] + LEBAR_PIPA <= 0) {
                    pipaX[i] = terjauh + JARAK_PIPA;
                    celahAtas[i] = acakCelah();
                    sudahLewat[i] = false;
                    terjauh = pipaX[i];
                }
            }

            int baris = (int)burungY;
            bool mati = (baris >= TINGGI - 1);   // jatuh ke tanah
            for (int i = 0; i < JUMLAH_PIPA; i++) {
                bool sejajarPipa = BURUNG_X + 1 >= pipaX[i] && BURUNG_X < pipaX[i] + LEBAR_PIPA;
                bool diluarCelah = baris < celahAtas[i] || baris >= celahAtas[i] + TINGGI_CELAH;
                if (sejajarPipa && diluarCelah) mati = true;
            }

            if (mati) {
                if (skor > skorTertinggi) skorTertinggi = skor;
                keadaan = SELESAI;
            }

            gambarPermainan();
        }

        else if (keadaan == SELESAI) {
            if (tombol == 'r' || tombol == 'R') {
                mulaiUlang();
                keadaan = BERMAIN;
            }
            gambarPermainan();

            char teks[40];
            attron(COLOR_PAIR(3) | A_BOLD | A_REVERSE);
            tulisTengah(LINES / 2 - 3, "   GAME SELESAI   ");
            attroff(COLOR_PAIR(3) | A_BOLD | A_REVERSE);
            snprintf(teks, sizeof(teks), "Skor: %d   Tertinggi: %d", skor, skorTertinggi);
            tulisTengah(LINES / 2 - 1, teks);
            tulisTengah(LINES / 2 + 1, "R = main lagi    Q = keluar");
        }

        refresh();
        napms(KECEPATAN);
    }

    endwin();

    cout << "MAKASIE SUDAH MAININ GEM INI!" << endl;
    cout << "Skor tertinggi kamu: " << skorTertinggi << endl;
    return 0;
}