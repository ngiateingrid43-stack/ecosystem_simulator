
#include "Graphics/Window.h"
#include "Core/Ecosystem.h"
#include <iostream>

int main() {
    // Minimal entry point: create an ecosystem, initialize it, print a summary and exit.
    Ecosystem::Core::Ecosystem ecosystem(1200.0f, 800.0f, 500);
    ecosystem.Initialize(10, 5, 20);

    std::cout << "Simulation démarrée: entités=" << ecosystem.GetEntityCount()
              << " nourriture=" << ecosystem.GetFoodCount() << std::endl;

    return 0;
}

