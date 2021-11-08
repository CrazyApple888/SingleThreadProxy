#include <iostream>
#include "Proxy.h"

/// $ prog_name is_debug
int main(int argc, char *argv[]) {
    bool is_debug = (argc == 3 && strcmp("-d", argv[2]) == 0);
    int port;
    try {
        port = std::stoi(argv[1]);
    } catch (std::exception &exc) {
        std::cerr << exc.what() << std::endl;
        return EXIT_FAILURE;
    }
    auto proxy = new Proxy(is_debug);
    proxy->start(port);
}