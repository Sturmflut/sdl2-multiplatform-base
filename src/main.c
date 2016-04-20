#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string.h>
#include <malloc.h>

#ifdef BUILD_LINUX
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#else
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#endif


SDL_Window* window;
SDL_Renderer* renderer;

SDL_Texture *texture_panda;

SDL_Rect rect_screen;
SDL_Rect rect_panda;

Mix_Music* music_bg;
Mix_Music* sound_jump;

float accel = 0.0f;
float rotation = 0.0f;


int init()
{
    SDL_DisplayMode mode;

    // Initialize SDL itself
    printf("SDL_Init()\n");
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
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

    // Init audio
    printf("Mix_Init()\n");
    if(Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG)
    {
        printf("Mix_Init error: %s\n", Mix_GetError());

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
    }

    // Open audio device
    printf("Mix_OpenAudio()\n");
    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0)
    {
        printf("Mix_OpenAudio error: %s\n", Mix_GetError());

        Mix_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
    }

    return 1;
}


int load_resources()
{
    // Load the panda PNG into a texture
    printf("IMG_LoadTexture()\n");
    texture_panda = IMG_LoadTexture(renderer, "panda.png");

    if(!texture_panda)
    {
        printf("SDL_CreateTextureFromSurface error: %s\n", SDL_GetError());
        return 0;
    }

    // Store panda size
    SDL_QueryTexture(texture_panda, NULL, NULL, &rect_panda.w, &rect_panda.h);

    // Load background music
    printf("Mix_LoadMUS(\"bgmusic.ogg\")\n");
    music_bg = Mix_LoadWAV("bgmusic.ogg");
    if(!music_bg)
    {
        printf("Mix_LoadMUS error: %s\n", Mix_GetError());

        SDL_DestroyTexture(texture_panda);
        return 0;
    }

    // Load background music
    printf("Mix_LoadMUS(\"jump.ogg\")\n");
    sound_jump = Mix_LoadWAV("jump.ogg");
    if(!sound_jump)
    {
        printf("Mix_LoadMUS error: %s\n", Mix_GetError());

        Mix_FreeMusic(music_bg);

        SDL_DestroyTexture(texture_panda);
        return 0;
    }

    return 1;
}


void free_resources()
{
    Mix_FreeMusic(sound_jump);
    Mix_FreeMusic(music_bg);

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

        // Jump
        if(e.type == SDL_KEYDOWN
                || e.type == SDL_FINGERDOWN
                || e.type == SDL_MOUSEBUTTONDOWN)
        {
            accel = 20.0f;
            Mix_PlayChannel(-1, sound_jump, 0);
        }
    }

    return 1;
}


int main(int argc, char** argv)
{
    int running = 1;

    int move_right = 1;

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
    rect_panda.y = rect_screen.h - rect_panda.h;

    // Measure time
    ticks = SDL_GetTicks();

    // Start playing music
    if(Mix_PlayChannel(0, music_bg, -1) == -1)
        printf("Failed to play background music: %s\n", Mix_GetError());

    // Main loop
    do
    {
        frame_current++;

        // Move the Panda
        if(move_right)
        {
            rect_panda.x = rect_panda.x + 3;
            rotation += 1.5;
        }
        else
        {
            rect_panda.x = rect_panda.x - 3;
            rotation -= 1.5;
        }


        rect_panda.y -= accel;
        accel -= 1;



        // Handle edges and floor
        if(move_right && rect_panda.x > rect_screen.w - rect_panda.w)
            move_right = 0;

        if(!move_right && rect_panda.x < 1)
            move_right = 1;

        if(rect_panda.y > rect_screen.h - rect_panda.h)
            rect_panda.y = rect_screen.h - rect_panda.h;


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
        SDL_RenderCopyEx(renderer,
                         texture_panda,
                         NULL,
                         &rect_panda,
                         rotation,
                         NULL,
                         move_right ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);

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
