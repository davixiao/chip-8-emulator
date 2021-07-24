/**
 * Credit to Austin Morlan for Graphics Platform code
 * https://austinmorlan.com/posts/chip8_emulator/#the-platform-layer
 */
#include <SDL2/SDL.h>
#include "Platform.h"

constexpr uint8_t keymap[16] = {
  SDLK_x,
  SDLK_1,
  SDLK_2,
  SDLK_3,
  SDLK_q,
  SDLK_w,
  SDLK_e,
  SDLK_a,
  SDLK_s,
  SDLK_d,
  SDLK_z,
  SDLK_c,
  SDLK_4,
  SDLK_r,
  SDLK_f,
  SDLK_v,
};

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	texture = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
}

Platform::~Platform() {
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch) {
	SDL_UpdateTexture(texture, nullptr, buffer, pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys) {
	bool quit = false;

	SDL_Event event;
  while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT: {
				quit = true;
        break;
			} 
			case SDL_KEYDOWN: {
        if (event.key.keysym.sym == SDLK_ESCAPE) 
          quit = true;
        else {
          for (unsigned int i = 0; i < 16; ++i) {
            if (event.key.keysym.sym == keymap[i]) {
              keys[i] = 1;
            }
          }
        }
        break;
      }
			case SDL_KEYUP: {
        for (unsigned int i = 0; i < 16; ++i) {
          if (event.key.keysym.sym == keymap[i]) {
            keys[i] = 0;
          }
        }
        break;
      }
		}
	}
	return quit;
}
