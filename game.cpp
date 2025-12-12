#include "game.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>

// Constructeur
Game::Game() 
    : window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE + SCORE_AREA_HEIGHT), "Tic-Tac-Toe"),
      currentPlayer(1),
      gameOver(false),
      winner(0),
      scoreX(0),
      scoreO(0),
      scoreDraw(0),
      hasWinningLine(false),
      gameMode(PLAYER_VS_PLAYER),
      showModeSelection(true),
      hoveredCell(-1),
      hoveredButton(-1),
      firstPlayer(1),
      lastFirstPlayer(0),
      soundsLoaded(false) {
    
    std::srand(std::time(nullptr));
    
    for (int i = 0; i < 9; i++) {
        board[i] = 0;
    }
    
    for (int i = 0; i < 3; i++) {
        winningCombo[i] = -1;
    }
    
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Erreur :  Impossible de charger la police!" << std::endl;
    }
    
    
    loadSounds();
    
    createGridLines();
    createModeButtons();
    
    std::cout << "Tic-Tac-Toe - Jeu démarré!" << std::endl;
}

// Charger les sons
void Game:: loadSounds() {
    soundsLoaded = true;
    
    if (! clickBuffer.loadFromFile("sounds/click.wav")) {
        std::cerr << "Impossible de charger click.wav" << std::endl;
        soundsLoaded = false;
    } else {
        clickSound.setBuffer(clickBuffer);
        clickSound.setVolume(50);
    }
    
    if (!winBuffer.loadFromFile("sounds/win.wav")) {
        std::cerr << "Impossible de charger win.wav" << std:: endl;
        soundsLoaded = false;
    } else {
        winSound.setBuffer(winBuffer);
        winSound.setVolume(70);
    }
    
    if (!drawBuffer.loadFromFile("sounds/draw.wav")) {
        std::cerr << "Impossible de charger draw.wav" << std::endl;
        soundsLoaded = false;
    } else {
        drawSound.setBuffer(drawBuffer);
        drawSound.setVolume(60);
    }
    
    if (soundsLoaded) {
        std::cout << "Sons chargés avec succès!" << std::endl;
    } else {
        std::cout << "Jeu en mode silencieux (sons non trouvés)" << std::endl;
    }
}

// Jouer le son de clic 
void Game::playClickSound() {
    if (soundsLoaded && clickSound.getStatus() != sf::Sound::Playing) {
        clickSound.play();
    }
}

// Jouer le son de victoire 
void Game::playWinSound() {
    if (soundsLoaded) {
        winSound.play();
    }
}

// Jouer le son de match nul 
void Game::playDrawSound() {
    if (soundsLoaded) {
        drawSound.play();
    }
}

// Définir aléatoirement le premier joueur
void Game::setRandomFirstPlayer() {
    firstPlayer = (std::rand() % 2) + 1;
    lastFirstPlayer = firstPlayer;
    currentPlayer = firstPlayer;
    
    std::cout << "Tirage au sort : Le joueur " 
              << (firstPlayer == 1 ? "X" : "O") 
              << " commence !" << std::endl;
}

// Alterner le joueur qui commence
void Game::alternateFirstPlayer() {
    firstPlayer = (lastFirstPlayer == 1) ? 2 : 1;
    lastFirstPlayer = firstPlayer;
    currentPlayer = firstPlayer;
    
    std::cout << "Alternance :  Le joueur " 
              << (firstPlayer == 1 ?  "X" : "O") 
              << " commence cette partie !" << std::endl;
}

// Obtenir la couleur d'un joueur
sf::Color Game::getPlayerColor(int player) {
    if (player == 1) return sf::Color(52, 152, 219);
    else return sf::Color(231, 76, 60);
}

// Obtenir la case depuis la position de la souris
int Game::getCellFromPosition(int mouseX, int mouseY) {
    int adjustedY = mouseY - SCORE_AREA_HEIGHT;
    if (adjustedY < 0 || adjustedY >= WINDOW_SIZE) return -1;
    if (mouseX < 0 || mouseX >= WINDOW_SIZE) return -1;
    
    int col = mouseX / CELL_SIZE;
    int row = adjustedY / CELL_SIZE;
    int cellIndex = row * 3 + col;
    
    if (cellIndex >= 0 && cellIndex <= 8) return cellIndex;
    return -1;
}

// Créer les boutons de sélection de mode
void Game:: createModeButtons() {
    float buttonWidth = 450;
    float buttonHeight = 70;
    float startY = 180;
    float spacing = 90;
    
    std::vector<std::string> buttonLabels = {
        "Joueur vs Joueur",
        "Joueur vs IA (Facile)",
        "Joueur vs IA (Difficile)"
    };
    
    for (int i = 0; i < 3; i++) {
        sf::RectangleShape button(sf:: Vector2f(buttonWidth, buttonHeight));
        button.setPosition((WINDOW_SIZE - buttonWidth) / 2, startY + i * spacing);
        button.setFillColor(sf::Color(52, 152, 219));
        button.setOutlineThickness(0);
        modeButtons.push_back(button);
        
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(30);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        text.setString(buttonLabels[i]);
        
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(
            (WINDOW_SIZE - textBounds.width) / 2 - textBounds.left,
            startY + i * spacing + (buttonHeight - textBounds.height) / 2 - textBounds.top
        );
        modeButtonTexts.push_back(text);
    }
}

// Créer les lignes de la grille
void Game::createGridLines() {
    for (int i = 1; i < 3; i++) {
        sf::RectangleShape line(sf::Vector2f(LINE_THICKNESS, WINDOW_SIZE));
        line.setPosition(i * CELL_SIZE, SCORE_AREA_HEIGHT);
        line.setFillColor(sf::Color(189, 195, 199));
        lines.push_back(line);
    }
    
    for (int i = 1; i < 3; i++) {
        sf::RectangleShape line(sf::Vector2f(WINDOW_SIZE, LINE_THICKNESS));
        line.setPosition(0, i * CELL_SIZE + SCORE_AREA_HEIGHT);
        line.setFillColor(sf::Color(189, 195, 199));
        lines.push_back(line);
    }
    
    sf::RectangleShape separator(sf::Vector2f(WINDOW_SIZE, 3));
    separator.setPosition(0, SCORE_AREA_HEIGHT);
    separator.setFillColor(sf::Color(149, 165, 166));
    lines.push_back(separator);
}

// Gérer le mouvement de la souris
void Game::handleMouseMove(int mouseX, int mouseY) {
    if (showModeSelection) {
        hoveredButton = -1;
        for (size_t i = 0; i < modeButtons.size(); i++) {
            if (modeButtons[i].getGlobalBounds().contains(mouseX, mouseY)) {
                hoveredButton = i;
                break;
            }
        }
    } else if (! gameOver) {
        hoveredCell = getCellFromPosition(mouseX, mouseY);
        if (hoveredCell != -1 && board[hoveredCell] != 0) {
            hoveredCell = -1;
        }
    }
}

// Boucle principale du jeu
void Game::run() {
    while (window. isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (event. type == sf::Event::KeyPressed) {
                handleKeyPress(event.key.code);
            }
            
            if (event.type == sf::Event:: MouseMoved) {
                handleMouseMove(event.mouseMove.x, event.mouseMove.y);
            }
            
            if (event.type == sf::Event:: MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (showModeSelection) {
                        handleModeSelection(event.mouseButton.x, event.mouseButton.y);
                    } else if (! gameOver) {
                        handleClick(event.mouseButton.x, event.mouseButton.y);
                    }
                }
            }
        }
        
        // Rendu
        window.clear(sf::Color(236, 240, 241));
        
        if (showModeSelection) {
            drawModeSelection();
        } else {
            drawScoreBoard();
            drawGrid();
            drawCellHighlight();
            drawSymbols();
            drawCurrentPlayerIndicator();
            
            if (hasWinningLine) {
                drawWinningLine();
            }
            
            if (gameOver) {
                drawEndScreen();
            }
        }
        
        window.display();
        
        // Tour de l'IA
        if (! showModeSelection && !gameOver && currentPlayer == 2 && gameMode != PLAYER_VS_PLAYER) {
            sf::sleep(sf::milliseconds(500));
            aiMove();
        }
    }
}

// Dessiner le highlight de la case survolée
void Game::drawCellHighlight() {
    if (hoveredCell != -1 && ! gameOver) {
        int col = hoveredCell % 3;
        int row = hoveredCell / 3;
        
        sf::RectangleShape highlight(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        highlight.setPosition(col * CELL_SIZE, row * CELL_SIZE + SCORE_AREA_HEIGHT);
        highlight.setFillColor(sf::Color(52, 152, 219, 30));
        window.draw(highlight);
    }
}

// Dessiner l'indicateur du joueur actuel
void Game::drawCurrentPlayerIndicator() {
    if (gameOver) return;
    
    std::string playerText = "Tour :  ";
    playerText += (currentPlayer == 1 ? "X" : "O");
    
    if (currentPlayer == 2 && gameMode != PLAYER_VS_PLAYER) {
        playerText += " (IA)";
    }
    
    sf::Text indicator;
    indicator.setFont(font);
    indicator.setString(playerText);
    indicator.setCharacterSize(22);
    indicator.setFillColor(getPlayerColor(currentPlayer));
    indicator.setStyle(sf::Text::Bold);
    
    indicator.setPosition(10, WINDOW_SIZE + SCORE_AREA_HEIGHT - 35);
    
    window.draw(indicator);
}

// Afficher l'écran de sélection de mode
void Game:: drawModeSelection() {
    float time = animationClock.getElapsedTime().asSeconds();
    float bounce = std::sin(time * 2) * 5;
    
    sf::Text title;
    title.setFont(font);
    title.setString("TIC-TAC-TOE");
    title.setCharacterSize(70);
    title.setFillColor(sf::Color(52, 73, 94));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition((WINDOW_SIZE - titleBounds.width) / 2, 40 + bounce);
    window.draw(title);
    
    sf::Text subtitle;
    subtitle.setFont(font);
    subtitle.setString("Choisissez votre mode de jeu");
    subtitle.setCharacterSize(24);
    subtitle.setFillColor(sf::Color(127, 140, 141));
    
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition((WINDOW_SIZE - subtitleBounds.width) / 2, 125);
    window.draw(subtitle);
    
    for (size_t i = 0; i < modeButtons.size(); i++) {
        sf::RectangleShape button = modeButtons[i];
        
        if (hoveredButton == static_cast<int>(i)) {
            button.setFillColor(sf::Color(41, 128, 185));
            button.setScale(1.02f, 1.02f);
            sf::Vector2f originalPos = modeButtons[i].getPosition();
            sf::FloatRect buttonBounds = modeButtons[i].getLocalBounds();
            button.setPosition(
                originalPos.x - buttonBounds.width * 0.01f,
                originalPos.y - buttonBounds.height * 0.01f
            );
        }
        
        window. draw(button);
        window.draw(modeButtonTexts[i]);
    }
    
    sf::Text footer;
    footer.setFont(font);
    footer.setString("Cree par Landry (C++ et SFML)");
    footer.setCharacterSize(18);
    footer.setFillColor(sf::Color(149, 165, 166));
    
    sf::FloatRect footerBounds = footer.getLocalBounds();
    footer.setPosition((WINDOW_SIZE - footerBounds.width) / 2, WINDOW_SIZE + SCORE_AREA_HEIGHT - 30);
    window.draw(footer);
}

// Gérer la sélection de mode
void Game::handleModeSelection(int mouseX, int mouseY) {
    for (size_t i = 0; i < modeButtons.size(); i++) {
        if (modeButtons[i].getGlobalBounds().contains(mouseX, mouseY)) {
            
            playClickSound();
            
            gameMode = static_cast<GameMode>(i);
            showModeSelection = false;
            
            setRandomFirstPlayer();
            
            std::cout << "Mode sélectionné : ";
            if (gameMode == PLAYER_VS_PLAYER) std::cout << "Joueur vs Joueur";
            else if (gameMode == PLAYER_VS_AI_EASY) std::cout << "Joueur vs IA (Facile)";
            else std::cout << "Joueur vs IA (Difficile)";
            std::cout << std::endl;
            
            break;
        }
    }
}

// Gérer les clics de souris avec son 
void Game::handleClick(int mouseX, int mouseY) {
    int cellIndex = getCellFromPosition(mouseX, mouseY);
    if (cellIndex == -1) return;
    
    if (board[cellIndex] == 0) {
        board[cellIndex] = currentPlayer;
        
        // Son de clic 
        playClickSound();
        
        std::cout << "Joueur " << (currentPlayer == 1 ? "X" : "O") 
                  << " joue sur la case " << cellIndex << std::endl;
        
        winner = checkWinner();
        if (winner != 0) {
            gameOver = true;
            hasWinningLine = true;
            updateScore();
            
            // Son de victoire 
            playWinSound();
            
            std::cout << "Le joueur " << (winner == 1 ? "X" : "O") 
                      << " a gagné!" << std::endl;
        } else if (isBoardFull()) {
            gameOver = true;
            winner = 3;
            updateScore();
            
            // Son de match nul 
            playDrawSound();
            
            std::cout << "Match nul!" << std::endl;
        }
        
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    } else {
        std::cout << "Case déjà occupée!" << std:: endl;
    }
}

// Gérer les touches du clavier
void Game::handleKeyPress(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::R && gameOver) {
        resetGame();
    }
    if (key == sf::Keyboard:: M && ! showModeSelection) {
        showModeSelection = true;
        resetGame();
        lastFirstPlayer = 0;
    }
}

// Mouvement de l'IA
void Game::aiMove() {
    if (gameMode == PLAYER_VS_AI_EASY) {
        aiMoveEasy();
    } else if (gameMode == PLAYER_VS_AI_HARD) {
        aiMoveHard();
    }
}

// IA Facile avec son 
void Game::aiMoveEasy() {
    std::vector<int> availableMoves;
    
    for (int i = 0; i < 9; i++) {
        if (board[i] == 0) {
            availableMoves.push_back(i);
        }
    }
    
    if (! availableMoves.empty()) {
        int randomIndex = std::rand() % availableMoves.size();
        int move = availableMoves[randomIndex];
        
        board[move] = currentPlayer;
        
        // Son de clic 
        playClickSound();
        
        std::cout << "IA (Facile) joue sur la case " << move << std::endl;
        
        winner = checkWinner();
        if (winner != 0) {
            gameOver = true;
            hasWinningLine = true;
            updateScore();
            playWinSound();
            std::cout << "L'IA a gagné!" << std::endl;
        } else if (isBoardFull()) {
            gameOver = true;
            winner = 3;
            updateScore();
            playDrawSound();
            std::cout << "Match nul!" << std::endl;
        }
        
        currentPlayer = 1;
    }
}

// IA Difficile avec son 
void Game::aiMoveHard() {
    int bestScore = std::numeric_limits<int>::min();
    int bestMove = -1;
    
    for (int i = 0; i < 9; i++) {
        if (board[i] == 0) {
            board[i] = 2;
            int score = minimax(0, false);
            board[i] = 0;
            
            if (score > bestScore) {
                bestScore = score;
                bestMove = i;
            }
        }
    }
    
    if (bestMove != -1) {
        board[bestMove] = currentPlayer;
        
        // Son de clic 
        playClickSound();
        
        std:: cout << "IA (Difficile) joue sur la case " << bestMove << std::endl;
        
        winner = checkWinner();
        if (winner != 0) {
            gameOver = true;
            hasWinningLine = true;
            updateScore();
            playWinSound();
            std::cout << "L'IA a gagné!" << std::endl;
        } else if (isBoardFull()) {
            gameOver = true;
            winner = 3;
            updateScore();
            playDrawSound();
            std::cout << "Match nul!" << std::endl;
        }
        
        currentPlayer = 1;
    }
}

// Algorithme Minimax
int Game::minimax(int depth, bool isMaximizing) {
    int result = checkWinner();
    
    if (result == 2) return 10 - depth;
    if (result == 1) return depth - 10;
    if (isBoardFull()) return 0;
    
    if (isMaximizing) {
        int bestScore = std::numeric_limits<int>::min();
        for (int i = 0; i < 9; i++) {
            if (board[i] == 0) {
                board[i] = 2;
                int score = minimax(depth + 1, false);
                board[i] = 0;
                bestScore = std::max(score, bestScore);
            }
        }
        return bestScore;
    } else {
        int bestScore = std::numeric_limits<int>::max();
        for (int i = 0; i < 9; i++) {
            if (board[i] == 0) {
                board[i] = 1;
                int score = minimax(depth + 1, true);
                board[i] = 0;
                bestScore = std::min(score, bestScore);
            }
        }
        return bestScore;
    }
}

// Vérifier s'il y a un gagnant
int Game::checkWinner() {
    int winningCombos[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
        {0, 4, 8}, {2, 4, 6}
    };
    
    for (int i = 0; i < 8; i++) {
        int a = winningCombos[i][0];
        int b = winningCombos[i][1];
        int c = winningCombos[i][2];
        
        if (board[a] != 0 && board[a] == board[b] && board[b] == board[c]) {
            winningCombo[0] = a;
            winningCombo[1] = b;
            winningCombo[2] = c;
            return board[a];
        }
    }
    
    return 0;
}

// Vérifier si la grille est pleine
bool Game:: isBoardFull() {
    for (int i = 0; i < 9; i++) {
        if (board[i] == 0) {
            return false;
        }
    }
    return true;
}

// Mettre à jour le score
void Game::updateScore() {
    if (winner == 1) {
        scoreX++;
    } else if (winner == 2) {
        scoreO++;
    } else if (winner == 3) {
        scoreDraw++;
    }
}

// Dessiner la ligne gagnante
void Game::drawWinningLine() {
    if (winningCombo[0] == -1) return;
    
    int startIndex = winningCombo[0];
    int endIndex = winningCombo[2];
    
    int startCol = startIndex % 3;
    int startRow = startIndex / 3;
    int endCol = endIndex % 3;
    int endRow = endIndex / 3;
    
    float startX = startCol * CELL_SIZE + CELL_SIZE / 2.0f;
    float startY = startRow * CELL_SIZE + SCORE_AREA_HEIGHT + CELL_SIZE / 2.0f;
    float endX = endCol * CELL_SIZE + CELL_SIZE / 2.0f;
    float endY = endRow * CELL_SIZE + SCORE_AREA_HEIGHT + CELL_SIZE / 2.0f;
    
    float dx = endX - startX;
    float dy = endY - startY;
    float length = std::sqrt(dx * dx + dy * dy);
    float angle = std::atan2(dy, dx) * 180 / 3.14159f;
    
    sf::RectangleShape winLine(sf::Vector2f(length, 10));
    winLine.setPosition(startX, startY);
    winLine.setRotation(angle);
    winLine.setFillColor(sf::Color(46, 204, 113, 220));
    winLine.setOutlineThickness(2);
    winLine.setOutlineColor(sf:: Color(39, 174, 96));
    
    window.draw(winLine);
}

// Dessiner le tableau des scores
void Game::drawScoreBoard() {
    sf::RectangleShape scoreBackground(sf::Vector2f(WINDOW_SIZE, SCORE_AREA_HEIGHT));
    scoreBackground.setPosition(0, 0);
    scoreBackground.setFillColor(sf:: Color(52, 73, 94));
    window.draw(scoreBackground);
    
    sf::Text title;
    title.setFont(font);
    title.setString("SCORE");
    title.setCharacterSize(24);
    title.setFillColor(sf::Color(236, 240, 241));
    title.setStyle(sf:: Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition((WINDOW_SIZE - titleBounds.width) / 2, 5);
    window.draw(title);
    
    sf::Text scoreXText;
    scoreXText. setFont(font);
    std::ostringstream ossX;
    ossX << "X: " << scoreX;
    scoreXText.setString(ossX.str());
    scoreXText.setCharacterSize(28);
    scoreXText.setFillColor(getPlayerColor(1));
    scoreXText.setStyle(sf::Text::Bold);
    scoreXText.setPosition(50, 40);
    window.draw(scoreXText);
    
    sf::Text scoreDrawText;
    scoreDrawText. setFont(font);
    std::ostringstream ossDraw;
    ossDraw << "Nuls: " << scoreDraw;
    scoreDrawText.setString(ossDraw.str());
    scoreDrawText.setCharacterSize(28);
    scoreDrawText.setFillColor(sf::Color(189, 195, 199));
    scoreDrawText.setStyle(sf::Text::Bold);
    sf::FloatRect drawBounds = scoreDrawText.getLocalBounds();
    scoreDrawText.setPosition((WINDOW_SIZE - drawBounds.width) / 2, 40);
    window.draw(scoreDrawText);
    
    sf::Text scoreOText;
    scoreOText.setFont(font);
    std::ostringstream ossO;
    ossO << "O: " << scoreO;
    scoreOText.setString(ossO.str());
    scoreOText.setCharacterSize(28);
    scoreOText.setFillColor(getPlayerColor(2));
    scoreOText.setStyle(sf:: Text::Bold);
    sf::FloatRect oBounds = scoreOText.getLocalBounds();
    scoreOText.setPosition(WINDOW_SIZE - oBounds. width - 50, 40);
    window.draw(scoreOText);
}

// Dessiner la grille
void Game::drawGrid() {
    for (const auto& line : lines) {
        window.draw(line);
    }
}

// Dessiner les X et O
void Game::drawSymbols() {
    for (int i = 0; i < 9; i++) {
        if (board[i] != 0) {
            int col = i % 3;
            int row = i / 3;
            
            sf::Text text;
            text.setFont(font);
            text.setString(board[i] == 1 ?  "X" : "O");
            text.setCharacterSize(100);
            text.setFillColor(getPlayerColor(board[i]));
            text.setStyle(sf::Text::Bold);
            
            sf::FloatRect bounds = text.getLocalBounds();
            text.setPosition(
                col * CELL_SIZE + (CELL_SIZE - bounds.width) / 2 - bounds.left,
                row * CELL_SIZE + SCORE_AREA_HEIGHT + (CELL_SIZE - bounds.height) / 2 - bounds.top
            );
            
            window.draw(text);
        }
    }
}

// Dessiner l'écran de fin
void Game::drawEndScreen() {
    sf::RectangleShape background(sf::Vector2f(WINDOW_SIZE, 220));
    background.setPosition(0, (WINDOW_SIZE + SCORE_AREA_HEIGHT) / 2 - 110);
    background.setFillColor(sf::Color(255, 255, 255, 245));
    window.draw(background);
    
    sf::Text endText;
    endText.setFont(font);
    
    if (winner == 3) {
        endText.setString("Match nul !");
        endText.setFillColor(sf::Color(52, 73, 94));
    } else {
        std::string winnerText = (winner == 1 ? "X" : "O");
        if (winner == 2 && gameMode != PLAYER_VS_PLAYER) {
            endText.setString("L'IA gagne !");
        } else {
            endText. setString(winnerText + " gagne !");
        }
        endText.setFillColor(getPlayerColor(winner));
    }
    
    endText.setCharacterSize(60);
    endText.setStyle(sf::Text::Bold);
    
    sf::FloatRect bounds = endText.getLocalBounds();
    endText.setPosition(
        (WINDOW_SIZE - bounds.width) / 2 - bounds.left,
        (WINDOW_SIZE + SCORE_AREA_HEIGHT) / 2 - 90
    );
    window.draw(endText);
    
    sf::Text restartText;
    restartText.setFont(font);
    restartText.setString("R:  Recommencer | M: Menu");
    restartText.setCharacterSize(25);
    restartText.setFillColor(sf::Color(127, 140, 141));
    
    bounds = restartText.getLocalBounds();
    restartText.setPosition(
        (WINDOW_SIZE - bounds.width) / 2 - bounds.left,
        (WINDOW_SIZE + SCORE_AREA_HEIGHT) / 2 + 20
    );
    window.draw(restartText);
}

// Réinitialiser le jeu
void Game::resetGame() {
    for (int i = 0; i < 9; i++) {
        board[i] = 0;
    }
    
    for (int i = 0; i < 3; i++) {
        winningCombo[i] = -1;
    }
    hasWinningLine = false;
    hoveredCell = -1;
    
    gameOver = false;
    winner = 0;
    
    if (lastFirstPlayer != 0) {
        alternateFirstPlayer();
    } else {
        setRandomFirstPlayer();
    }
    
    std::cout << "\nNouvelle partie!\n" << std::endl;
}
