#ifndef TETRIS_PANEL_H
#define TETRIS_PANEL_H 1
#include "windows.h"
#include <vector>
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "IL/il.h"
using namespace std;
#define MAX_BLOCK_TYPE_NUM 7
#define MAX_BLOCK_WIDTH 4
#define MAX_BLOCK_HEIGHT 4
//MAX_BLOCK_WIDTH and MAX_BLOCK_HEIGHT esit olmali diger durumlar test edilmedi..
struct TetrisPanelOptions{
	float x;
	float y;
	float width;
	float height;
	int colnum;
	int rownum;

	float next_blk_x;
	float next_blk_y;
	float next_blk_w;
	float next_blk_h;
};
struct RGBcolor{
	GLubyte r;
	GLubyte g;
	GLubyte b;
};

enum TetrisState { FALL, NEW_BLOCK, REMOVE_LINES, GAME_OVER};


class Block{
public:
	Block(int type, float block_width, float block_height);
	void set_type(int type);
	void set_origin(float x,float y);
	void draw();
	void draw(float transx, float transy);
	int get_col();
	int get_row();
	void move(int col, int row);
	void move_down();
	void move_left();
	void move_right();
	void rotate_left();
	void rotate_right();
	void set_rotate(int angle);
	bool is_empty_col(int x);
	bool is_empty_row(int y);
	bool is_not_empty(int x,int y);
	void set_color(RGBcolor* color);
	RGBcolor get_color();
	int get_type();
private:
	void calculate_spaces();
	int _space_x,_space_y;
	int _type;
	float _orig_x,_orig_y;
	float _block_width,_block_height;
	int _col,_row;
	int _angle;
	bool _remove_spaces;
	RGBcolor _color;
	static int _blocks[MAX_BLOCK_TYPE_NUM][MAX_BLOCK_HEIGHT][MAX_BLOCK_WIDTH];
};

class TetrisMatrix{
public:
	TetrisMatrix(int colnum, int rownum);
	void paste_block(Block* blk, int x, int y);
	bool is_not_empty(int x,int y);
	void set_matrix(int x,int y); 
	void unset_matrix(int x,int y);
	bool is_non_collision(Block* blk, int x, int y);
	void draw(float x, float y, float block_width, float block_height);
	bool test_lines(vector<int> * lines);
	void unset_line(int y);
	void fall(int y);
	void set_color(int x,int y,RGBcolor* color);
	RGBcolor get_color(int x, int y);
	void change_color_line(int y,float shine);
	void clear();
	~TetrisMatrix();
private:
	int _colnum,_rownum;
	int* _matrix;
	RGBcolor* _colors;
};



class TetrisPanel{
public:
	TetrisPanel(TetrisPanelOptions* tpo);
	~TetrisPanel();
	void start_tetris();
	void draw();
	void update(DWORD frame_msecond);
	long get_score();
private:
	
	void draw_border();
	void game_over();
	int _colnum,  _rownum;
	float _x,_y;
	float _width, _height;
	float _block_width,_block_height;
	float _next_blk_x;
	float _next_blk_y;
	float _next_blk_w;
	float _next_blk_h;
	bool _started;
	DWORD _last_move_time,_last_input_time,_last_remove_time,_last_rotate_time;
	TetrisMatrix* _ptetris_matrix;
	TetrisState _state;
	Block* _pblk,*_pnext_blk;
	long _score;
	vector <int> _full_lines;
};



class TextPanel{
public:
	TextPanel(float x,float y, float w,float h);
	~TextPanel();
	void  set_text(const char *fmt, ...);
	void set_color(RGBcolor* color);
	void set_font(string &font,int font_height);
	RGBcolor get_color();
	void draw_text();
	void set_x(float x);
	void set_y(float y);
	void set_w(float w);
	void set_h(float h);
	float get_x();
	float get_y();
	float get_w();
	float get_h();
private:
	void BuildFont();
	void KillFont();
	float _x,_y,_w,_h;
	string _text;
	string _font;
	GLuint	_base;
	RGBcolor _color;
	int _font_height;
};

class ScorePanel:public TextPanel{
public:
	ScorePanel(float x,float y, float w,float h);
	
	void set_label(string str);
	void set_score(long score);
	void draw();
private:
	void draw_border();
	string _labeltxt;
	long _score;
	
};
class Image {
public:
	Image(string path);
	~Image();
	void set_pos(float x,float y,float w,float h);
	float get_x();
	float get_y();
	float get_w();
	float get_h();
	void draw();
private:
	string _path;
	float _x,_y,_w,_h;
	ILuint _texid;
	GLuint _image;
};

#endif