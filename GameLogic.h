#pragma once
#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <tuple>
#include "GameConstants.h"
#include "Board.h"

bool isWithinBounds(ll x, ll y);
std::tuple<bool, ll, ll, ll, ll> checkWin(ll currentPlayer);
bool ischan(ll x, ll y);
int AttackPoint(int PlayerCom[MaxN][MaxN], int x, int y);
int DefendPoint(int PlayerCom[MaxN][MaxN], int x, int y);
void ComputerGo(SDL_Renderer* renderer, TTF_Font* font, int PlayerCom[MaxN][MaxN], ll& currentPlayer, bool& gameWon, ll& winStartX, ll& winStartY, ll& winEndX, ll& winEndY);
void handleMouseClick(SDL_Renderer* renderer, TTF_Font* font, ll x, ll y, ll& currentPlayer, bool& gameWon, ll& winStartX, ll& winStartY, ll& winEndX, ll& winEndY);
void drawWinningLine(SDL_Renderer* renderer, ll startX, ll startY, ll endX, ll endY);

#endif