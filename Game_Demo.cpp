/*
// Game_Demo.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <Windows.h>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_render.h>
#include <tuple>
#include <SDL_surface.h>
#include <cstdlib> // for rand()
#include <ctime> // for time()
#undef main
using namespace std;
typedef int ll;

const ll MaxN = 20; // Giả định kích thước tối đa của bàn cờ
const ll Row = 20;   // Số hàng của bàn cờ
const ll Column = 20; // Số cột của bàn cờ
const int CURSOR_WIDTH = 32; // Đặt kích thước mong muốn cho con trỏ
const int CURSOR_HEIGHT = 32; // Đặt kích thước mong muốn cho con trỏ
ll Result = 0;

//const ll AttackArr[10] = { 0, 3, 24, 192, 1536, 12288, 98304, 531441, 4782969, 500000000 };
//const ll DefendArr[10] = { 0, 2, 18, 140, 800, 8000, 70569, 350000, 30000000, 300000000 };
const ll AttackArr[10] = { 0, 5, 50, 500, 5000, 50000, 1000000, 5000000, 25000000, 1000000000 };
const ll DefendArr[10] = { 0, 10, 100, 1000, 10000, 50000, 250000, 1000000, 5000000, 50000000 };


struct Point {
    ll x, y;
    Point(ll x_, ll y_) {
        x = x_;
        y = y_;
    }
};

const ll WINDOW_WIDTH = 1280;
const ll WINDOW_HEIGHT = 720;
const ll CELL_SIZE = 35;
const ll BOARD_SIZE = 20;
const ll WIN_CONDITION = 5;
ll board[BOARD_SIZE][BOARD_SIZE];

void drawBoard(SDL_Renderer* renderer) {
    // Vẽ nền bàn cờ
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    //SDL_SetRenderDrawColor(renderer, 0, 0 , 0, 0xFF);
    //SDL_RenderClear(renderer);

    // Vẽ các ô trên bàn cờ
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            SDL_Rect cellRect = { j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE };

            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);// màu ô màu đen
            //SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // Màu của ô trống (trắng)
            // Vẽ ô
            SDL_RenderFillRect(renderer, &cellRect);

            // Vẽ viền ô
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 0xFF/ 5); // Màu viền (xám)
            SDL_RenderDrawRect(renderer, &cellRect);
        }
    }

    // Vẽ các đường lưới
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    for (int i = 0; i <= BOARD_SIZE; ++i) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, BOARD_SIZE * CELL_SIZE);
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, BOARD_SIZE * CELL_SIZE, i * CELL_SIZE);
    }
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, ll x, ll y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect dstrect = { x - surface->w / 2, y - surface->h / 2, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void drawPieces(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color colorX = { 255, 0, 0 };  // Màu đỏ cho X
    SDL_Color colorO = { 0, 0, 255 };  // Màu xanh dương cho O

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

void Show_who_wins(SDL_Renderer* renderer, TTF_Font* font, ll &currentPlayer) {
    SDL_Color Red = { 76, 187, 23, 255 };
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "PLAYER WINS!", Red);;
    if(currentPlayer == 1) surfaceMessage = TTF_RenderText_Solid(font, "PLAYER WINS!", Red);
    else surfaceMessage = TTF_RenderText_Solid(font, "BOT WINS!", Red);
    // now you can convert it into a texture
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = WINDOW_WIDTH/ 2;  //controls the rect's x coordinate 
    Message_rect.y = WINDOW_HEIGHT/ 2 - 200; // controls the rect's y coordinte
    Message_rect.w = 300; // controls the width of the rect
    Message_rect.h = 100; // controls the height of the rect

    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

bool isWithinBounds(ll x, ll y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

std::tuple<bool, ll, ll, ll, ll> checkWin(ll currentPlayer) {
    // Check horizontal win
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
                if (blockCount < 2) {
                    return { true, i, j, i, j + WIN_CONDITION - 1 };
                }
            }
        }
    }

    // Check vertical win
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
                if (blockCount < 2) {
                    return { true, i, j, i + WIN_CONDITION - 1, j };
                }
            }
        }
    }

    // Check diagonal win (top-left to bottom-right)
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
                if (blockCount < 2) {
                    return { true, i, j, i + WIN_CONDITION - 1, j + WIN_CONDITION - 1 };
                }
            }
        }
    }

    // Check diagonal win (top-right to bottom-left)
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
                if (blockCount < 2) {
                    return { true, i, j, i + WIN_CONDITION - 1, j - WIN_CONDITION + 1 };
                }
            }
        }
    }

    return { false, 0, 0, 0, 0 };
}


ll tX[8] = { 1, 0, 1, 1, -1, 0, -1, -1 };
ll tY[8] = { 0, 1, 1, -1, 0, -1, -1, 1 };

// Kiểm tra xem ô có hợp lệ không (trong phạm vi bàn cờ)
bool ischan(ll x, ll y) {
    return x < 0 || y < 0 || x >= Row || y >= Column;
}

// Tìm vị trí trống
bool FindNullPos(ll PlayerCom[MaxN][MaxN]) {
    for (ll i = 0; i < Row; ++i)
        for (ll j = 0; j < Column; ++j)
            if (PlayerCom[i][j] == 0) return true;
    return false;
}

//const ll AttackArr[10] = { 0, 3, 24, 192, 1536, 12288, 98304, 531441, 4782969, 500000000 };
//const ll DefendArr[10] = { 0, 2, 18, 140, 800, 8000, 70569, 350000, 30000000, 300000000 };

// Tính điểm tấn công của ô [x][y]
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

            if (PlayerCom[x + i * tX[k]][y + i * tY[k]] == -1) {
                Ally[k % 4]++;
            }

            if (PlayerCom[x + i * tX[k]][y + i * tY[k]] == 1) {
                Enemy[k % 4]++;
                break;
            }
        }
    }

    int SumPoint = 0;
    for (int i = 0; i < 4; i++) {
        int Point = AttackArr[Ally[i]];

        // Mình có 4 con, ưu tiên đánh
        if (Ally[i] == 4) Point = AttackArr[9];

        // Bị chặn thì giảm điểm
        if (Enemy[i] == 1 || Block[i] == 1) Point /= 2;

        // Bị chặn hai đầu thì không đánh
        if (Enemy[i] == 1 && Ally[i] < 4 && Block[i] == 1) Point = 0;
        if (Enemy[i] == 2) Point = 0;
        SumPoint += Point;
    }

    return SumPoint;
}

// Tính điểm phòng ngự của ô [x][y]
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

            if (PlayerCom[x + i * tX[k]][y + i * tY[k]] == 1) {
                Enemy[k % 4]++;
            }

            if (PlayerCom[x + i * tX[k]][y + i * tY[k]] == -1) {
                Ally[k % 4]++;
                break;
            }
        }
    }

    int SumPoint = 0;
    for (int i = 0; i < 4; i++) {
        int Point = DefendArr[Enemy[i]];

        // Địch có 4 con, ưu tiên chặn
        if (Enemy[i] == 4) Point = DefendArr[9];

        // Bị chặn thì giảm điểm
        if ((Ally[i] == 1) || (Block[i] == 1)) Point /= 2;

        // Bị chặn hai đầu thì điểm bằng 0
        if (Ally[i] == 1 && Enemy[i] < 4 && Block[i] == 1) Point = 0;
        if (Ally[i] == 2) Point = 0;
        SumPoint += Point;
    }

    return SumPoint;
}

// Hàm điều khiển lượt đi của máy tính
void ComputerGo(SDL_Renderer* renderer, TTF_Font* font, int PlayerCom[MaxN][MaxN], ll& currentPlayer, bool& gameWon, ll& winStartX, ll& winStartY, ll& winEndX, ll& winEndY) {
    int x, y;
    // Tính điểm tổng = điểm công + điểm thủ
    int MaxPoint = -DefendArr[9];
    for (int i = 0; i < Row; i++) {
        for (int j = 0; j < Column; j++) {
            if (PlayerCom[i][j] == 0) {
                // Chọn r ngẫu nhiên, tăng tính ngẫu nhiên
                int r = rand() % 2;
                int Point = AttackPoint(PlayerCom, i, j) + DefendPoint(PlayerCom, i, j);

                // Lấy tổng điểm lớn nhất (MaxPoint), đánh vào đó
                if (MaxPoint < Point || (MaxPoint == Point && r == 0)) {
                    MaxPoint = Point;
                    x = i;
                    y = j;
                }
            }
        }
    }
    ll row = y / CELL_SIZE;
    ll col = x / CELL_SIZE;
    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE && board[row][col] == 0) {
        PlayerCom[x][y] = -1;
        SDL_Color colorO = { 0, 0, 255 };  // Màu xanh cho O
        renderText(renderer, font, "O", x, y, colorO);
        std::tuple<bool, ll, ll, ll, ll> winResult = checkWin(currentPlayer);
        if (std::get<0>(winResult)) {
            std::cout << "Bot " << " wins!" << std::endl;
            gameWon = true;
            winStartX = std::get<1>(winResult);
            winStartY = std::get<2>(winResult);
            winEndX = std::get<3>(winResult);
            winEndY = std::get<4>(winResult);
            return;
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
            std::cout << "Player " << currentPlayer << " wins!" << std::endl;
            gameWon = true;
            winStartX = std::get<1>(winResult);
            winStartY = std::get<2>(winResult);
            winEndX = std::get<3>(winResult);
            winEndY = std::get<4>(winResult);
            return;
        }
        // Chuyển lượt chơi cho đối phương
        currentPlayer = (currentPlayer == 1) ? -1 : 1;
    }
}


void drawWinningLine(SDL_Renderer* renderer, ll startX, ll startY, ll endX, ll endY) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer,
        startY * CELL_SIZE + CELL_SIZE / 2, startX * CELL_SIZE + CELL_SIZE / 2,
        endY * CELL_SIZE + CELL_SIZE / 2, endX * CELL_SIZE + CELL_SIZE / 2);
}

SDL_Rect newGameButton = { 20, 20, 150, 50 }; // Vị trí (20, 20) và kích thước (150x50)

void drawNewGameButton(SDL_Renderer* renderer, TTF_Font* font) {
    // Vẽ nút
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Màu đen
    SDL_RenderFillRect(renderer, &newGameButton);

    // Vẽ chữ "New Game" lên nút
    SDL_Color color = { 255, 255, 255 }; // Màu trắng
    renderText(renderer, font, "New Game", newGameButton.x + newGameButton.w / 2, newGameButton.y + newGameButton.h / 2, color);
}


void handleNewGameButton(SDL_Event* e, bool& gameWon, ll& currentPlayer) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int x = e->button.x;
        int y = e->button.y;

        if (x >= newGameButton.x && x <= newGameButton.x + newGameButton.w &&
            y >= newGameButton.y && y <= newGameButton.y + newGameButton.h) {
            // Reset game state
            memset(board, 0, sizeof(board));
            gameWon = false;
            currentPlayer = 1;
            std::cout << "New Game started!" << std::endl;
        }
    }
}


int main(ll argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Caro Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("calibri.ttf", 30);
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load background image
    SDL_Surface* loadedSurface = IMG_Load("Dark_warrior.jpg");
    if (!loadedSurface) {
        std::cerr << "Unable to load image! SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    if (!backgroundTexture) {
        std::cerr << "Unable to create texture from surface! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    bool quit = false;
    SDL_Event e;
    ll currentPlayer = 1;
    bool gameWon = false;
    ll winStartX = 0, winStartY = 0, winEndX = 0, winEndY = 0;

    // Tải hình ảnh cho con trỏ chuột
    SDL_Surface* cursorSurfaceOriginal = IMG_Load("Orange_cursor.png");
    if (!cursorSurfaceOriginal) {
        std::cerr << "Unable to load image! SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tạo surface mới với kích thước mong muốn
    SDL_Surface* cursorSurface = SDL_CreateRGBSurface(0, CURSOR_WIDTH, CURSOR_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    SDL_Rect dstRect = { 0, 0, CURSOR_WIDTH, CURSOR_HEIGHT };
    SDL_BlitScaled(cursorSurfaceOriginal, NULL, cursorSurface, &dstRect);

    // Tạo con trỏ chuột từ surface
    SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, 0, 0);
    SDL_FreeSurface(cursorSurface);
    if (!cursor) {
        std::cerr << "Unable to create cursor! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Đặt con trỏ chuột
    SDL_SetCursor(cursor);
    
    while (!quit) {

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && !gameWon) {
                int x = e.button.x / CELL_SIZE;  // Tọa độ x của ô được click
                int y = e.button.y / CELL_SIZE;  // Tọa độ y của ô được click
                if (board[y][x] == 0 && x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE) {
                    SDL_GetMouseState(&x, &y);
                    handleMouseClick(renderer, font, x, y, currentPlayer, gameWon, winStartX, winStartY, winEndX, winEndY);
                    if (gameWon == false) {
                        currentPlayer = -1;
                        ComputerGo(renderer, font, board, currentPlayer, gameWon, winStartX, winStartY, winEndX, winEndY);
                        currentPlayer = 1;
                    }
                }
                
            }
            
            
        }

        

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);


        //Render background
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

        drawBoard(renderer);
        drawPieces(renderer, font);
        if (gameWon) {
            drawWinningLine(renderer, winStartX, winStartY, winEndX, winEndY);
            if(get<0>(checkWin(1)) == 1) Show_who_wins(renderer, font, currentPlayer = 1);
            else Show_who_wins(renderer, font, currentPlayer = -1);
            //reset ve nguoi choi 1
            currentPlayer = 1;
            //clear the table
            if (SDL_PollEvent(&e) != 0) {
                //handleNewGameButton(&e, gameWon, currentPlayer);
                //drawNewGameButton(renderer, font);
                if (e.type == SDL_MOUSEBUTTONDOWN && gameWon == 1) {
                    memset(board, 0, sizeof(board));
                    gameWon = !gameWon;
                }
            }
         
        }
        
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_FreeCursor(cursor);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    // Don't forget to free your surface and texture
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
*/

/*
#include <Windows.h>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_render.h>
#include <tuple>
#include <SDL_surface.h>
#include <cstdlib>
#include <ctime>
#undef main
using namespace std;
typedef int ll;

const ll MaxN = 20;
const ll Row = 20;
const ll Column = 20;
const int CURSOR_WIDTH = 32;
const int CURSOR_HEIGHT = 32;
ll Result = 0;

const ll AttackArr[10] = { 0, 5, 50, 500, 5000, 50000, 1000000, 5000000, 25000000, 1000000000 };
const ll DefendArr[10] = { 0, 10, 100, 1000, 10000, 50000, 250000, 1000000, 5000000, 50000000 };

struct Point {
    ll x, y;
    Point(ll x_, ll y_) : x(x_), y(y_) {}
};

const ll WINDOW_WIDTH = 1280;
const ll WINDOW_HEIGHT = 720;
const ll CELL_SIZE = 35;
const ll BOARD_SIZE = 20;
const ll WIN_CONDITION = 5;
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
                    x = i;  // x là hàng
                    y = j;  // y là cột
                }
            }
        }
    }
    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[x][y] == 0) {
        board[x][y] = -1;  // Bot đánh "O" vào ô [x][y]
        SDL_Color colorO = { 0, 0, 255 };
        ll centerX = y * CELL_SIZE + CELL_SIZE / 2;  // Tọa độ pixel ngang
        ll centerY = x * CELL_SIZE + CELL_SIZE / 2;  // Tọa độ pixel dọc
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
    ll row = y / CELL_SIZE;  // y là tọa độ pixel dọc
    ll col = x / CELL_SIZE;  // x là tọa độ pixel ngang
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
                int x = e.button.x;  // Tọa độ pixel ngang
                int y = e.button.y;  // Tọa độ pixel dọc
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
}*/