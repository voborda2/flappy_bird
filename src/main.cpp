#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Flappy Bird");

    // Načtení textur, inicializace proměnných

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Logika hry (gravitace, kolize, pohyb)

        window.clear();
        // Vykreslení ptáčka, pozadí, překážek
        window.display();
    }

    return 0;
}