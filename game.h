#ifndef GAME_H
#define GAME_H 1
#include "windows.h"
#include "tetris_panel.h"
class Game{
public:
	Game();
	void draw();
	void update(DWORD frame_msecond);
	~Game();
private:
	//int a;
	ScorePanel* _pscore_panel;
	TetrisPanel* _ptetris_panel;
	Image* _pbackground;
	
};

#endif