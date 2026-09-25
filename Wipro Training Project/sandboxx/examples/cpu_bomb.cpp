#include <iostream>
#include <chrono>

int main() {
    std::cout << "[SANDBOX TEST] Starting CPU load test inside sandbox..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    volatile double dummy = 0.0;
    while (true) {
        dummy += 1.0;
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = now - start;
        if (elapsed.count() > 3.0) break;
    }
    std::cout << "[SANDBOX TEST] CPU load test finished safely." << std::endl;
    return 0;
}
