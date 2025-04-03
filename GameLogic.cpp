#include "GameLogic.h"

bool isWithinBounds(ll x, ll y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

std::tuple<bool, ll, ll, ll, ll> checkWin(ll currentPlayer) {
    for (ll i = 0; i < BOARD_SIZE; ++i) {
        for (ll j = 0; j <= BOARD_SIZE - WIN_CONDITION; ++j) {
            bool win = true;
            ll blockCount = 0;
            for (ll k = 0; k < WIN_CONDITION; ++k) {
                if (board[i][j + k] != currentPlayer) {
                    win = false;
                    break;
                }
            }
            if (win) {
                if ((j > 0 && board[i][j - 1] != '\0' && board[i][j - 1] != currentPlayer) ||
                    (j + WIN_CONDITION < BOARD_SIZE && board[i][j + WIN_CONDITION] != '\0' && board[i][j + WIN_CONDITION] != currentPlayer)) {
                    blockCount++;
                }
                if (blockCount < 2) return { true, i, j, i, j + WIN_CONDITION - 1 };
            }
        }
    }

    for (ll i = 0; i <= BOARD_SIZE - WIN_CONDITION; ++i) {
        for (ll j = 0; j < BOARD_SIZE; ++j) {
            bool win = true;
            ll blockCount = 0;
            for (ll k = 0; k < WIN_CONDITION; ++k) {
                if (board[i + k][j] != currentPlayer) {
                    win = false;
                    break;
                }
            }
            if (win) {
                if ((i > 0 && board[i - 1][j] != '\0' && board[i - 1][j] != currentPlayer) ||
                    (i + WIN_CONDITION < BOARD_SIZE && board[i + WIN_CONDITION][j] != '\0' && board[i + WIN_CONDITION][j] != currentPlayer)) {
                    blockCount++;
                }
                if (blockCount < 2) return { true, i, j, i + WIN_CONDITION - 1, j };
            }
        }
    }

    for (ll i = 0; i <= BOARD_SIZE - WIN_CONDITION; ++i) {
        for (ll j = 0; j <= BOARD_SIZE - WIN_CONDITION; ++j) {
            bool win = true;
            ll blockCount = 0;
            for (ll k = 0; k < WIN_CONDITION; ++k) {
                if (board[i + k][j + k] != currentPlayer) {
                    win = false;
                    break;
                }
            }
            if (win) {
                if ((isWithinBounds(i - 1, j - 1) && board[i - 1][j - 1] != '\0' && board[i - 1][j - 1] != currentPlayer) ||
                    (isWithinBounds(i + WIN_CONDITION, j + WIN_CONDITION) && board[i + WIN_CONDITION][j + WIN_CONDITION] != '\0' && board[i + WIN_CONDITION][j + WIN_CONDITION] != currentPlayer)) {
                    blockCount++;
                }
                if (blockCount < 2) return { true, i, j, i + WIN_CONDITION - 1, j + WIN_CONDITION - 1 };
            }
        }
    }

    for (ll i = 0; i <= BOARD_SIZE - WIN_CONDITION; ++i) {
        for (ll j = WIN_CONDITION - 1; j < BOARD_SIZE; ++j) {
            bool win = true;
            ll blockCount = 0;
            for (ll k = 0; k < WIN_CONDITION; ++k) {
                if (board[i + k][j - k] != currentPlayer) {
                    win = false;
                    break;
                }
            }
            if (win) {
                if ((isWithinBounds(i - 1, j + 1) && board[i - 1][j + 1] != '\0' && board[i - 1][j + 1] != currentPlayer) ||
                    (isWithinBounds(i + WIN_CONDITION, j - WIN_CONDITION) && board[i + WIN_CONDITION][j - WIN_CONDITION] != '\0' && board[i + WIN_CONDITION][j - WIN_CONDITION] != currentPlayer)) {
                    blockCount++;
                }
                if (blockCount < 2) return { true, i, j, i + WIN_CONDITION - 1, j - WIN_CONDITION + 1 };
            }
        }
    }

    return { false, 0, 0, 0, 0 };
}

bool ischan(ll x, ll y) {
    return x < 0 || y < 0 || x >= Row || y >= Column;
}

int AttackPoint(int PlayerCom[MaxN][MaxN], int x, int y) {
    int tX[8] = { 1, 0, 1, 1, -1, 0, -1, -1 };
    int tY[8] = { 0, 1, 1, -1, 0, -1, -1, 1 };
    int Ally[4] = {}, Enemy[4] = {}, Block[4] = {};

    for (int k = 0; k < 8; k++) {
        for (int i = 1; i < 6; i++) {
            if (ischan(x + i * tX[k], y + i * tY[k])) {
                Block[k % 4]++;
                break;
            }
            if (PlayerCom[x + i * tX[k]][y + i * tY[k]] == 0) break;
            if (PlayerCom[x + i * tX[k]][y + i * tY[k]] == -1) Ally[k % 4]++;
            if (PlayerCom[x + i * tX[k]][y + i * tY[k]] == 1) {
                Enemy[k % 4]++;
                break;
            }
        }
    }

    int SumPoint = 0;
    for (int i = 0; i < 4; i++) {
        int Point = AttackArr[Ally[i]];
        if (Ally[i] == 4) Point = AttackArr[9];
        if (Enemy[i] == 1 || Block[i] == 1) Point /= 2;
        if (Enemy[i] == 1 && Ally[i] < 4 && Block[i] == 1) Point = 0;
        if (Enemy[i] == 2) Point = 0;
        SumPoint += Point;
    }
    return SumPoint;
}

int DefendPoint(int PlayerCom[MaxN][MaxN], int x, int y) {
    int tX[8] = { 1, 0, 1, 1, -1, 0, -1, -1 };
    int tY[8] = { 0, 1, 1, -1, 0, -1, -1, 1 };
    int Ally[4] = {}, Enemy[4] = {}, Block[4] = {};

    for (int k = 0; k < 8; k++) {
        for (int i = 1; i < 6; i++) {
            if (ischan(x + i * tX[k], y + i * tY[k])) {
                Block[k % 4]++;
                break;
            }
            if (PlayerCom[x + i * tX[k]][y + i * tY[k]] == 0) break;
            if (PlayerCom[x + i * tX[k]][y + i * tY[k]] == 1) Enemy[k % 4]++;
            if (PlayerCom[x + i * tX[k]][y + i * tY[k]] == -1) {
                Ally[k % 4]++;
                break;
            }
        }
    }

    int SumPoint = 0;
    for (int i = 0; i < 4; i++) {
        int Point = DefendArr[Enemy[i]];
        if (Enemy[i] == 4) Point = DefendArr[9];
        if ((Ally[i] == 1) || (Block[i] == 1)) Point /= 2;
        if (Ally[i] == 1 && Enemy[i] < 4 && Block[i] == 1) Point = 0;
        if (Ally[i] == 2) Point = 0;
        SumPoint += Point;
    }
    return SumPoint;
}

void ComputerGo(SDL_Renderer* renderer, TTF_Font* font, int PlayerCom[MaxN][MaxN], ll& currentPlayer, bool& gameWon, ll& winStartX, ll& winStartY, ll& winEndX, ll& winEndY) {
    int x, y;
    int MaxPoint = -DefendArr[9];
    for (int i = 0; i < Row; i++) {
        for (int j = 0; j < Column; j++) {
            if (PlayerCom[i][j] == 0) {
                int r = rand() % 2;
                int Point = AttackPoint(PlayerCom, i, j) + DefendPoint(PlayerCom, i, j);
                if (MaxPoint < Point || (MaxPoint == Point && r == 0)) {
                    MaxPoint = Point;
                    x = i;
                    y = j;
                }
            }
        }
    }
    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[x][y] == 0) {
        board[x][y] = -1;
        SDL_Color colorO = { 0, 0, 255 };
        ll centerX = y * CELL_SIZE + CELL_SIZE / 2;
        ll centerY = x * CELL_SIZE + CELL_SIZE / 2;
        renderText(renderer, font, "O", centerX, centerY, colorO);
        std::tuple<bool, ll, ll, ll, ll> winResult = checkWin(currentPlayer);
        if (std::get<0>(winResult)) {
            gameWon = true;
            winStartX = std::get<1>(winResult);
            winStartY = std::get<2>(winResult);
            winEndX = std::get<3>(winResult);
            winEndY = std::get<4>(winResult);
        }
    }
}

void handleMouseClick(SDL_Renderer* renderer, TTF_Font* font, ll x, ll y, ll& currentPlayer, bool& gameWon, ll& winStartX, ll& winStartY, ll& winEndX, ll& winEndY) {
    ll row = y / CELL_SIZE;
    ll col = x / CELL_SIZE;
    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE && board[row][col] == 0) {
        board[row][col] = currentPlayer;
        std::tuple<bool, ll, ll, ll, ll> winResult = checkWin(currentPlayer);
        if (std::get<0>(winResult)) {
            gameWon = true;
            winStartX = std::get<1>(winResult);
            winStartY = std::get<2>(winResult);
            winEndX = std::get<3>(winResult);
            winEndY = std::get<4>(winResult);
            return;
        }
        currentPlayer = (currentPlayer == 1) ? -1 : 1;
    }
}

void drawWinningLine(SDL_Renderer* renderer, ll startX, ll startY, ll endX, ll endY) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, startY * CELL_SIZE + CELL_SIZE / 2, startX * CELL_SIZE + CELL_SIZE / 2, endY * CELL_SIZE + CELL_SIZE / 2, endX * CELL_SIZE + CELL_SIZE / 2);
}