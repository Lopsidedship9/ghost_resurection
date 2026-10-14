#ifndef GAME_H
#define GAME_H

#include "includes.h"
#include "image.h"
#include "utils.h"
#include "synth.h"
#include "map.h"

class Game
{
public:
	static Game* instance;

	//window
	SDL_Window* window;
	SDL_Renderer* renderer;
	int window_width;
	int window_height;

	//some globals
	long frame;
    float time;
	float elapsed_time;
	int fps;
	bool must_exit;
	bool game_started;
	int selected_menu_option;
	

	//audio
	Synth synth;

	//ctor
	Game( int window_width, int window_height, SDL_Window* window );

	//main functions
	void render( void );
	void update( double dt );

	void showFramebuffer(Image* img);

	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseMove(SDL_MouseMotionEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onResize(int width, int height);

	//audio stuff
	void enableAudio(); //opens audio channel to play sound
	void onAudio(float* buffer, unsigned int len, double time, SDL_AudioSpec &audio_spec); //called constantly to fill the audio buffer
};


#endif 