#include <iostream>
#include <vector>
#include <cstring>

int main() {
    std::cout << "[SANDBOX TEST] Starting Memory allocation test..." << std::endl;
    try {
        std::vector<char*> allocations;
        for (int i = 0; i < 20; ++i) {
            char* ptr = new char[10 * 1024 * 1024]; // 10 MB chunks
            std::memset(ptr, 1, 10 * 1024 * 1024);
            allocations.push_back(ptr);
            std::cout << "[SANDBOX TEST] Allocated total " << (i + 1) * 10 << " MB memory." << std::endl;
        }
        for (auto* ptr : allocations) delete[] ptr;
    } catch (const std::exception& e) {
        std::cout << "[SANDBOX TEST] Memory allocation caught exception (Limit Enforced): " << e.what() << std::endl;
    }
    return 0;
}
