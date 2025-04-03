#include <Windows.h>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <cstdlib>
#include <ctime>
#include "GameConstants.h"
#include "Board.h"
#include "GameLogic.h"
#undef main
using namespace std;

int main(ll argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1 || !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return 1;

    SDL_Window* window = SDL_CreateWindow("Caro Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return 1;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("calibri.ttf", 30);
    if (!font) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface* loadedSurface = IMG_Load("Dark_warrior.jpg");
    if (!loadedSurface) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    if (!backgroundTexture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface* cursorSurfaceOriginal = IMG_Load("Orange_cursor.png");
    if (!cursorSurfaceOriginal) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface* cursorSurface = SDL_CreateRGBSurface(0, CURSOR_WIDTH, CURSOR_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    SDL_Rect dstRect = { 0, 0, CURSOR_WIDTH, CURSOR_HEIGHT };
    SDL_BlitScaled(cursorSurfaceOriginal, NULL, cursorSurface, &dstRect);
    SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, 0, 0);
    SDL_FreeSurface(cursorSurface);
    if (!cursor) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_SetCursor(cursor);

    bool quit = false;
    SDL_Event e;
    ll currentPlayer = 1;
    bool gameWon = false;
    ll winStartX = 0, winStartY = 0, winEndX = 0, winEndY = 0;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;
            else if (e.type == SDL_MOUSEBUTTONDOWN && !gameWon) {
                int x = e.button.x;
                int y = e.button.y;
                if (x / CELL_SIZE < BOARD_SIZE && y / CELL_SIZE < BOARD_SIZE && board[y / CELL_SIZE][x / CELL_SIZE] == 0) {
                    handleMouseClick(renderer, font, x, y, currentPlayer, gameWon, winStartX, winStartY, winEndX, winEndY);
                    if (!gameWon) {
                        currentPlayer = -1;
                        ComputerGo(renderer, font, board, currentPlayer, gameWon, winStartX, winStartY, winEndX, winEndY);
                        currentPlayer = 1;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        drawBoard(renderer);
        drawPieces(renderer, font);
        if (gameWon) {
            drawWinningLine(renderer, winStartX, winStartY, winEndX, winEndY);
            Show_who_wins(renderer, font, std::get<0>(checkWin(1)) ? 1 : -1);
            currentPlayer = 1;
            if (SDL_PollEvent(&e) != 0 && e.type == SDL_MOUSEBUTTONDOWN) {
                memset(board, 0, sizeof(board));
                gameWon = false;
            }
        }
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_FreeCursor(cursor);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}