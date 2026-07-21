#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <sys/ioctl.h>

struct EditorConfig {
    int cx, cy;
    int rows, cols;
    int rowoff; // row offset - how far we've scrolled.
    std::vector<std::string> lines; //file contents
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

void scroll(){
    //if cursor is above visible area, scroll up
    if(E.cy < E.rowoff)
        E.rowoff = E.cy;
    //if cursor is below visible area, scroll down 
    if(E.cy >= E.rowoff + E.rows)
        E.rowoff = E.cy - E.rows + 1;
}

void loadFile(const char* filename){
    std::ifstream file(filename); //opens the file - different name from parameter
    std::string line;
    while(std::getline(file, line)) { //read one line at a time
        E.lines.push_back(line); //add it to the vector
    }
}

void initEditor(){
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    E.rows = ws.ws_row, E.cols = ws.ws_col;
    E.cx=0, E.cy=0, E.rowoff=0;
}

void drawRows(){
    for(int i=0; i<E.rows; i++){
        int filerow = i + E.rowoff;
        if(filerow < (int)E.lines.size()){
            int len = E.lines[filerow].size();
            if(len > E.cols) len = E.cols;
            write(STDOUT_FILENO, E.lines[filerow].c_str(), len);
        } else {
            write(STDOUT_FILENO, "~", 1);
        }
        write(STDOUT_FILENO, "\r\n", 2);
    }
}

void refreshScreen(){
    scroll();
    write(STDOUT_FILENO, "\x1b[2J", 4); // clear screen
    write(STDOUT_FILENO, "\x1b[H", 3);  // cursor to top-left
    drawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);  // cursor to top-left again
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1, E.cx + 1);
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
                        case 'B': if(E.cy < (int)E.lines.size() - 1 ) E.cy++;
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

int main(int argc, char* argv[]) {
    Terminal term;
    initEditor();
    if (argc >= 2 ) loadFile(argv[1]);
    readInput();
    return 0;
}
