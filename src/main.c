#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string.h>
#include <malloc.h>

#ifdef BUILD_LINUX
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif


SDL_Window* window;
SDL_Renderer* renderer;

SDL_Texture *texture_panda;


int init()
{
    SDL_DisplayMode mode;
    SDL_Rect screen_size;

    // Initialize SDL itself
    printf("SDL_Init()\n");
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init error: %s\n", SDL_GetError());

        return 0;
    }

    // Get the current display mode
    printf("SDL_GetCurrentDisplayMode()\n");
    if(SDL_GetCurrentDisplayMode( 0, &mode ) != 0)
    {
        printf("SDL_GetCurrentDisplayMode error: %s\n", SDL_GetError());

        SDL_Quit();

        return 0;
    }

    screen_size.w = mode.w;
    screen_size.h = mode.h;

    //Create a new full-screen window
    printf("SDL_CreateWindow()\n");
    window = SDL_CreateWindow("Multiplatform Base Application",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              screen_size.w,
                              screen_size.h,
                              SDL_WINDOW_SHOWN);
    if(!window)
    {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());

        SDL_Quit();

        return 0;
    }

    // Create a renderer
    printf("SDL_CreateRenderer()\n");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer)
    {
        printf("SDL_CreateRenderer error: %s\n", SDL_GetError());

        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
    }

    return 1;
}


int load_resources()
{
    // Load the panda
    printf("SDL_LoadBMP(\"panda.bmp\")\n");
    SDL_Surface* bitmap_panda = SDL_LoadBMP("panda.bmp");

    if(!bitmap_panda)
    {
        printf("SDL_LoadBMP error: %s\n", SDL_GetError());
        return 0;
    }

    // Convert the panda to a texture
    printf("SDL_CreateTextureFromSurface()\n");
    texture_panda = SDL_CreateTextureFromSurface(renderer, bitmap_panda);
    SDL_FreeSurface(bitmap_panda);

    if(!texture_panda)
    {
        printf("SDL_CreateTextureFromSurface error: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}


void free_resources()
{
    SDL_DestroyTexture(texture_panda);
}


void cleanup()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}


int handle_events()
{
    SDL_Event e;

    while (SDL_PollEvent(&e)){
        // Window is closed
        if (e.type == SDL_QUIT){
            return 0;
        }
    }

    return 1;
}


int main(int argc, char** argv)
{
    int running = 1;

    // Initialize everything
    if(!init())
    {
        printf("Failed to initialize SDL, exiting.\n");

        return 1;
    }

    // Load the resources
    if(!load_resources())
    {
        printf("Failed to load all necessary resources, exiting.\n");

        cleanup();

        return 1;
    }

    // Main loop
    do
    {
        // Handle events
        running = handle_events();

        // Clear the renderer
        SDL_RenderClear(renderer);

        // Draw the panda
        SDL_RenderCopy(renderer, texture_panda, NULL, NULL);

        // Update the screen
        SDL_RenderPresent(renderer);

    } while(running);

    // Free resources
    free_resources();

    // Clean up SDL
    cleanup();

    // Exit
    return 0;
}
