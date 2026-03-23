#include <exception>
#include <iostream>

#include "Application.h"

int main() {
    try {
        Application app;
        return app.Run();
    } catch (const std::exception& exception) {
        std::cerr << "Application error: " << exception.what() << '\n';
        return 1;
    }
}
