class Game{

public:
	Game();
	~Game();
	void gameLoop();
private:
	bool quit;  		// boolean to keep game loop running
	int gameState;  	// to keep track of the game's state (0: menu, 1: game)
	int score;  //score
	int miss; 	// miss to 0 and gameover
	int key; 	//key input
	mole * moles [9];
}