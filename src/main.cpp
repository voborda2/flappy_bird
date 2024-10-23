#include <SFML/Graphics.hpp>
#include <cstdlib> // pro funkci rand()
#include <vector>
#include <iostream>
#include <string>
#include <filesystem>

int main() {

    std::filesystem::current_path("../../../..");  // Přejde o 4 úrovně výš z "build/bin/Debug" do kořene projektu, jinak nejde relativni path s CMakem a debuggem wtf
    
    std::cout << "Current path: " << std::filesystem::current_path() << std::endl;

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Flappy Bird");

    // Načtení textur
    sf::Texture birdTexture, backgroundTexture, pipeTexture;
    birdTexture.loadFromFile("assets/bird3.png");
    backgroundTexture.loadFromFile("assets/background2.png");
    pipeTexture.loadFromFile("assets/pipe1.png");

    // Sprity
    sf::Sprite birdSprite(birdTexture);
    sf::Sprite backgroundSprite(backgroundTexture);

    // Škálování pozadí na velikost okna
    backgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / backgroundSprite.getGlobalBounds().width,
        static_cast<float>(window.getSize().y) / backgroundSprite.getGlobalBounds().height
    );

    // Inicializace pozice ptáčka a pozadí
    birdSprite.setPosition(100, 300); // Počáteční pozice ptáčka
    //backgroundSprite.setPosition(0, 0); // Pozadí zůstává na místě

    // Proměnné pro logiku ptáčka
    float birdVelocity = 0.0f; // Rychlost ptáčka (y-směr)
    const float gravity = 0.0005f; // Gravitační konstanta
    const float jumpStrength = -0.3f; // Síla skoku (při kliknutí)

    // Proměnné pro překážky
    sf::Sprite pipeSprite(pipeTexture); // Jedna roura
    std::vector<sf::Sprite> pipes; // Vektor všech rour
    float pipeSpeed = 0.15f; // Rychlost pohybu rour z pravé strany doleva
    float speedIncreaseRate = 0.01f; // Míra zrychlení
    int speedIncreaseScoreThreshold = 4; // Po každých 5 bodech zvýšíme rychlost
    float maxPipeSpeed = 15.0f; // Maximální rychlost rour
    const float pipeGap = 300.0f; // Mezera, kterou ptáček může proletět
    float pipeSpawnInterval = 0.3f; // Počáteční rychlost pro generování rour
    const float beggpipeSpawnInterval = pipeSpawnInterval;
    const float beggpipeSpeed = pipeSpeed;
    sf::Clock pipeSpawnClock; // Pro měření času mezi generováním rour

    // Proměnná pro ukončení hry
    bool gameOver = false;

    // Proměnná pro skóre
    int score =0;
    int bestScore = 0; 
    int attemptCount = 1; // Proměnná pro počítání pokusů

    // Pismo a text pro skore
    sf::Font font;
    font.loadFromFile("assets/arial.ttf");
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    // Text pro nejlepší skóre
    sf::Text bestScoreText;
    bestScoreText.setFont(font);
    bestScoreText.setCharacterSize(40);
    bestScoreText.setFillColor(sf::Color::Yellow);
    bestScoreText.setPosition(250, 10);

    // Text pro pokusy
    sf::Text attemptText;
    attemptText.setFont(font);
    attemptText.setCharacterSize(40);
    attemptText.setFillColor(sf::Color::Green);
    attemptText.setPosition(500, 10); // Umístění pokusů vedle nejlepšího skóre

    // Tlačítko restartu
    sf::RectangleShape restartButton(sf::Vector2f(200, 50));
    restartButton.setFillColor(sf::Color::Green);
    restartButton.setPosition(540, 300); // Střed obrazovky

    sf::Text buttonText;
    buttonText.setFont(font);
    buttonText.setString("Restart");
    buttonText.setCharacterSize(40);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setPosition(570, 300); // Na tlačítku


    // Detekce kliknutí pro skok (aby reagoval jen na jedno kliknutí)
    bool isMouseClicked = false;

    // Pomocná proměnná pro zaznamenání, že ptáček přeletěl rouru
    std::vector<bool> passedPipes;

    // Hlavní smyčka hry
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Restartování hry při kliknutí na tlačítko po skončení
            if (gameOver && event.type == sf::Event::MouseButtonPressed) {
                if (restartButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    // Reset hry
                    birdSprite.setPosition(100, 300);
                    pipes.clear();
                    passedPipes.clear();
                    birdVelocity = 0.0f;
                    score = 0;
                    gameOver = false;
                    attemptCount++; // Zvýšení počtu pokusů
                    pipeSpawnInterval = beggpipeSpawnInterval;
                    pipeSpeed = beggpipeSpeed;
                }
            }
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
            if (pipeSpawnClock.getElapsedTime().asSeconds() > pipeSpawnInterval / pipeSpeed) {
                pipeSpawnClock.restart();

                // Náhodná výška mezery
                float pipeOffset = rand() % 100 + 200;  // Náhodná hodnota mezi 180 a 360 pixely

                // Vytvoření horní roury
                sf::Sprite upperPipe(pipeSprite);
                upperPipe.setPosition(1280, pipeOffset - upperPipe.getGlobalBounds().height); // Nastav pozici
                pipes.push_back(upperPipe);
                passedPipes.push_back(false); // Ověření průletu

                // Vytvoření dolní roury
                sf::Sprite lowerPipe(pipeSprite);
                lowerPipe.setPosition(1280, pipeOffset + pipeGap); // Nastav pozici
                pipes.push_back(lowerPipe);
            }

            // Pohyb rour z pravé strany doleva
            for (size_t i = 0; i < pipes.size(); i += 2) { // Zpracováváme roury po dvojicích (horní a dolní)
                pipes[i].move(-pipeSpeed, 0);     // Horní roura
                pipes[i+1].move(-pipeSpeed, 0);   // Dolní roura

                // Skórování při proletu mezi rourami
                if (pipes[i].getPosition().x + pipes[i].getGlobalBounds().width < birdSprite.getPosition().x && !passedPipes[i / 2]) {
                    score++;
                    passedPipes[i / 2] = true; // Označíme, že ptáček touto rourou prošel
                    std::cout << "Score: " << score << std::endl;

                    // Zrychlení pohybu rour po každých 5 bodech
                    if (score % speedIncreaseScoreThreshold == 0) {
                        pipeSpeed += speedIncreaseRate;

                        // Omezíme maximální rychlost
                        if (pipeSpeed > maxPipeSpeed) {
                            pipeSpeed = maxPipeSpeed;
                        }

                        std::cout << "Increased pipe speed: " << pipeSpeed << std::endl;
                    }
                }
            }

            // Konec hry, pokud ptáček narazí na rouru nebo na okraj okna
            for (auto &pipe : pipes) {
                if (birdSprite.getGlobalBounds().intersects(pipe.getGlobalBounds()) ||
                    birdSprite.getPosition().y < 0 || birdSprite.getPosition().y + birdSprite.getGlobalBounds().height > window.getSize().y) {
                    gameOver = true;

                    // Uložení nejlepšího skóre
                    if (score > bestScore) {
                        bestScore = score;
                    }
                }
            }

            // Aktualizace skóre
            scoreText.setString("Score: " + std::to_string(score));
            bestScoreText.setString("Best: " + std::to_string(bestScore));
            attemptText.setString("Attempts: " + std::to_string(attemptCount)); // Aktualizace pokusů
        }

        // Vykreslení
        window.clear();
        window.draw(backgroundSprite); // Vykreslení pozadí
        for (auto &pipe : pipes) {
            window.draw(pipe); // Vykreslení každé roury
        }
        window.draw(birdSprite); // Vykreslení ptáčka
        window.draw(scoreText); // Vykreslení skóre
        window.draw(bestScoreText); // Vykreslení nejlepšího skóre
        window.draw(attemptText); // Vykreslení počtu pokusů

        // Pokud hra skončila, zobrazíme tlačítko Restart
        if (gameOver) {
            window.draw(restartButton);
            window.draw(buttonText);
        }

        window.display();
        //std::cout << "Bird position: " << birdSprite.getPosition().y << std::endl;
    }

    return 0;
}
