#include <iostream>
#include <unistd.h>

int main() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        std::cout << "[SANDBOX TEST] Isolated UTS Hostname: " << hostname << std::endl;
    } else {
        std::cout << "[SANDBOX TEST] Failed to retrieve hostname." << std::endl;
    }
    return 0;
}
