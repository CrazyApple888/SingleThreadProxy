#include <iostream>
#include <fstream>
#include "Logger.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    auto logger = new Logger(false, std::cout);
    logger->info("AB", "A");
    auto str = std::string("B");
    logger->info("AB", str);
    logger->debug("AB", "C");

    std::ofstream file;
    file.open("logs.txt");
    auto file_logger = Logger(true, file);
    file_logger.info("AB", "CCCC");
    return 0;
}
