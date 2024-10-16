#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Flappy Bird");

    // Textury
    sf::Texture birdTexture;
    sf::Texture backgroundTexture;
    sf::Texture pipeTexture;

    // Načtení textur ze souborů
    if (!birdTexture.loadFromFile("assets/bird.png")) {
        // Ošetření chyby, pokud se textura nenačte
        return -1;
    }
    if (!backgroundTexture.loadFromFile("assets/background.png")) {
        return -1;
    }
    if (!pipeTexture.loadFromFile("assets/pipe.png")) {
        return -1;
    }

    // Sprity
    sf::Sprite birdSprite(birdTexture);
    sf::Sprite backgroundSprite(backgroundTexture);
    sf::Sprite pipeSprite(pipeTexture);

    // Nastavení pozice spritů (volitelné)
    birdSprite.setPosition(100, 300); // Pozice ptáčka
    backgroundSprite.setPosition(0, 0); // Pozice pozadí
    pipeSprite.setPosition(500, 400); // Pozice překážky (roury)

    // Hlavní smyčka hry
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Aktualizace logiky hry...

        // Vykreslení
        window.clear();
        window.draw(backgroundSprite); // Nejprve pozadí
        window.draw(pipeSprite); // Překážky
        window.draw(birdSprite); // Ptáček
        window.display();
    }

    return 0;
}