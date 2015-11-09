#ifndef MAIN_H
#define MAIN_H 1
extern SDLKey g_key;
extern const int g_screen_width;
extern const int g_screen_height;
extern const int g_screen_bpp;
extern const int g_sdl_screen_property;

void video_init();
void video_deinit();


#endif