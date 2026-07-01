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

int main() {
    Terminal term;
    std::cout << "Editor starting...\n";
    return 0;
}
