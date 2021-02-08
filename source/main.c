#include <SDL2/SDL.h>
#include "utils.hpp"

u8 handle_keyboard(SDL_Event *event)
{
	u8 result = 1;
	if (event->type == SDL_QUIT)
	{
		result = 0;
	}

	if (event->type == SDL_KEYDOWN)
	{
		if (event->key.keysym.sym == SDLK_ESCAPE)
		{
			result = 0;
		}
	}

	return result;
}

void draw_colored_rectangle(u32* pixels, i32 window_width, i32 window_height, i32 x_center, i32 y_center, 
		i32 width, i32 height, u32 color)
{
	i32 x_start = x_center - width / 2;
	i32 y_start = y_center - height / 2; 
	if (x_start + width < 0 || x_start >= window_width) return; 
	if (y_start + height < 0 || y_start >= window_height) return; 

	if (x_start < 0)
	{
		width -= -x_start;
		x_start = 0;
	}
	if (y_start < 0)
	{
		height -= -y_start;
		y_start = 0;
	}

	
	u32 *pixel_start = pixels + y_start * window_width + x_start;
	for (i32 y = y_start; y < y_start + height; ++y)
	{
		if (y >= window_height) break;

		u32 *pixel = pixel_start;
		for (i32 x = x_start; x < x_start + width; ++x)
		{
			if (x >= window_width) break;

			*pixel++ = color;
		}
		pixel_start += window_width;
	}
}


void draw_to_pixels(u32 *pixels, u32 window_width, u32 window_height,
		float *data, u32 n_data)
{
	//NOTE(stanisz): find the maximum among the data
	float max_data = -1;

	for (u32 i = 0; i < n_data; ++i)
	{
		float this_data = data[i];
		if ( this_data > max_data )
		{
			max_data = this_data;
		}
	}

	u32 bar_width = (float)window_width / n_data;
	u32 width_offset = bar_width / 2;

	for (u32 i = 0; i < n_data; ++i)
	{
		float this_data = data[i];

		draw_colored_rectangle(pixels, window_width, window_height, 
				(i+1) * 40, (i+1) * 90, 60, 80, 0xff000000);
	}
}


int main(int argc, char **argv)
{
	const u32 window_width = 1280, window_height = 720;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("linvis", 
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			window_width, window_height, 0);

	UNUSED(argc);
	UNUSED(argv);

	u8 is_running = 1;

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (!renderer)
		printf("%s\n", SDL_GetError());

	SDL_Texture *screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888,
			SDL_TEXTUREACCESS_STATIC, window_width, window_height);
	if (!screen_texture)
		printf("%s\n", SDL_GetError());
		

	u32 *pixels = (u32*)malloc(sizeof(u32) * window_width * window_height);

	u64 last_time = SDL_GetPerformanceCounter();
	float delta_time = 0;
	
	while (is_running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			is_running = handle_keyboard(&event);
		}

		memset(pixels, 0xffffffff, window_width * window_height * sizeof(u32));

		float data[] = {0.4234f, 0.123f, 0.372f, 0.772f, 0.123f};
		draw_to_pixels(pixels, window_width, window_height, data, 5);

		SDL_UpdateTexture(screen_texture, NULL, pixels, window_width * sizeof(u32));
		SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		
		u64 current_time = SDL_GetPerformanceCounter();
		delta_time = (current_time - last_time) * 1000.0f / SDL_GetPerformanceFrequency();
		last_time = current_time;
	}

	SDL_Quit();

	free(pixels);


	return 0;
};
