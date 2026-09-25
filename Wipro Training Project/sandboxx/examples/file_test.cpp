#include <iostream>
#include <fstream>

int main() {
    std::cout << "[SANDBOX TEST] Testing Filesystem access inside sandbox..." << std::endl;
    std::ofstream testFile("/tmp/sandbox_test.txt");
    if (testFile.is_open()) {
        testFile << "Written from inside isolated SandBoxX container.\n";
        testFile.close();
        std::cout << "[SANDBOX TEST] File write to /tmp succeeded!" << std::endl;
    } else {
        std::cerr << "[SANDBOX TEST] File write restricted." << std::endl;
    }
    return 0;
}
