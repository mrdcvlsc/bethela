#ifndef BETHELA_TIMING_HPP
#define BETHELA_TIMING_HPP

#include <iostream>
#include <chrono>

auto timing_start() {
    std::cout << "program running, please wait...\n";
    return std::chrono::high_resolution_clock::now();
}

template<typename T>
void timing_end(std::string const& NAME, T start, size_t cnt) {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << NAME << "cryption took : \n";
    std::cout << "\t" << duration.count() << " milliseconds\n";
    std::cout << "\t" << duration.count() / 1000 << " seconds\n";
    std::cout << "\t" << NAME << "crypted File(s) : " << cnt << "\n";
}

#endif