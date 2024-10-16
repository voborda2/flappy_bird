#include <SFML/Graphics.hpp>
#include <cstdlib> // pro funkci rand()
#include <vector>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Flappy Bird");

    // Načtení textur
    sf::Texture birdTexture, backgroundTexture, pipeTexture;
    birdTexture.loadFromFile("C:/Users/Adam/Desktop/Skola/CVUT/Flappy/flappy_bird/assets/bird.png");
    backgroundTexture.loadFromFile("C:/Users/Adam/Desktop/Skola/CVUT/Flappy/flappy_bird/assets/background.png");
    pipeTexture.loadFromFile("C:/Users/Adam/Desktop/Skola/CVUT/Flappy/flappy_bird/assets/pipe.png");

    // Sprity
    sf::Sprite birdSprite(birdTexture);
    sf::Sprite backgroundSprite(backgroundTexture);

    // Inicializace pozice ptáčka a pozadí
    birdSprite.setPosition(100, 300); // Počáteční pozice ptáčka
    backgroundSprite.setPosition(0, 0); // Pozadí zůstává na místě

    // Proměnné pro logiku ptáčka
    float birdVelocity = 0.0f; // Rychlost ptáčka (y-směr)
    const float gravity = 0.001f; // Gravitační konstanta
    const float jumpStrength = -0.5f; // Síla skoku (při kliknutí)

    // Proměnné pro překážky
    sf::Sprite pipeSprite(pipeTexture); // Jedna roura
    std::vector<sf::Sprite> pipes; // Vektor všech rour
    const float pipeSpeed = 1.0f; // Rychlost pohybu rour z pravé strany doleva
    const float pipeGap = 250.0f; // Mezera, kterou ptáček může proletět
    sf::Clock pipeSpawnClock; // Pro měření času mezi generováním rour

    // Proměnná pro ukončení hry
    bool gameOver = false;

    // Detekce kliknutí pro skok (aby reagoval jen na jedno kliknutí)
    bool isMouseClicked = false;

    // Hlavní smyčka hry
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Kontrola, zda hra neskončila
        if (!gameOver) {
            // Gravitace (ptáček padá dolů)
            birdVelocity += gravity;
            birdSprite.move(0, birdVelocity);

            // Pokud bylo kliknuto myší a ptáček skáče jen na jedno kliknutí
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                if (!isMouseClicked) { // Pokud kliknutí nebylo detekováno
                    birdVelocity = jumpStrength; // Inicializuj skok
                    isMouseClicked = true; // Zamez vícenásobnému skoku při držení
                }
            } else {
                isMouseClicked = false; // Po uvolnění tlačítka může znovu kliknout
            }

            // Generování překážek
            if (pipeSpawnClock.getElapsedTime().asSeconds() > 1.5) { // Každých 1.5 sekundy
                pipeSpawnClock.restart();

                // Náhodná výška mezery
                float pipeOffset = rand() % 200 + 200; // Mezery ve výšce mezi 200 a 400 pixely

                // Vytvoření horní roury
                sf::Sprite upperPipe(pipeSprite);
                upperPipe.setPosition(1280, pipeOffset - pipeGap); // Nastav pozici
                pipes.push_back(upperPipe);

                // Vytvoření dolní roury
                sf::Sprite lowerPipe(pipeSprite);
                lowerPipe.setPosition(1280, pipeOffset + pipeGap); // Nastav pozici
                pipes.push_back(lowerPipe);
            }

            // Pohyb rour z pravé strany doleva
            for (auto &pipe : pipes) {
                pipe.move(-pipeSpeed, 0);
            }

            // Konec hry, pokud ptáček narazí na rouru nebo na okraj okna
            for (auto &pipe : pipes) {
                if (birdSprite.getGlobalBounds().intersects(pipe.getGlobalBounds()) ||
                    birdSprite.getPosition().y < 0 || birdSprite.getPosition().y + birdSprite.getGlobalBounds().height > 600) {
                    gameOver = true; // Hra končí, ale okno zůstane otevřené
                }
            }
        }

        // Vykreslení
        window.clear();
        window.draw(backgroundSprite); // Vykreslení pozadí
        for (auto &pipe : pipes) {
            window.draw(pipe); // Vykreslení každé roury
        }
        window.draw(birdSprite); // Vykreslení ptáčka
        window.display();
        std::cout << "Bird position: " << birdSprite.getPosition().y << std::endl;
    }

    return 0;
}
