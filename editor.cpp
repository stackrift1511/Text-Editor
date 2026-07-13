#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <sys/ioctl.h>

struct EditorConfig {
    int cx, cy;
    int rows, cols;
};
EditorConfig E;

class Terminal {
public:
    Terminal(){
        tcgetattr(STDIN_FILENO, &orig_termios);
        struct termios raw;
        raw = orig_termios;
        raw.c_lflag &= ~(ECHO | ICANON | ISIG);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }

    ~Terminal(){
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    }

private:
    struct termios orig_termios;
};

void initEditor(){
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    E.rows = ws.ws_row, E.cols = ws.ws_col;
    E.cx=0, E.cy=0;
}

void drawRows(){
    for(int i=1; i<=E.rows; i++){
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void refreshScreen(){
    write(STDOUT_FILENO, "\x1b[2J", 4); // clear screen
    write(STDOUT_FILENO, "\x1b[H", 3);  // cursor to top-left
    drawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);  // cursor to top-left again
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.cy + 1, E.cx + 1);
    write(STDOUT_FILENO, buf, strlen(buf));
}


void readInput(){
    while(true){
        refreshScreen();
        char c;
        read(STDIN_FILENO, &c, 1);
        if(c=='q'){
            //clean up the screen
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            break;
        }
        else if (c == '\x1b') {
            char seq[2];
            read(STDIN_FILENO, &seq[0], 1); // read '['
            read(STDIN_FILENO, &seq[1], 1); // read A/B/C/D
            if (seq[0] == '[') {
                    switch (seq[1]) 
                    {
                        // cy is the row, and row 0 is at the top. So moving up means decreasing cy, moving down means increasing it
                        case 'A': if(E.cy > 0) E.cy--; 
                              break; // up
                        case 'B': if(E.cy < E.rows - 1 ) E.cy++;
                              break;// down
                        case 'C': if(E.cx < E.cols - 1) E.cx++;
                              break;// right
                        case 'D': if(E.cx > 0) E.cx--;
                              break;// left
                    }
            }
        }
        
        
    }
}

int main() {
    Terminal term;
    initEditor();
    readInput();
    std::cout << "Editor starting...\n";
    return 0;
}
