#include "Board.h"

ll board[BOARD_SIZE][BOARD_SIZE];

void drawBoard(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            SDL_Rect cellRect = { j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderFillRect(renderer, &cellRect);
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 0xFF / 5);
            SDL_RenderDrawRect(renderer, &cellRect);
        }
    }
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    for (int i = 0; i <= BOARD_SIZE; ++i) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, BOARD_SIZE * CELL_SIZE);
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, BOARD_SIZE * CELL_SIZE, i * CELL_SIZE);
    }
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, ll x, ll y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect dstrect = { x - surface->w / 2, y - surface->h / 2, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void drawPieces(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color colorX = { 255, 0, 0 };
    SDL_Color colorO = { 0, 0, 255 };
    for (ll i = 0; i < BOARD_SIZE; ++i) {
        for (ll j = 0; j < BOARD_SIZE; ++j) {
            ll centerX = j * CELL_SIZE + CELL_SIZE / 2;
            ll centerY = i * CELL_SIZE + CELL_SIZE / 2;
            if (board[i][j] == 1) {
                renderText(renderer, font, "X", centerX, centerY, colorX);
            }
            else if (board[i][j] == -1) {
                renderText(renderer, font, "O", centerX, centerY, colorO);
            }
        }
    }
}

void Show_who_wins(SDL_Renderer* renderer, TTF_Font* font, ll currentPlayer) {
    SDL_Color Red = { 76, 187, 23, 255 };
    SDL_Surface* surfaceMessage = (currentPlayer == 1) ? TTF_RenderText_Solid(font, "PLAYER WINS!", Red) : TTF_RenderText_Solid(font, "BOT WINS!", Red);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect Message_rect = { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 200, 300, 100 };
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}