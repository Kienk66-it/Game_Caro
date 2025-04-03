#pragma once
#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

typedef int ll;

const ll MaxN = 20;
const ll Row = 20;
const ll Column = 20;
const int CURSOR_WIDTH = 32;
const int CURSOR_HEIGHT = 32;

const ll AttackArr[10] = { 0, 5, 50, 500, 5000, 50000, 1000000, 5000000, 25000000, 1000000000 };
const ll DefendArr[10] = { 0, 10, 100, 1000, 10000, 50000, 250000, 1000000, 5000000, 50000000 };

const ll WINDOW_WIDTH = 1280;
const ll WINDOW_HEIGHT = 720;
const ll CELL_SIZE = 35;
const ll BOARD_SIZE = 20;
const ll WIN_CONDITION = 5;

#endif