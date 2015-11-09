#include "game.h"
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "main.h"
Game::Game()
{	
	float px=1.0f/g_screen_width;
	float py=1.0f/g_screen_height;

	TetrisPanelOptions tpo;
	tpo.width  = 250*px;
	tpo.height = 500*py;
	tpo.x = 25*px;
	tpo.y = 25*py;
	tpo.colnum = 10;
	tpo.rownum = 20;
	tpo.next_blk_x=tpo.x+tpo.width+50*px;
	tpo.next_blk_y=tpo.y+350*py;
	tpo.next_blk_w=75*px;
	tpo.next_blk_h=100*py;

	_ptetris_panel = new TetrisPanel(&tpo);

	_pscore_panel = new ScorePanel(tpo.x+tpo.width+50*px,tpo.y+25*py,75*px,100*py);
	_pscore_panel->set_label("Puan");
	_pscore_panel->set_score(0);
	RGBcolor color;
	color.r=0x33;
	color.g=0x99;
	color.b=0xFF;
	_pscore_panel->set_color(&color);

	_pbackground= new Image("background.jpg");
}
void Game::draw(){
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_pbackground->draw();
	_ptetris_panel->draw();
	_pscore_panel->draw();
	
}
void Game::update(DWORD frame_msecond){
	_ptetris_panel->update(frame_msecond);
	_pscore_panel->set_score(_ptetris_panel->get_score());
}
Game::~Game(){
	delete _ptetris_panel;
	delete _pscore_panel;
}