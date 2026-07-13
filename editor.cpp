#include <iostream>
#include <termios.h>
#include <unistd.h>

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

void drawRows(){
    for(int i=1; i<=24; i++){
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void refreshScreen(){
    write(STDOUT_FILENO, "\x1b[2J", 4); // clear screen
    write(STDOUT_FILENO, "\x1b[H", 3);  // cursor to top-left
    drawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);  // cursor to top-left again
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
        std::cout << "key: " << (int)c << " ('" << c << "')\r\n";
        
    }
}

int main() {
    Terminal term;
    readInput();
    std::cout << "Editor starting...\n";
    return 0;
}
