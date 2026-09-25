#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "[SANDBOX EXAMPLE] Hello from inside SandBoxX container!" << std::endl;
    std::cout << "[SANDBOX EXAMPLE] Current PID inside sandbox: " << getpid() << std::endl;
    return 0;
}
