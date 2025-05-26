#define _CRT_NONSTDC_NO_DEPRECATE
#include<iostream>
#include<windows.h>
#include<string>
#include<cstdlib>
#include<conio.h>
#include<ctime>

/* 俄羅斯方塊  Tetris */
using namespace std;

/* 工具箱  */
HANDLE hIn, hOut;

void Control_HANDLE_Unit()
{
    hIn = GetStdHandle(STD_INPUT_HANDLE);
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
}

//座標給定函式 
void position(int x, int y)
{
    static COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(hOut, c);
}

/*遊戲主體*/
const int nWidth = 15, nHeight = 20;
char Unit[3] = { '|','H','X' };
int frame[nHeight][nWidth] = { 0 };
int Delay_time;
int score = 0;

void draw(int x, int y, int uniframe, char* unit)
{
    position(x, y);
    if (uniframe == 1)
        cout << unit[0];
    else if (uniframe == 2)
        cout << unit[1];
    else if (uniframe == 3)
        cout << unit[2];
    else
        cout << ' ';
}

int i = 5, j = 0;

int Asset[7][4][4] = {
   {{0,0,3,0},{0,0,3,0},{0,0,3,0},{0,0,3,0}},
   {{0,0,3,0},{0,3,3,0},{0,3,0,0},{0,0,0,0}},
   {{0,3,0,0},{0,3,3,0},{0,0,3,0},{0,0,0,0}},
   {{0,0,0,0},{0,0,3,0},{0,3,3,3},{0,0,0,0}},
   {{0,0,0,0},{0,3,3,0},{0,3,3,0},{0,0,0,0}},
   {{0,3,3,0},{0,0,3,0},{0,0,3,0},{0,0,0,0}},
   {{0,3,3,0},{0,3,0,0},{0,3,0,0},{0,0,0,0}}
};

void AssetPos(int px, int py, int(&asset)[4][4])
{
    for (int y = py; y < (py + 4); y++)
        for (int x = px; x < (px + 4); x++)
            if (frame[y][x] == 0 && asset[y - py][x - px] != 0) {
                frame[y][x] = asset[y - py][x - px];
                draw(x, y, frame[y][x], Unit);
            }
}

void cleanAsset(int px, int py, int(&asset)[4][4])
{
    for (int y = py; y < (py + 4); y++)
        for (int x = px; x < (px + 4); x++)
            if (asset[y - py][x - px] == 3) {
                frame[y][x] = 0;
                draw(x, y, frame[y][x], Unit);
            }
}

bool collision(int px, int py, int(&asset)[4][4])
{
    for (int y = py; y < (py + 4); y++)
        for (int x = px; x < (px + 4); x++)
            if (frame[y][x] != 0 && asset[y - py][x - px] != 0) return true;
    return false;
}

void Rotation(int px, int py, int(&asset)[4][4])
{
    int temp[4][4] = { 0 };
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++)
            temp[x][y] = asset[3 - y][x];
    cleanAsset(px, py, asset);
    if (!collision(px, py, temp)) {
        for (int x = 0; x < 4; x++)
            for (int y = 0; y < 4; y++)
                asset[x][y] = temp[x][y];
    }
    AssetPos(px, py, asset);
}

static DWORD inputCount;
static INPUT_RECORD ir;

void key_Control(int(&asset)[4][4])
{
    ReadConsoleInput(hIn, &ir, 1, &inputCount);
    switch (ir.Event.KeyEvent.wVirtualKeyCode)
    {
    case VK_LEFT:
        cleanAsset(i, j, asset);
        if (!collision(i - 1, j, asset)) i--;
        AssetPos(i, j, asset);
        break;
    case VK_RIGHT:
        cleanAsset(i, j, asset);
        if (!collision(i + 1, j, asset)) i++;
        AssetPos(i, j, asset);
        break;
    case VK_UP:
        Rotation(i, j, asset);
        break;
    case VK_DOWN:
        Delay_time = 0;
        break;
    }
    Sleep(30);
}

bool Eliminate_Row_Asset(int y)
{
    for (int x = 1; x < nWidth - 1; x++)
        if (frame[y][x] != 3) return false;
    return true;
}

void Score(int count)
{
    score += count;
    position(nWidth + 16, nHeight - 1);
    cout << score;
}

bool GameOver()
{
    for (int x = 0; x < nWidth; x++)
        if (frame[0][x] == 3) return true;
    return false;
}

void SpeedMode()
{
    if (score > 5000)
        Delay_time = 25;
    else if (score > 4000)
        Delay_time = 50;
    else if (score > 2000)
        Delay_time = 100;
    else if (score > 1000)
        Delay_time = 150;
    else
        Delay_time = 200;
}

int main()
{
    Control_HANDLE_Unit();

    for (int x = 0; x < nWidth; x++)
        frame[nHeight - 1][x] = 2;
    for (int y = 0; y < nHeight; y++) {
        frame[y][0] = 1;
        frame[y][nWidth - 1] = 1;
    }

    for (int y = 0; y < nHeight; y++)
        for (int x = 0; x < nWidth; x++)
            draw(x, y, frame[y][x], Unit);

    for (int y = 0; y < nHeight; y++) {
        position(nWidth + 1, y);
        cout << '>' << y;
    }

    position(nWidth + 10, nHeight - 1);
    cout << "Score:";
    Score(0);

    int asset[4][4] = { 0 };

    while (1)
    {
        i = 5, j = 0;
        SpeedMode();
        srand((unsigned int)time(NULL));
        int random = rand() % 7;
        for (int x = 0; x < 4; x++)
            for (int y = 0; y < 4; y++)
                asset[x][y] = Asset[random][x][y];

        AssetPos(i, j, asset);

        while (1)
        {
            while (!kbhit())
            {
                Sleep(Delay_time);
                cleanAsset(i, j, asset);
                if (collision(i, j + 1, asset)) {
                    AssetPos(i, j, asset);
                    break;
                }
                j++;
                AssetPos(i, j, asset);
                Sleep(Delay_time);
                if (kbhit()) {
                    ReadConsoleInput(hIn, &ir, 1, &inputCount);
                    key_Control(asset);
                }
            }
            if (kbhit()) {
                ReadConsoleInput(hIn, &ir, 1, &inputCount);
                key_Control(asset);
            }
            cleanAsset(i, j, asset);
            if (collision(i, j + 1, asset)) {
                AssetPos(i, j, asset);
                break;
            }
            AssetPos(i, j, asset);
        }

        for (int y = nHeight - 2; y > 0; y--) {
            if (Eliminate_Row_Asset(y)) {
                for (int x = 1; x < nWidth - 1; x++) {
                    frame[y][x] = 0;
                    draw(x, y, frame[y][x], Unit);
                }
                Sleep(100);
                Score(100);
                for (int j = y; j > 0; j--)
                    for (int x = 1; x < nWidth - 1; x++) {
                        frame[j][x] = frame[j - 1][x];
                        draw(x, j, frame[j][x], Unit);
                    }
                y++;
            }
        }

        if (GameOver()) {
            position(nWidth + 10, nHeight / 2);
            cout << "GAME OVER!!";
            break;
        }
    }

    system("PAUSE");
    return 0;
}
