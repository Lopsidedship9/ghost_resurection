#include "game.h"
#include "utils.h"
#include "input.h"
#include "image.h"
#include "movement.h"
#include "collectables.h"
#include "interactables.h"
#include "screens.h"
#include "audio_story.h"
#include "game_ending.h"
#include "ghost_ai.h"

#include <cmath>
#include <algorithm>

Game* Game::instance = NULL;

Image font;
Image minifont;
Image sprite;
Image menu_play_image;
Image menu_exit_image;
Image control_tutorial_image;
Image game_over_image;
Image win_image;
Color bgcolor(130, 80, 100);

// --- Mapa (Tiled) ---
Image tileset;                 // data/tilemap_packed.png (192x176, 12 columnas de tiles de 16x16)
const int TILESET_COLUMNS = 49;
GameMap* world_map = NULL;           // todas las capas del .tmj (Suelo, Edificios, personajes)
bool tileset_ok = false;
float cam_x = 0, cam_y = 0;    // camara en pixeles del mundo


// --- Jugador e inventario ---
int player_x = 18;
int player_y = 22;
PlayerInventory player_inventory;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	this->renderer = NULL;
	instance = this;
	must_exit = false;
	game_started = false;
	story_playing = false;
	audio_ok = false;
	selected_menu_option = 0;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;

	font.load("data/bitmap-font-white.tga"); //load bitmap-font image
	minifont.load("data/mini-font-white-4x6.tga"); //load bitmap-font image
	sprite.load("data/spritesheet.tga"); //example to load an sprite
	menu_play_image.loadTGA("data/menu_play_160x120.tga");
	menu_exit_image.loadTGA("data/menu_exit_160x120.tga");
	control_tutorial_image.loadTGA("data/tutorial_160x120.tga");

	if (!win_image.loadTGA("data/victory_160x120.tga"))
		std::cerr << "ERROR: no se encuentra data/win.tga" << std::endl;
	
	if (!game_over_image.loadTGA("data/gameover_160x120.tga"))
		std::cerr << "ERROR: no se encuentra data/death.tga" << std::endl;

	// Cargar la narracion en memoria al iniciar el juego.
	if (!loadStoryAudio("data/historia.wav"))
		std::cerr << "La partida continuara sin narracion." << std::endl;

	// cargar tileset y mapa (todas las capas del .tmj)
	tileset_ok = tileset.load("data/colored_packed.png");
	if (!tileset_ok)
		std::cerr << "ERROR: no se encuentra data/colored_packed.png" << std::endl;
	world_map = loadGameMap("data/mapilla.tmj");
	if (!world_map)
		std::cerr << "ERROR: no se encuentra data/mapilla.tmj" << std::endl;
	else
		std::cout << " + Mapa cargado: " << world_map->numLayers << " capas" << std::endl;

	initGhostAI(2, 1);

	//enableAudio(); //enable this line if you plan to add audio to your application
	//synth.playSample("data/coin.wav",1,true);
	//synth.osc1.amplitude = 0.5;
}


//what to do when the image has to be draw
void Game::render(void)
{
	//Create a new Image (or we could create a global one if we want to keep the previous frame)
	Image framebuffer(160, 120); //do not change framebuffer size

	framebuffer.fill( bgcolor );

	if (isGameEndingActive())
	{
		framebuffer.drawImage(win_image, 0, 0);
		showFramebuffer(&framebuffer);
		return;
	}

	// Si el fantasma alcanza al jugador, mostrar la pantalla de muerte.
	if (isGhostGameOver())
	{
		framebuffer.drawImage(game_over_image, 0, 0);
		showFramebuffer(&framebuffer);
		return;
	}

	//Si la historia se esta reproduciendo, mostrar la pantalla de tutorial y no actualizar el mundo.
	if (story_playing)
	{
		framebuffer.drawImage(control_tutorial_image, 0, 0);
		showFramebuffer(&framebuffer);
		return;
	}


	// Antes de empezar la partida, mostrar el menu de inicio.
	if (!game_started)
	{
		renderStartMenu(framebuffer, menu_play_image, menu_exit_image, selected_menu_option);
		showFramebuffer(&framebuffer);
		return;
	}

	// avisos en pantalla si falta algo
	if (!tileset_ok)
		framebuffer.drawText("Falta data/tilemap_packed.png", 2, 2, minifont, 4, 6);
	if (!world_map)
		framebuffer.drawText("Falta data/mapa.tmj", 2, 12, minifont, 4, 6);

	// dibujar las capas del mapa en orden (suelo -> edificios -> personajes)
	if (world_map && tileset_ok)
		world_map->render(framebuffer, tileset, TILESET_COLUMNS, (int)cam_x, (int)cam_y);

	//send image to screen
	showFramebuffer(&framebuffer);
}

void Game::update(double seconds_elapsed)
{
	// No seguir actualizando el mundo tras completar el juego.
	if (isGameEndingActive())
		return;
	if (isGhostGameOver())
		return;
	// Mientras se reproduce la historia, esperar a que termine.
	if (story_playing)
	{
		if (!isStoryAudioPlaying())
		{
			story_playing = false;
			game_started = true;
			std::cout << "Narracion terminada. Empieza la partida." << std::endl;
		}
		return;
	}

	// No actualizar el mundo mientras se muestra el menu de inicio.
	if (!game_started)
		return;

	// Leer una dirección y delegar el movimiento al módulo movement.cpp.
	MoveDirection direction = MoveDirection::NONE;
	if (Input::wasKeyPressed(SDL_SCANCODE_UP))
		direction = MoveDirection::UP;
	else if (Input::wasKeyPressed(SDL_SCANCODE_DOWN))
		direction = MoveDirection::DOWN;
	else if (Input::wasKeyPressed(SDL_SCANCODE_LEFT))
		direction = MoveDirection::LEFT;
	else if (Input::wasKeyPressed(SDL_SCANCODE_RIGHT))
		direction = MoveDirection::RIGHT;

	movePlayer(world_map, player_x, player_y, direction, !must_exit);

	updateGhostAI(world_map, player_x, player_y, seconds_elapsed);

	if (isGhostGameOver())
		return;

	if (Input::wasKeyPressed(SDL_SCANCODE_Z) ||
		Input::wasKeyPressed(SDL_SCANCODE_X))
	{
		// Primero comprobar si la interaccion activa la casilla final.
		if (tryActivateGameEnding(world_map, player_x, player_y,
			player_inventory.hearts_collected))
		{
			std::cout << "Juego completado!" << std::endl;
			return;
		}
		//std::cout << "Funcion de interaccion llamada" << std::endl;
		interactWithNearbyObject(world_map, player_x, player_y);
	}

	// Si el jugador está sobre un objeto recogible, se añade al inventario.
	if (collectObjectAt(world_map, player_x, player_y, player_inventory))
	{
		std::cout << "Objeto recogido. Corazones: "
			<< player_inventory.hearts_collected << std::endl;
	}

	/*
	if (world_map)
	{
		float max_x = world_map->width * world_map->tile_width - 160;
		float max_y = world_map->height * world_map->tile_height - 120;
		if (cam_x < 0) cam_x = 0;
		if (cam_y < 0) cam_y = 0;
		if (cam_x > max_x) cam_x = max_x;
		if (cam_y > max_y) cam_y = max_y;
	}*/

	//Read the keyboard state, to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	// mover la camara por el mapa con las flechas

	// La camara sigue al jugador: lo mantiene centrado siempre que los
	// limites del mapa lo permitan. El framebuffer mide 160x120 pixeles.
	if (world_map) {
		const float screen_center_x = 160.0f * 0.5f;
		const float screen_center_y = 120.0f * 0.5f;

		// Centro de la casilla del jugador, convertido a pixeles del mundo.
		cam_x = player_x * world_map->tile_width + world_map->tile_width * 0.5f - screen_center_x;
		cam_y = player_y * world_map->tile_height + world_map->tile_height * 0.5f - screen_center_y;

		// Limitar la camara al tamano del mapa. max() evita valores negativos
		// cuando el mapa es mas pequeno que la pantalla.
		float max_x = std::max(0.0f, world_map->width * world_map->tile_width - 160.0f);
		float max_y = std::max(0.0f, world_map->height * world_map->tile_height - 120.0f);
		cam_x = std::max(0.0f, std::min(cam_x, max_x));
		cam_y = std::max(0.0f, std::min(cam_y, max_y));
	}

	//example of 'was pressed'
	if (Input::wasKeyPressed(SDL_SCANCODE_A)) //if key A was pressed
	{
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_Z)) //if key Z was pressed
	{
	}

	//to read the gamepad state
	if (Input::gamepads[0].isButtonPressed(A_BUTTON)) //if the A button is pressed
	{
	}

	if (Input::gamepads[0].direction & PAD_UP) //left stick pointing up
	{
		bgcolor.set(0, 255, 0);
	}
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	// Durante la narracion no permitir que se vuelva a seleccionar JUGAR.
	if (story_playing)
	{
		if (event.keysym.sym == SDLK_x)
		{
			stopStoryAudio();
			story_playing = false;
			game_started = true;

			std::cout << "Narracion interrumpida. Empieza la partida."
				<< std::endl;
		}
		
		if (event.keysym.sym == SDLK_ESCAPE) {
			unloadStoryAudio();
			story_playing = false;
			must_exit = true;
		}
		return;
	}

	// Si se muestra la pantalla final, X vuelve al menu de inicio.
	if (isGameEndingActive())
	{
		if (event.keysym.sym == SDLK_x)
		{
			resetGameEnding();
			game_started = false;
			story_playing = false;
			resetGhostAI();
			selected_menu_option = 0;
			std::cout << "Volviendo al menu de inicio." << std::endl;
		}
		return; // Ignorar las demas teclas mientras se muestra el final.
	}
	
	if (isGhostGameOver())
	{
		if (event.type == SDL_KEYDOWN &&
			event.keysym.sym == SDLK_x)
		{
			resetGhostAI();
			resetGameEnding();
			game_started = false;
			story_playing = true;
			selected_menu_option = 0;
		}
	}
	
	if (!game_started)
	{
		StartMenuAction action = handleStartMenuKey(event.keysym.sym, selected_menu_option);
		if (action == START_MENU_PLAY)
		{
			if (playStoryAudio())
			{
				story_playing = true;
			}
			else
			{
				// Si falta el archivo o no se puede abrir, dejar jugar igualmente.
				std::cerr << "Se iniciara la partida sin narracion." << std::endl;
				game_started = true;
			}
		}
		else if (action == START_MENU_EXIT)
		{
			unloadStoryAudio();
			must_exit = true;
		}
		return;
	}
	
	if (event.keysym.sym == SDLK_ESCAPE)
	{
		unloadStoryAudio();
		must_exit = true;
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseMove(SDL_MouseMotionEvent event)
{
}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	#ifdef USE_OPENGL
		glViewport( 0,0, width, height );
	#endif
	window_width = width;
	window_height = height;
}

//sends the image to the framebuffer of the GPU
void Game::showFramebuffer(Image* img)
{
	float startx = -1.0; float starty = -1.0;
	float width = 2.0; float height = 2.0;

	//center in window
	float real_aspect = window_width / (float)window_height;
	float desired_aspect = img->width / (float)img->height;
	float diff = desired_aspect / real_aspect;
	width *= diff;
	startx = -diff;

#ifdef USE_OPENGL
	static GLuint texture_id = -1;
	static GLuint shader_id = -1;
	if (!texture_id)
		glGenTextures(1, &texture_id);

	//upload as texture
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
	glDisable(GL_CULL_FACE); glDisable(GL_DEPTH_TEST); glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex2f(startx, starty + height);
	glTexCoord2f(1.0, 0.0); glVertex2f(startx + width, starty + height);
	glTexCoord2f(1.0, 1.0); glVertex2f(startx + width, starty);
	glTexCoord2f(0.0, 1.0); glVertex2f(startx, starty);
	glEnd();
#else
	static SDL_Texture* texture = NULL;
	static int tex_width = 0;
	static int tex_height = 0;
	if (!texture || tex_width != img->width || tex_height != img->height)
	{
		if(texture)
			SDL_DestroyTexture(texture);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR888, SDL_TEXTUREACCESS_TARGET, img->width, img->height);
		tex_width = img->width;
		tex_height = img->height;
	}

	//SDL_RenderClear(renderer);
	SDL_Rect rect = { 0, 0, (int)img->width, (int)img->height };
	SDL_UpdateTexture(texture, &rect, img->pixels, img->width*4);
	SDL_RenderCopy(renderer, texture, NULL, NULL);

#endif
	/* this version resizes the image which is slower
	Image resized = *img;
	//resized.quantize(1); //change this line to have a more retro look
	resized.scale(window_width, window_height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (1) //flip
	{
	glRasterPos2f(-1, 1);
	glPixelZoom(1, -1);
	}
	glDrawPixels( resized.width, resized.height, GL_RGBA, GL_UNSIGNED_BYTE, resized.pixels );
	*/
}

//AUDIO STUFF ********************

SDL_AudioSpec audio_spec;

void AudioCallback(void*  userdata,
	Uint8* stream,
	int    len)
{
	static double audio_time = 0;

	memset(stream, 0, len);//clear
	if (!Game::instance)
		return;

	Game::instance->onAudio((float*)stream, len / sizeof(float), audio_time, audio_spec);
	audio_time += len / (double)audio_spec.freq;
}

void Game::enableAudio()
{
	SDL_memset(&audio_spec, 0, sizeof(audio_spec)); /* or SDL_zero(want) */
	audio_spec.freq = 48000;
	audio_spec.format = AUDIO_F32;
	audio_spec.channels = 1;
	audio_spec.samples = 1024;
	audio_spec.callback = AudioCallback; /* you wrote this function elsewhere. */
	SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(nullptr, 0, &audio_spec, nullptr, 0);
	if (!audio_device) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}
	SDL_PauseAudioDevice(audio_device, 0);
}

void Game::onAudio(float *buffer, unsigned int len, double time, SDL_AudioSpec& audio_spec)
{
	//fill the audio buffer using our custom retro synth
	synth.generateAudio(buffer, len, audio_spec);
}
