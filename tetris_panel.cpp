#include "tetris_panel.h"
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "windows.h"
#include <cstdlib>
#include <ctime> 
#include <math.h>
#include "main.h"
#include "SDL/SDL_syswm.h"
#include <assert.h>
#define MOVE_WAIT_MSECOND 500
#define LEFTRIGHTDOWN_WAIT_MSECOND 50
#define ROTATE_WAIT_MSECOND 100
#define REMOVE_WAIT_MSECOND 1000
#define PI 3.14159265

void draw_rect(float x, float y, float width, float height){
	glBegin(GL_LINE_LOOP);
		glColor3ub(0, 0, 0);
			glVertex2f(x, y);
		glColor3ub(0, 0, 0);
			glVertex2f(x+width,y);
		glColor3ub(0, 0, 0);
			glVertex2f(x+width, y+height);
		glColor3ub(0, 0, 0);
			glVertex2f(x, y+height);
	glEnd();
}
TetrisPanel::TetrisPanel(TetrisPanelOptions* tpo){
	_x		= tpo->x;
	_y		= tpo->y;
	_colnum = tpo->colnum;
	_rownum = tpo->rownum;
	_width  = tpo->width;
	_height = tpo->height;

	_next_blk_x=tpo->next_blk_x;
	_next_blk_y=tpo->next_blk_y;
	_next_blk_w=tpo->next_blk_w;
	_next_blk_h=tpo->next_blk_h;
	
	_block_width  = _width/_colnum;
	_block_height = _height/_rownum;
	
	_last_move_time  =MOVE_WAIT_MSECOND;
	_last_input_time =LEFTRIGHTDOWN_WAIT_MSECOND;
	_last_remove_time=0;
	_last_rotate_time=ROTATE_WAIT_MSECOND;


	_ptetris_matrix = new TetrisMatrix(_colnum, _rownum);
	
	_pblk = new Block(0,_block_width,_block_height);
	_pblk->set_origin(_x,_y);

	RGBcolor color;
	color.r=rand()%192+64;
	color.g=rand()%192+64;
	color.b=rand()%192+64;


	_pnext_blk= new Block(0,_block_width,_block_height);
	_pnext_blk->set_origin(_next_blk_x,_next_blk_y);
	_pnext_blk->set_type(rand()%MAX_BLOCK_TYPE_NUM);
	_pnext_blk->set_color(&color);

	_started = true;
	_state = NEW_BLOCK;
	_score=0;
	srand((unsigned)time(0));
}
long TetrisPanel::get_score(){
	return _score;
}


void TetrisPanel::draw(){
	draw_border();
	if(_state!=REMOVE_LINES)
		_pblk->draw();
	_pnext_blk->draw();
	_ptetris_matrix->draw(_x,_y,_block_width,_block_height);
	if(_state==GAME_OVER)
	{
		TextPanel tp(_x,_y+_height/2,1.0,1.0);
		RGBcolor color; 
		color.r=255;
		color.g=0;
		color.b=0;
		tp.set_font(string("Arial"),12);
		tp.set_color(&color);
		tp.set_text("OYUN BÝTTÝ");
		tp.draw_text();

		tp.set_y(tp.get_y()+0.05);
		tp.set_text("BAÞLATMAK ÝÇÝN S YE BAS");
		tp.draw_text();
		
	}
	
}
void TetrisPanel::draw_border(){
	float borderx=(float)(3.0f/g_screen_width);
	float bordery=(float)(3.0f/g_screen_height);
	draw_rect(_x-borderx,_y-bordery,_width+2*borderx,_height+2*bordery);

	borderx=(float)(20.0f/g_screen_width);
	bordery=(float)(20.0f/g_screen_height);
	draw_rect(_next_blk_x-borderx,_next_blk_y-bordery,_next_blk_w+2*borderx,_next_blk_h+2*bordery);
}
void TetrisPanel::update(DWORD frame_msecond){
	if(g_key==SDLK_p)
	{
		_started=!_started;
		g_key=SDLK_CLEAR;
	}
	if (_state==GAME_OVER)
	{
		if(g_key==SDLK_s)
		{
			_state=NEW_BLOCK;
			_started=true;

			_last_move_time  =MOVE_WAIT_MSECOND;
			_last_input_time =LEFTRIGHTDOWN_WAIT_MSECOND;
			_last_remove_time=0;
			_last_rotate_time=ROTATE_WAIT_MSECOND;

			RGBcolor color;
			color.r=rand()%192+64;
			color.g=rand()%192+64;
			color.b=rand()%192+64;


			_pnext_blk->set_type(rand()%MAX_BLOCK_TYPE_NUM);
			_pnext_blk->set_color(&color);
			
			_ptetris_matrix->clear();
			g_key=SDLK_CLEAR;
			_score=0;
		}
	}
	if(_started)
	{	
		
		bool noncollision;
		if(_last_input_time>=LEFTRIGHTDOWN_WAIT_MSECOND)
		{
			Uint8 * keystate = SDL_GetKeyState(NULL);
			if(keystate[SDLK_LEFT])
			{	//sol tarafi bos ise bu problem yaratir
				
				noncollision = _ptetris_matrix->is_non_collision(_pblk,_pblk->get_col()-1,_pblk->get_row());
				if(noncollision)
					_pblk->move_left();
			} 
			if(keystate[SDLK_RIGHT])
			{	
				
				noncollision =_ptetris_matrix->is_non_collision(_pblk,_pblk->get_col()+1,_pblk->get_row());

				if(noncollision)
					_pblk->move_right();
			}
			
			if(keystate[SDLK_DOWN])
			{	
				_last_move_time = MOVE_WAIT_MSECOND;
			}
			_last_input_time = 0;
		}
		else
		{
			_last_input_time+=frame_msecond;
		}

		if(_last_rotate_time>=ROTATE_WAIT_MSECOND)
		{
			Uint8 * keystate = SDL_GetKeyState(NULL);
			
			if(keystate[SDLK_UP])
			{	
				_pblk->rotate_left();
				noncollision =_ptetris_matrix->is_non_collision(_pblk,_pblk->get_col(),_pblk->get_row());
				if(!noncollision)
					_pblk->rotate_right();
			}
			_last_rotate_time=0;
		}
		else
		{
			_last_rotate_time+=frame_msecond;
		}
		
		if(_last_move_time>=MOVE_WAIT_MSECOND)
		{
			if(_state==NEW_BLOCK)
			{	

				_pblk->set_type(_pnext_blk->get_type());
				_pblk->set_color(&(_pnext_blk->get_color()));
				_pblk->set_rotate(0);
				_pblk->move(_colnum/2-1,0);
				if(_ptetris_matrix->is_non_collision(_pblk,_colnum/2-1,0)){
					_state = FALL;
				}
				else{
					_started=false;
					_state=GAME_OVER;
					game_over();
				}
						RGBcolor color;
						color.r=rand()%192+64;
						color.g=rand()%192+64;
						color.b=rand()%192+64;
						_pnext_blk->set_type(rand()%MAX_BLOCK_TYPE_NUM);
						_pnext_blk->set_color(&color);

			
			}
			else if(_state==FALL)
			{	
				noncollision =_ptetris_matrix->is_non_collision(_pblk,_pblk->get_col(),_pblk->get_row()+1);

				if (noncollision)
				{
					_pblk->move_down();
				}
				else
				{
					_ptetris_matrix->paste_block(_pblk,_pblk->get_col(),_pblk->get_row());
					_full_lines.clear();
					if(_ptetris_matrix->test_lines(&_full_lines))
					{
						_state = REMOVE_LINES;
						_last_remove_time = 0;
					}
					else
					{
						_state = NEW_BLOCK;

					}
				}
			}
			_last_move_time=0;
		}
		else
		{
			_last_move_time+=frame_msecond;
		}

		if (_state==REMOVE_LINES)
		{	
			if (_last_remove_time>=REMOVE_WAIT_MSECOND)
			{
					
				_state=NEW_BLOCK;
				//_full_lines daki satir numaralari kucukten buyuge sirali olmali yoksa burasi duzgun calismaz
				vector<int>::iterator iter;
				for( iter = _full_lines.begin(); iter != _full_lines.end(); iter++ ) 
				{	_score+=10;
					_ptetris_matrix->unset_line(*iter);
					_ptetris_matrix->fall(*iter);
				}
				//satirlar silindikten sonra blogun eski yerdeki cizimini onleyip yeni blogu hemen yerine koymak icin
				update(frame_msecond+MOVE_WAIT_MSECOND);

			}
			else
			{
				vector<int>::iterator iter;
				for( iter = _full_lines.begin(); iter != _full_lines.end(); iter++ ) 
				{
					_ptetris_matrix->change_color_line(*iter,(float)_last_remove_time/REMOVE_WAIT_MSECOND);
				}

			}
			
			_last_remove_time+=frame_msecond;
		}

	}
}

void TetrisPanel::start_tetris(){
	_started=true;
}
void TetrisPanel::game_over(){
	//OutputDebugString("oyun bitti");
}

TetrisPanel::~TetrisPanel(){
	delete _ptetris_matrix;
	delete _pblk;
}

TetrisMatrix::TetrisMatrix(int colnum, int rownum){
	_colnum = colnum;
	_rownum = rownum;
	_matrix = new int[_rownum * _colnum];
	_colors = new RGBcolor[_rownum * _colnum];
	for(int i=0; i<_rownum*_colnum; i++)
	{
		_matrix[i]=0;
		_colors[i].r=0;
		_colors[i].g=0;
		_colors[i].b=0;

	}
}
TetrisMatrix::~TetrisMatrix(){
	delete[] _matrix;
	delete[] _colors;
}
void TetrisMatrix::set_color(int x,int y,RGBcolor* color){
	_colors[y*_colnum+x]= *color;
}
RGBcolor TetrisMatrix::get_color(int x, int y){
	return _colors[y*_colnum+x];
}
void TetrisMatrix::fall(int y){		

	for(int uppery=y;uppery>=1;uppery--)
	{
		for(int x2=0;x2<_colnum;x2++)
		{
			_matrix[uppery*_colnum+x2]=_matrix[(uppery-1)*_colnum+x2];
			set_color(x2,uppery,&(get_color(x2,uppery-1)));
		}
	}
		

	

}
void TetrisMatrix::change_color_line(int y,float shine){
	if (shine>1.0f) shine=1;
	if (shine<0.0f) shine=0.0;

	for(int x=0;x<_colnum;x++)
	{	RGBcolor color;
		color = get_color(x,y);
		color.r=color.r+(255-color.r)*(shine);
		color.g=color.g+(255-color.g)*(shine);
		color.b=color.b+(255-color.b)*(shine);
		set_color(x,y,&color);
	}
}
void TetrisMatrix::clear(){
	for(int y=0; y<_rownum; y++)
	for(int x=0; x<_colnum; x++)
	{
		unset_matrix(x,y);
	}
}
void TetrisMatrix::unset_line(int y){
	for(int x=0; x<_colnum; x++)
	{
		unset_matrix(x,y);
	}
		
}

bool TetrisMatrix::test_lines(vector<int> * lines){
	bool full = true;
	bool have_full=false;
	for(int i=0;i<_rownum;i++)
	{
		full = true;
		for(int j=0;j<_colnum;j++)
		{
			if (!_matrix[i*_colnum+j])
			{
				full=false;
				break;
			}

		}
		if (full)
		{
			lines->push_back(i);
			have_full=true;
		}
	}
	return have_full;

}
void TetrisMatrix::draw(float x, float y, float block_width, float block_height){
	glPushMatrix();
	glTranslatef(x,y,0);
	glScalef(block_width,block_height,1);
	for(int row=0; row<_rownum; row++){
		  for(int col=0; col<_colnum; col++){
			  if (is_not_empty(col,row)){
				glPushMatrix();
				glTranslated(col,row,0);
				glBegin(GL_QUADS);
					glColor3ub(get_color(col,row).r, get_color(col,row).g, get_color(col,row).b);
						glVertex2d(0, 0);
					glColor3ub(0, get_color(col,row).g, get_color(col,row).b);
						glVertex2d(1,0);
					glColor3ub(get_color(col,row).r, 0, get_color(col,row).b);
						glVertex2d(1, 1);
					glColor3ub(get_color(col,row).r, get_color(col,row).g, 0);
						glVertex2d(0, 1);
				glEnd();
				glPopMatrix();
			  }
		  }
	}
	glPopMatrix();
		

}
bool TetrisMatrix::is_not_empty(int x,int y){
	if (_matrix[y*_colnum + x])
		return true;
	else
		return false;
}

void TetrisMatrix::set_matrix(int x,int y){
	_matrix[y*_colnum + x]=1;
}
void TetrisMatrix::unset_matrix(int x,int y){
	_matrix[y*_colnum + x]=0;
}
bool TetrisMatrix::is_non_collision(Block *blk, int x, int y){
	
	for(int row=0;row<MAX_BLOCK_HEIGHT;row++)
	{
		for(int col=0;col<MAX_BLOCK_WIDTH;col++)
		{	if ((x+col>=_colnum)||(y+row>=_rownum)||(x+col<0)||(y+row<0))
			{
				if(blk->is_not_empty(col,row))
					return false;
				else
					continue;
			}
			if (blk->is_not_empty(col,row)&& is_not_empty(x+col,y+row))
				return false;
		}
	}
	return true;
}
void TetrisMatrix::paste_block(Block* blk, int x, int y){
	for(int row=0;row<MAX_BLOCK_HEIGHT;row++)
	{
		for(int col=0;col<MAX_BLOCK_WIDTH;col++)
		{	if ((x+col>=_colnum)||(y+row>=_rownum)||(x+col<0)||(y+row<0))
				continue;

			if (blk->is_not_empty(col,row))
			{
				set_matrix(x+col,y+row);
				set_color(x+col,y+row,&(blk->get_color()));
			}
		}
	}
}

ScorePanel::ScorePanel(float x, float y, float w, float h):TextPanel(x,y,w,h){
	_labeltxt=" ";
	_score=0;
}

void ScorePanel::draw_border(){
	float borderx=(float)(20.0f/g_screen_width);
	float bordery=(float)(20.0f/g_screen_height);
	draw_rect(get_x()-borderx,get_y()-bordery,get_w()+2*borderx,get_h()+2*bordery);
	
}
void ScorePanel::draw(){
	draw_border();
	
	set_text(_labeltxt.c_str());
	draw_text();

	set_text("%ld",_score);
	float y=get_y();
	set_y(y+0.05);
	draw_text();

	set_y(y);
	
}
void ScorePanel::set_label(string str){
	_labeltxt=str;
}
void ScorePanel::set_score(long score){
	_score=score;
}




TextPanel::TextPanel(float x,float y, float w,float h){
	_x=x;
	_y=y;
	_w=w;
	_h=h;
	_font="Courier New";
	_text=" ";
	_color.r=255;
	_color.g=255;
	_color.b=255;
	_font_height=24;
	BuildFont();
}
TextPanel::~TextPanel(){
	KillFont();
}
void TextPanel::set_text(const char *fmt, ...){
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);			
		string str(text);
		_text=text;
}
void TextPanel::set_color(RGBcolor* color){
	_color=*color;
}
void TextPanel::set_font(string& font, int font_height){
	KillFont();
	_font=font;
	_font_height=font_height;
	BuildFont();

}
void TextPanel::set_x(float x){
	_x=x;
}
void TextPanel::set_y(float y){
	_y=y;
}
void TextPanel::set_w(float w){
	_w=w;
}
void TextPanel::set_h(float h){
	_h=h;
}
float TextPanel::get_x(){
	return _x;
}
float TextPanel::get_y(){
	return _y;
}
float TextPanel::get_w(){
	return _w;
}
float TextPanel::get_h(){
	return _h;
}

void TextPanel::BuildFont()								// Build Our Bitmap Font
{


	SDL_SysWMinfo pInfo;


	SDL_VERSION(&pInfo.version);
	SDL_GetWMInfo(&pInfo); 


	HWND myHwnd = pInfo.window;
	HDC hDC = GetDC(myHwnd);


	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping

	_base = glGenLists(256);								// Storage For 96 Characters
	assert(_base!=0);
	font = CreateFont(	-_font_height,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						TURKISH_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH ,		// Family And Pitch
						_font.c_str());					// Font Name
	assert(font!=NULL);
	oldfont = (HFONT)SelectObject(hDC, font);           // Selects The Font We Want
	wglUseFontBitmaps(hDC, 0, 256, _base);				// Builds 96 Characters Starting At Character 32
	SelectObject(hDC, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
}
void TextPanel::KillFont()									// Delete The Font List
{
	glDeleteLists(_base, 256);							// Delete All 96 Characters
}



RGBcolor TextPanel::get_color(){
	return _color;
}
void TextPanel::draw_text(){
	glPushMatrix();

	glColor3ub(_color.r,_color.g,_color.b);
	glTranslatef(0,0,0);
	glRasterPos2f(_x,_y);

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(_base - 0);								// Sets The Base Character to 32
	glCallLists(_text.length(), GL_UNSIGNED_BYTE, _text.c_str());	// Draws The Display List Text
	glPopAttrib();

	glPopMatrix();
}

Image::Image(string path){

	ILboolean success;
	set_pos(0.0f,0.0f,1.0f,1.0f);
	ilGenImages(1, &_texid); /* Generation of one image name */
	ilBindImage(_texid); /* Binding of image name */
	success = ilLoadImage(path.c_str()); /* Loading of image "image.jpg" */
	assert(success);
	success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	assert(success);
	glGenTextures(1, &_image);
	glBindTexture(GL_TEXTURE_2D,_image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear
      interpolation for magnification filter */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear
      interpolation for minifying filter */
    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
      ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
      ilGetData()); 
	ilDeleteImages(1, &_texid);
}
Image::~Image(){
	glDeleteTextures(1, &_image);
}
void Image::set_pos(float x,float y,float w,float h){
	_x=x;
	_y=y;
	_w=w;
	_h=h;
}

float Image::get_x(){
	return _x;
}
float Image::get_y(){
	return _y;
}
float Image::get_w(){
	return _w;
}
float Image::get_h(){
	return _h;
}
void Image::draw(){
glColor3ub(255,255,255);
glEnable(GL_TEXTURE_2D);
glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(_x, _y);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(_x,_y+_h);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(_x+_w,_y+_h);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(_x+_w,_y);
 glEnd();
 glDisable(GL_TEXTURE_2D);
}

Block::Block(int type, float block_width, float block_height){
	if(_type<MAX_BLOCK_TYPE_NUM)
		_type = type;
	else
		_type = 0;

	_block_width  = block_width;
	_block_height = block_height;
	_orig_x = 0;
	_orig_y = 0;
	_col = 0;
	_row = 0;
	_angle = 0;
	_space_x=0;
	_space_y=0;
	_remove_spaces=false;
}
int Block::get_type(){
	return _type;
}
void Block::set_color(RGBcolor* color){
	_color=*color;
}
RGBcolor Block::get_color(){
	return _color;
}
void Block::rotate_left(){
	_remove_spaces=false;
	_angle+=90;
	if(_angle==360) _angle=0;
	calculate_spaces();
	_remove_spaces=true;	
}
void Block::rotate_right(){
	_remove_spaces=false;
	_angle-=90;
	if(_angle==-90) _angle=270;
	calculate_spaces();
	_remove_spaces=true;
	
}
void Block::calculate_spaces(){
	_space_x=0;
	_space_y=0;
	for(int x=0;x<MAX_BLOCK_WIDTH;x++)
		if(is_empty_col(x)) 
			_space_x++;
		else
			break;
		
	
	for(int y=0;y<MAX_BLOCK_HEIGHT;y++)
		if(is_empty_row(y)) 
			_space_y++;
		else
			break;

}

void Block::set_rotate(int angle){
	_remove_spaces=false;
	_angle=angle;
	calculate_spaces();
	_remove_spaces=true;
}
bool Block::is_not_empty(int x,int y){
	//return _blocks[_type][y][x];
	int rx=x,ry=y;
	if(_remove_spaces)
	{
		x+=_space_x;
		y+=_space_y;
	}

	switch(_angle){
		case 0:
			rx=x;
			ry=y;
			break;
		case 90:
			rx=y;
			ry=MAX_BLOCK_WIDTH-1-x;
			break;
		case 180:
			ry=MAX_BLOCK_HEIGHT-1-y;
			rx=MAX_BLOCK_WIDTH-1-x;
			break;
		case 270:
			rx=MAX_BLOCK_HEIGHT-1-y;
			ry=x;
			break;
	}
	
	
	if ((rx>=MAX_BLOCK_WIDTH)||(ry>=MAX_BLOCK_HEIGHT)||(rx<0)||(ry<0)) return false;
	
	if(_blocks[_type][ry][rx])
		return true;
	else
		return false;
}

bool Block::is_empty_col(int x){
	if ((x>=MAX_BLOCK_HEIGHT)||(x<0)) 
		return true;
	for(int i=0;i<MAX_BLOCK_HEIGHT;i++)
		if (is_not_empty(x,i))
			return false;
	return true;

}
bool Block::is_empty_row(int y){
	if ((y>=MAX_BLOCK_WIDTH)||(y<0)) 
		return true;
	for(int i=0;i<MAX_BLOCK_WIDTH;i++)
		if (is_not_empty(i,y))
			return false;
	return true;

}
int Block::get_col(){
	return _col;
}
int Block::get_row(){
	return _row;
}
void Block::set_type(int type){
	_type = type;
}
void Block::move(int col, int row){
	_col = col;
	_row = row;
}
void Block::move_down(){
	_row++;
}
void Block::move_left(){
	_col--;
}
void Block::move_right(){
	_col++;
}
void Block::set_origin(float x,float y){
	_orig_x = x;
	_orig_y = y;
}
void Block::draw(){
	draw(_orig_x+_block_width*_col,_orig_y+_block_height*_row);
}

void Block::draw(float transx, float transy){
	glPushMatrix();
	glTranslatef(transx,transy,0);
	glScalef(_block_width,_block_height,1);
	for(int x=0;x<MAX_BLOCK_WIDTH;x++){
		  for(int y=0;y<MAX_BLOCK_HEIGHT;y++){
			  if (is_not_empty(x,y)){
				glPushMatrix();
				glTranslated(x,y,0);
				glBegin(GL_QUADS);
					glColor3ub(_color.r, _color.g, _color.b);
						glVertex2d(0, 0);
					glColor3ub(0, _color.g, _color.b);
						glVertex2d(1,0);
					glColor3ub(_color.r, 0, _color.b);
						glVertex2d(1, 1);
					glColor3ub(_color.r, _color.g, 0);
						glVertex2d(0, 1);
				glEnd();
				glPopMatrix();
			  }
		  }
	}
	glPopMatrix();
}

int Block::_blocks[MAX_BLOCK_TYPE_NUM][MAX_BLOCK_HEIGHT][MAX_BLOCK_WIDTH]=
					{1,0,0,0,/*I*/
					 1,0,0,0,
					 1,0,0,0,
					 1,0,0,0,
					 0,1,0,0,/*J*/
				     0,1,0,0,
				     1,1,0,0,
				     0,0,0,0,
					 1,0,0,0,/*L*/
				     1,0,0,0,
				     1,1,0,0,
				     0,0,0,0,
					 1,1,0,0,/*O*/
				     1,1,0,0,
				     0,0,0,0,
				     0,0,0,0,
					 0,1,1,0,/*S*/
				     1,1,0,0,
				     0,0,0,0,
				     0,0,0,0,
					 0,1,0,0,/*T*/
				     1,1,1,0,
				     0,0,0,0,
				     0,0,0,0,
					 1,1,0,0,/*Z*/
				     0,1,1,0,
				     0,0,0,0,
					 0,0,0,0};