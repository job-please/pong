#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// game dimensions
#define WIN_WIDTH 640
#define WIN_HEIGHT 480
#define PAD_WIDTH 10
#define PAD_HEIGHT 80
#define BAL_SIZE 5

// game speed
const int speed = 5;

// main
int main(int argc, char* argv[]) {

    printf("RUNNING!");

    // create frame, renderer, scoreboard
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* frame = SDL_CreateWindow("Pong", 0, 0, WIN_WIDTH, WIN_HEIGHT, 0);
    SDL_Renderer* render = SDL_CreateRenderer(frame, -1, SDL_RENDERER_ACCELERATED);
    if (TTF_Init()==-1) {
        printf("SDL_ttf Init Error: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }
    TTF_Font* font = TTF_OpenFont("fonts/PressStart2P-Regular.ttf", 24);
    if (!font) {
        printf("Font load error: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    // define paddles, ball
    SDL_Rect my_paddle = {0, 0, PAD_WIDTH, PAD_HEIGHT};
    SDL_Rect ball = {WIN_WIDTH/2, WIN_HEIGHT/2, BAL_SIZE, BAL_SIZE};
    int ball_vel_x = speed;
    int ball_vel_y = speed;

    // game loop
    bool run = true;
    SDL_Event event;
    int score = 0;
    while (run) {
        // get key
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q) {
                run = false;
            }
        }

        // update paddle position
        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_W] && my_paddle.y > 0) {
            my_paddle.y -= 5;
        }
        if (keystates[SDL_SCANCODE_S] && my_paddle.y < (WIN_HEIGHT-PAD_HEIGHT)) {
            my_paddle.y += 5;
        }

        // update ball position
        ball.x += ball_vel_x;
        ball.y += ball_vel_y;

        // handle collisions
        if (ball.y <= 0 || ball.y >= WIN_HEIGHT-BAL_SIZE) {
            ball_vel_y = -ball_vel_y;
        }
        if (ball.x >= WIN_WIDTH) {
            ball_vel_x = -ball_vel_x;
        }
        if (SDL_HasIntersection(&ball, &my_paddle)) {
            ball_vel_x = -ball_vel_x;
            ball.x = my_paddle.x + PAD_WIDTH; // avoid sticking
        }

        // ball missed
        if (ball.x + BAL_SIZE <= 0) {
            ball.x = WIN_WIDTH/2;
            ball.y = WIN_HEIGHT/2;
            SDL_Delay(1000);
            score++;
        }

        // clear frame
        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);

        // draw objects
        SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
        SDL_RenderFillRect(render, &my_paddle);
        SDL_RenderFillRect(render, &ball);

        char score_buf[32];
        sprintf(score_buf, "Missed: %d", score);
        SDL_Color white = {255,255,255,255};
        SDL_Surface* text_surface = TTF_RenderText_Solid(font, score_buf, white);
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(render, text_surface);
        SDL_Rect scoreboard = {10, 10, text_surface->w, text_surface->h};
        SDL_RenderCopy(render, text_texture, NULL, &scoreboard);

        SDL_FreeSurface(text_surface);
        SDL_DestroyTexture(text_texture);

        // show
        SDL_RenderPresent(render);

        SDL_Delay(16);
    }

    // close frame
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(frame);
    SDL_Quit();
    TTF_CloseFont(font);
    TTF_Quit();
    return 0;
}