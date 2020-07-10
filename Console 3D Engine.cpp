#include <iostream>
#include <Windows.h>
#include <chrono>
#include <vector>
#include <algorithm>

#include "consts.h"
#include "My_Stack.h"
#include "maze_stack.h"
#include "cave_v1.h"

#define PI 3.1415926f

using namespace std;

int ScreenWidth = 278; // Ширина консольного окна   120 - Standart   278 - Fulscreen
int ScreenHeight = 68; // Высота консольного окна   40  - Standart   68  - Fulscreen

struct Object {
    float X = 1.0f;
    float Y = 1.0f;
    float A = 0.0f;
    float AngleVelocity = 2.f;
    float Velocity = 2.f;
} Player;

struct Map {
    int MapHeight = 16;     // Высота игрового поля
    int MapWidth = 16;      // Ширина игрового поля
    wstring map;            // Строковый массив
} M;

struct View {
    float FOV = PI / 3;    // Угол обзора (поле видимости)
    float Depth = 20.0f;   // Максимальная дистанция обзора
} V;

template<typename T>
struct Vector2D {
    T x;
    T y;

    Vector2D() : x(0), y(0) {};
    Vector2D(T nx, T ny) : x(nx), y(ny) {};
    Vector2D(const Vector2D<T>& V) : x(V.x), y(V.y) {};

    Vector2D<T>& operator = (const Vector2D<T>& V) { 
        if (this != &V) {
            this->x = V.x;
            this->y = V.y;
        }
        return *this; 
    }
    Vector2D<T> operator + (Vector2D<T> V) { return Vector2D<T>(this->x + V.x, this->y + V.y); }
    Vector2D<T> operator - (Vector2D<T> V) { return Vector2D<T>(this->x - V.x, this->y - V.y); }
    Vector2D<T> operator * (float c) { return Vector2D<T>(this->x * c, this->y * c); }
    Vector2D<T> operator / (float c) { return Vector2D<T>(this->x / c, this->y / c); }
    bool operator == (Vector2D<T> V) { return pair<T, T>(this->x, this->y) == pair<T, T>(V.x, V.y); }
};

int main()
{
    //////////////__StartInput__/////////////////
    int MazeNumber = 0;

    cout << "Choose the type of maze:" << endl
        << "1) Maze v1" << endl
        << "2) Maze v2" << endl
        << "3) Cave" << endl;
    cin >> MazeNumber; 
    while (MazeNumber > 3 || MazeNumber < 1) {
        cout << "Your input is incorrect. Chose a number from 1 to 3." << endl;
        cin >> MazeNumber;
    }

    cout << "Chose size of the maze: x y" << endl;
    cin >> M.MapWidth >> M.MapHeight;
    while (M.MapWidth < 3 || M.MapHeight < 3) {
        cout << "Your input is incorrect. Your coordinates should be more than 2." << endl;
        cin >> M.MapWidth >> M.MapHeight;
    }

    wstring congratulation;
    congratulation += L"ww    ww    wwww    ww    ww          ww          ww  wwwwww  ww      ww";
    congratulation += L"ww    ww  ww    ww  ww    ww          ww    ww    ww    ww    wwww    ww";
    congratulation += L"   ww     ww    ww  ww    ww          ww  ww  ww  ww    ww    ww  ww  ww";
    congratulation += L"   ww       wwww      wwww            wwww      wwww  wwwwww  ww    wwww";
    /////////////////////////////////////////////////////////////////////////////////////////////////

    wchar_t* screen = new wchar_t[ScreenWidth * ScreenHeight + 1]; // Массив для записи в буфер
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); // Буфер экрана
    SetConsoleActiveScreenBuffer(hConsole); // Настройка консоли
    DWORD dwBytesWritten = 0; // Для дебага


    ///////////////__MazeGenerating__/////////////////

    struct Maze* maze_1;
    switch (MazeNumber) {
    case 1: maze_1 = generate_stack_maze_v_1(M.MapWidth, M.MapHeight); break;
    case 2: maze_1 = generate_stack_maze_v_2(M.MapWidth, M.MapHeight); break;
    case 3: maze_1 = generate_cave_v_1(M.MapWidth, M.MapHeight); break;
    default: maze_1 = generate_stack_maze_v_1(M.MapWidth, M.MapHeight); break;
    }

    Vector2D<int> StartPoint(1, 1);
    Vector2D<int> FinishPoint(0, 0);

    for (int iy = 0; iy < M.MapHeight; ++iy) {
        for (int ix = 0; ix < M.MapWidth; ++ix) {
            if (maze_1->map[iy][ix] == WALL) { M.map += L"#"; }
            if (maze_1->map[iy][ix] == EMPTY) { M.map += L"."; }
            if (maze_1->map[iy][ix] == OUT_) { M.map += L"O"; 
                FinishPoint = Vector2D<int>(ix, iy);
            }
            if (maze_1->map[iy][ix] == IN_) { M.map += L"#"; 
                StartPoint = Vector2D<int>(ix, iy + 1);
            }
        }
    }

    Player.X = StartPoint.x;
    Player.Y = StartPoint.y;
    //////////////////////////////////////////////

    auto tp1 = chrono::system_clock::now(); // Переменные для подсчета
    auto tp2 = chrono::system_clock::now(); // пройденного времени

    Sleep(5);

    while (1) {
        tp2 = chrono::system_clock::now();
        chrono::duration <float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float dTime = elapsedTime.count();

        //////////////////__Control__/////////////////
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
            Player.A -= Player.AngleVelocity * dTime;
            if (Player.A > 2 * PI) { Player.A -= 2 * PI; }
            else if (Player.A < 0) { Player.A += 2 * PI; }
        }
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
            Player.A += Player.AngleVelocity * dTime;
            if (Player.A > 2 * PI) { Player.A -= 2 * PI; }
            else if (Player.A < 0) { Player.A += 2 * PI; }
        }

        Vector2D<float> normal(sin(Player.A), cos(Player.A));

        if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
            Player.X += normal.x * Player.Velocity * dTime;
            Player.Y += normal.y * Player.Velocity * dTime;

            if (Player.X < 0.2 || Player.X >= M.MapWidth - 0.2 || Player.Y < 0.2 || Player.Y >= M.MapHeight - 0.2) {
                Player.X -= normal.x * Player.Velocity * dTime;
                Player.Y -= normal.y * Player.Velocity * dTime;
            }

            if (M.map[(int)Player.Y * M.MapWidth + (int)Player.X] == '#') { 
                float bigStep = 0.8f;
                float smallStep = -0.1f;

                if (normal.x < 0) { bigStep = -bigStep; }
                if (normal.y < 0) { smallStep = -smallStep; }

                if (M.map[(int)(Player.Y + smallStep) * M.MapWidth + (int)(Player.X + bigStep)] == '#') {
                    Player.X -= normal.x * Player.Velocity * dTime;
                }

                if (Player.X < 0.2 || Player.X >= M.MapWidth - 0.2 || Player.Y < 0.2 || Player.Y >= M.MapHeight - 0.2) {
                    Player.X += normal.x * Player.Velocity * dTime;
                }


                bigStep = 0.8f;
                smallStep = -0.1f;

                if (normal.x < 0) { smallStep = -smallStep; }
                if (normal.y < 0) { bigStep = -bigStep; }

                if (M.map[(int)(Player.Y + bigStep) * M.MapWidth + (int)(Player.X + smallStep)] == '#') {
                    Player.Y -= normal.y * Player.Velocity * dTime;
                }

                if (Player.X < 0.2 || Player.X >= M.MapWidth - 0.2 || Player.Y < 0.2 || Player.Y >= M.MapHeight - 0.2) {
                    Player.Y += normal.y * Player.Velocity * dTime;
                }
            }
        }

        if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
            Player.X -= sin(Player.A) * Player.Velocity * dTime;
            Player.Y -= cos(Player.A) * Player.Velocity * dTime;

            if (Player.X < 0.2 || Player.X >= M.MapWidth - 0.2 || Player.Y < 0.2 || Player.Y >= M.MapHeight - 0.2) {
                Player.X += normal.x * Player.Velocity * dTime;
                Player.Y += normal.y * Player.Velocity * dTime;
            }

            if (M.map[(int)Player.Y * M.MapWidth + (int)Player.X] == '#') { 
                float bigStep = -0.8f;
                float smallStep = 0.1f;

                if (normal.x < 0) { bigStep = -bigStep; }
                if (normal.y < 0) { smallStep = -smallStep; }

                if (M.map[(int)(Player.Y + smallStep) * M.MapWidth + (int)(Player.X + bigStep)] == '#') {
                    Player.X += normal.x * Player.Velocity * dTime;
                }

                if (Player.X < 0.2 || Player.X >= M.MapWidth - 0.2 || Player.Y < 0.2 || Player.Y >= M.MapHeight - 0.2) {
                    Player.X -= normal.x * Player.Velocity * dTime;
                }


                bigStep = -0.8f;
                smallStep = 0.1f;

                if (normal.x < 0) { smallStep = -smallStep; }
                if (normal.y < 0) { bigStep = -bigStep; }

                if (M.map[(int)(Player.Y + bigStep) * M.MapWidth + (int)(Player.X + smallStep)] == '#') {
                    Player.Y += normal.y * Player.Velocity * dTime;
                }

                if (Player.X < 0.2 || Player.X >= M.MapWidth - 0.2 || Player.Y < 0.2 || Player.Y >= M.MapHeight - 0.2) {
                    Player.Y -= normal.y * Player.Velocity * dTime;
                }
            }
        }
    
        //////////////////////////////////////////////

        ////////////////__RayCasting__////////////////
        for (int ConsoleX = 0; ConsoleX < ScreenWidth; ConsoleX++) {
            float RayAngle = (Player.A - (V.FOV / 2.f)) + V.FOV * ((float)ConsoleX / (float)ScreenWidth);

            float Distance = 0;
            bool Mist = true;
            bool Boundary = false;

            Vector2D<float> normal(sin(RayAngle), cos(RayAngle));

            while (Mist && Distance < V.Depth) {
                Distance += 0.1f;

                Vector2D<int> point(Player.X + (normal * Distance).x, Player.Y + (normal * Distance).y);

                if (point.x < 0 || point.x >= M.MapWidth || point.y < 0 || point.y >= M.MapHeight) {
                    Distance = V.Depth;
                    Mist = false;
                }
                else if (M.map[(point.y * M.MapWidth) + point.x] == '#') {
                    Mist = false;

                    vector<pair<float, float>> p;

                    for (int tx = 0; tx < 2; tx++) 
                        for (int ty = 0; ty < 2; ty++) {
                            Vector2D<float> line((float)point.x - Player.X + (float)tx, (float)point.y - Player.Y + (float)ty);
                            float mod = sqrt(line.x * line.x + line.y * line.y);
                            float sqal = (normal.x * (line.x / mod)) + (normal.y * (line.y / mod));

                            p.push_back(make_pair(mod, sqal));
                        }
                    
                    sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) {
                        return left.first < right.first;
                        });

                    float bound = 0.005;
                    if (acos(p.at(0).second) < bound) Boundary = true;
                    if (acos(p.at(1).second) < bound) Boundary = true;
                    //if (acos(p.at(2).second) < bound) Boundary = true;
                }
            }

            int UpBoard = ((float)ScreenHeight / 2.f) - ((float)ScreenHeight / (float)Distance);
            int DownBoard = ScreenHeight - UpBoard;

            short Shade;

            if (Distance < 2)                       Shade = 0x2588; //Светлые полосы
            else if (Distance < 5)                  Shade = 0x2593;
            else if (Distance < 10)                 Shade = 0x2592;
            else if (Distance < V.Depth)            Shade = 0x2591;
            else                                    Shade = ' ';    //Темные полосы

            if (Boundary)                           Shade = '|';

            for (int ConsoleY = 0; ConsoleY < ScreenHeight; ++ConsoleY) { //printing Y lines
                if (ConsoleY < UpBoard) screen[ConsoleY * ScreenWidth + ConsoleX] = ' ';
                else if (ConsoleY <= DownBoard) screen[ConsoleY * ScreenWidth + ConsoleX] = Shade;

                else if (ConsoleY > DownBoard) {
                    float b = 1.0f - ((float)ConsoleY - ScreenHeight / 2.0) / ((float)ScreenHeight / 2.0);
                    if (b < 0.25)        Shade = 'z';
                    else if (b < 0.5)    Shade = 'x';
                    else if (b < 0.75)   Shade = '~';
                    else if (b < 0.9)    Shade = '-';
                    else                 Shade = ' ';

                    screen[ConsoleY * ScreenWidth + ConsoleX] = Shade;
                }
            }
        }

        //////////////////////////////////////////////

        //////////////__Interface__///////////////////
        swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f  ", Player.X, Player.Y, Player.A, 1.0f / dTime);
        for (int i = 35; i <= 40; ++i) {
            screen[i] = ' ';
        }

        int MiniMapWidth = 15;
        int MiniMapHeight = 15;

        for (int ConsoleX = 0; ConsoleX < MiniMapWidth; ConsoleX++) {
            for (int ConsoleY = 0; ConsoleY < MiniMapHeight; ConsoleY++) {
                if (Player.Y + ConsoleY - MiniMapHeight / 2 < 0 || Player.Y + ConsoleY - MiniMapHeight / 2 >= M.MapHeight) {
                    screen[(ConsoleY + 1) * ScreenWidth + 2 * (MiniMapWidth - ConsoleX - 1)] = '~';
                    screen[(ConsoleY + 1) * ScreenWidth + 2 * (MiniMapWidth - ConsoleX - 1) + 1] = '~';
                }
                else if (Player.X + ConsoleX - MiniMapWidth / 2 < 0 || Player.X + ConsoleX - MiniMapWidth / 2 >= M.MapWidth) {
                    screen[(ConsoleY + 1) * ScreenWidth + 2 * (MiniMapWidth - ConsoleX - 1)] = '~';
                    screen[(ConsoleY + 1) * ScreenWidth + 2 * (MiniMapWidth - ConsoleX - 1) + 1] = '~';
                }
                else if (M.map[((int)Player.Y + ConsoleY - MiniMapHeight / 2) * M.MapWidth + Player.X + (int)ConsoleX - MiniMapHeight / 2] == '#') {
                    screen[(ConsoleY + 1) * ScreenWidth + 2 * (MiniMapWidth - ConsoleX - 1)] = '#';
                    screen[(ConsoleY + 1) * ScreenWidth + 2 * (MiniMapWidth - ConsoleX - 1) + 1] = '#';
                }
                else if (M.map[((int)Player.Y + ConsoleY - MiniMapHeight / 2) * M.MapWidth + Player.X + (int)ConsoleX - MiniMapHeight / 2] == 'O') {
                    screen[(ConsoleY + 1) * ScreenWidth + 2 * (MiniMapWidth - ConsoleX - 1)] = 'O';
                    screen[(ConsoleY + 1) * ScreenWidth + 2 * (MiniMapWidth - ConsoleX - 1) + 1] = 'O';
                }
                else {
                    screen[(ConsoleY + 1) * ScreenWidth + 2 * (MiniMapWidth - ConsoleX - 1)] = ' ';
                    screen[(ConsoleY + 1) * ScreenWidth + 2 * (MiniMapWidth - ConsoleX - 1) + 1] = ' ';
                }

            }
        }

        screen[((MiniMapHeight / 2) + 1) * ScreenWidth + MiniMapWidth - 1] = 'P';
        screen[((MiniMapHeight / 2) + 1) * ScreenWidth + MiniMapWidth] = 'P';
        //////////////////////////////////////////////

        ////////////////__GameOption__////////////////
        if (Vector2D<int>(Player.X, Player.Y) == FinishPoint) {
            for (int ii = 0; ii < 4; ++ii) {
                for (int i = 0; i < 72; ++i) {
                    if (congratulation[ii * 72 + i] == 'w') {
                        screen[(ScreenHeight / 2 - 3 + ii) * ScreenWidth + ScreenWidth / 2 + i - 36] = 'W';
                    }
                }
            }
        }
        // ww    ww    wwww    ww    ww          ww          ww  wwwwww  ww      ww - 72 * 4
        // ww    ww  ww    ww  ww    ww  ww  ww  ww    ww    ww    ww    wwww    ww
        //    ww     ww    ww  ww    ww    OO    ww  ww  ww  ww    ww    ww  ww  ww
        //    ww       wwww      wwww            wwww      wwww  wwwwww  ww    wwww
        /////////////////////////////////////////////////////////////////////////////

        screen[ScreenWidth * ScreenHeight] = '\0';  // Последний символ - окончание строки
        WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth * ScreenHeight, { 0, 0 }, &dwBytesWritten); // Запись в буфер
    }
}
