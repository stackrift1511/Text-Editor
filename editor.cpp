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

void readInput(){
    while(true){
        char c;
        read(STDIN_FILENO, &c, 1);
        if(c=='q'){
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
