#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "windows.h"
#include "main.h"
#include "game.h"

/* globals */
const int g_screen_width  = 450;
const int g_screen_height = 550;
const int g_screen_bpp    = 32;
const int g_sdl_screen_property = SDL_OPENGL | SDL_HWSURFACE  /*| SDL_FULLSCREEN*/;
SDLKey g_key;
int main( int argc, char* args[] )
{
   
    video_init();
	Game game;

	bool continue_loop = true;
	SDL_Event event;
	
	DWORD previous_tick, tick;
	previous_tick=GetTickCount();

	while(continue_loop){
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					g_key=event.key.keysym.sym;
					if (event.key.keysym.sym == SDLK_ESCAPE)
						continue_loop = false;
				break;

				case SDL_QUIT:
					continue_loop = false;
				break;
			}
		}
		tick = GetTickCount();
		game.update(tick-previous_tick);
		previous_tick = tick;
		game.draw();
		SDL_GL_SwapBuffers();
	}

    video_deinit();
    
    return 0;    
}

void video_init()
{
	SDL_Init( SDL_INIT_VIDEO);
    SDL_SetVideoMode(g_screen_width, g_screen_height, g_screen_bpp, g_sdl_screen_property);
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	glViewport(0, 0, g_screen_width, g_screen_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0,1 /*g_screen_width*/, 1/*g_screen_height*/, 0.0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
    /* wrong DevIL version */
		SDL_Quit();
		return;
	}
	ilInit();

	
	glDisable(GL_LIGHTING);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

}





void video_deinit()
{
	SDL_Quit();
}