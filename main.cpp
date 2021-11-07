#include <iostream>
#include "Proxy.h"

/// $ prog_name is_debug
int main(int argc, char *argv[]) {
    bool is_debug = (argc == 2 && strcmp("-d", argv[1]) == 0);
    auto proxy = new Proxy(is_debug);
    proxy->start(3444);
}