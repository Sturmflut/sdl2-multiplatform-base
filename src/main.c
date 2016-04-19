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


SDL_Rect rect_screen;
SDL_Rect rect_panda;


int init()
{
    SDL_DisplayMode mode;

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

    rect_screen.w = mode.w;
    rect_screen.h = mode.h;

    //Create a new full-screen window
    printf("SDL_CreateWindow()\n");
    window = SDL_CreateWindow("Multiplatform Base Application",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              rect_screen.w,
                              rect_screen.h,
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

    // Store panda size
    rect_panda.w = bitmap_panda->w;
    rect_panda.h = bitmap_panda->h;

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

    int move_right = 1;
    int move_down = 1;

    int frame_current = 0;
    Uint32 ticks;


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

    // Initialize Panda
    rect_panda.x = 1;
    rect_panda.y = 1;

    // Measure time
    ticks = SDL_GetTicks();

    // Main loop
    do
    {
        frame_current++;

        // Move the Panda
        if(move_right)
            rect_panda.x++;
        else
            rect_panda.x--;

        if(move_down)
            rect_panda.y++;
        else
            rect_panda.y--;

        // Handle edges
        if(move_right && rect_panda.x > rect_screen.w - rect_panda.w)
            move_right = 0;

        if(!move_right && rect_panda.x < 1)
            move_right = 1;

        if(move_down && rect_panda.y > rect_screen.h - rect_panda.h)
            move_down = 0;

        if(!move_down && rect_panda.y < 1)
            move_down = 1;


        // Handle events
        running = handle_events();

        // Set the background color
        SDL_SetRenderDrawColor(renderer,
                               frame_current % 255,
                               frame_current % 255,
                               frame_current % 255,
                               SDL_ALPHA_OPAQUE);

        // Clear the renderer
        SDL_RenderClear(renderer);

        // Draw the panda
        SDL_RenderCopy(renderer, texture_panda, NULL, &rect_panda);

        // Update the screen
        SDL_RenderPresent(renderer);

        // Calculate FPS
        if(frame_current % 60 == 0)
        {
            Uint32 tick_diff = SDL_GetTicks() - ticks;
            printf("%.2f FPS (%i milliseconds) for the last 60 frames\n", 1.0 / (tick_diff / 60000.0), tick_diff);

            ticks = SDL_GetTicks();
        }
    } while(running);

    // Free resources
    free_resources();

    // Clean up SDL
    cleanup();

    // Exit
    return 0;
}
