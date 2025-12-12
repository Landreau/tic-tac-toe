#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>  // *** NOUVEAU ***
#include <vector>

enum GameMode {
    PLAYER_VS_PLAYER,
    PLAYER_VS_AI_EASY,
    PLAYER_VS_AI_HARD
};

class Game {
private: 
    sf::RenderWindow window;
    sf::Font font;
    
    sf::SoundBuffer clickBuffer;
    sf::SoundBuffer winBuffer;
    sf::SoundBuffer drawBuffer;
    sf::Sound clickSound;
    sf::Sound winSound;
    sf::Sound drawSound;
    bool soundsLoaded;
    
    int board[9];
    int currentPlayer;
    bool gameOver;
    int winner;
    
    int scoreX;
    int scoreO;
    int scoreDraw;
    
    int winningCombo[3];
    bool hasWinningLine;
    
    GameMode gameMode;
    bool showModeSelection;
    std::vector<sf::RectangleShape> modeButtons;
    std::vector<sf::Text> modeButtonTexts;
    
    int firstPlayer;
    int lastFirstPlayer;
    
    int hoveredCell;
    int hoveredButton;
    sf::Clock animationClock;
    
    std::vector<sf::RectangleShape> lines;
    
    static const int WINDOW_SIZE = 600;
    static const int CELL_SIZE = 200;
    static const int LINE_THICKNESS = 3;
    static const int SCORE_AREA_HEIGHT = 80;
    
    int checkWinner();
    bool isBoardFull();
    void handleClick(int mouseX, int mouseY);
    void handleKeyPress(sf:: Keyboard::Key key);
    void handleMouseMove(int mouseX, int mouseY);
    
    void loadSounds();
    void playClickSound();
    void playWinSound();
    void playDrawSound();
    
    void setRandomFirstPlayer();
    void alternateFirstPlayer();
    
    void aiMove();
    void aiMoveEasy();
    void aiMoveHard();
    int minimax(int depth, bool isMaximizing);
    
    void drawModeSelection();
    void handleModeSelection(int mouseX, int mouseY);
    void createModeButtons();
    
    void drawGrid();
    void drawSymbols();
    void drawEndScreen();
    void drawScoreBoard();
    void drawWinningLine();
    void drawCurrentPlayerIndicator();
    void drawCellHighlight();
    void createGridLines();
    
    void updateScore();
    
    int getCellFromPosition(int mouseX, int mouseY);
    sf::Color getPlayerColor(int player);

public:
    // Constructeur
    Game();
    
    // Méthode principale
    void run();
    
    // Réinitialisation
    void resetGame();
};

#endif
