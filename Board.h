#pragma once
#ifndef BOARD_H
#define BOARD_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "GameConstants.h"

extern ll board[BOARD_SIZE][BOARD_SIZE];

void drawBoard(SDL_Renderer* renderer);
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, ll x, ll y, SDL_Color color);
void drawPieces(SDL_Renderer* renderer, TTF_Font* font);
void Show_who_wins(SDL_Renderer* renderer, TTF_Font* font, ll currentPlayer);

#endif