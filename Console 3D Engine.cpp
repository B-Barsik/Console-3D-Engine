#include <iostream>
#include <Windows.h>
#include <chrono>

#define PI 3.1415926f

using namespace std;

int ScreenWidth = 120; // Ширина консольного окна
int ScreenHeight = 40; // Высота консольного окна

struct Object {
    float X = 0.0f;
    float Y = 0.0f;
    float A = 0.0f;
    float AngleVelocity = 1.5f;
    float Velocity = 5.f;
} Player;

struct Map {
    int MapHeight = 16;     // Высота игрового поля
    int MapWidth = 16;      // Ширина игрового поля
    wstring map;            // Строковый массив
} M;

struct View {
    float FOV = PI / 3;    // Угол обзора (поле видимости)
    float Depth = 30.0f;   // Максимальная дистанция обзора
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
};

int main()
{
    wchar_t* screen = new wchar_t[ScreenWidth * ScreenHeight + 1]; // Массив для записи в буфер
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); // Буфер экрана
    SetConsoleActiveScreenBuffer(hConsole); // Настройка консоли
    DWORD dwBytesWritten = 0; // Для дебага

    M.map += L"##########....##";
    M.map += L"#..............#";
    M.map += L"#.###.....#....#";
    M.map += L"#.###..........#";
    M.map += L"#.###..........#";
    M.map += L"#..............#";
    M.map += L"#..............#";
    M.map += L"#..............#";
    M.map += L"####.....###.###";
    M.map += L"#..#.....#.....#";
    M.map += L"##.#.....#.....#";
    M.map += L"#..#.....#.....#";
    M.map += L"#........#######";
    M.map += L"#..#...........#";
    M.map += L"#..#...........#";
    M.map += L"################";
    
    Player.X = 8;
    Player.Y = 1;

    auto tp1 = chrono::system_clock::now(); // Переменные для подсчета
    auto tp2 = chrono::system_clock::now(); // пройденного времени

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

        if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
            Player.X += sin(Player.A) * Player.Velocity * dTime; 
            Player.Y += cos(Player.A) * Player.Velocity * dTime; 

            if (M.map[(int)Player.Y * M.MapWidth + (int)Player.X] == '#') { // Если столкнулись со стеной, то откатываем шаг
                Player.X -= sin(Player.A) * Player.Velocity * dTime;
                Player.Y -= cos(Player.A) * Player.Velocity * dTime;
            }
        }

        if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
            Player.X -= sin(Player.A) * Player.Velocity * dTime;
            Player.Y -= cos(Player.A) * Player.Velocity * dTime;

            if (M.map[(int)Player.Y * M.MapWidth + (int)Player.X] == '#') { // Если столкнулись со стеной, то откатываем шаг
                Player.X += sin(Player.A) * Player.Velocity * dTime;
                Player.Y += cos(Player.A) * Player.Velocity * dTime;
            }
        }
        //////////////////////////////////////////////

        ////////////////__RayCasting__////////////////
        for (int ConsoleX = 0; ConsoleX < ScreenWidth; ++ConsoleX) {
            float RayAngle = (Player.A - (V.FOV / 2.f)) + V.FOV * ((float)ConsoleX / ScreenWidth);

            float Distance = 0;
            bool Mist = true;

            Vector2D<float> normal(sin(RayAngle), cos(RayAngle));
            Vector2D<int> point;

            while (Mist && Distance < V.Depth) {
                Distance += 0.1;
                point = Vector2D<int>(Player.X, Player.Y) + Vector2D<int>((normal * Distance).x, (normal * Distance).y);

                if (point.x < 0 || point.x > M.MapWidth || point.y < 0 || point.y > M.MapHeight) {
                    Distance = V.Depth;
                    Mist = false;
                }
                else if (M.map[(point.y * M.MapWidth) + point.x] == '#') {
                    Mist = false;
                }
            }

            float UpBoard = ScreenHeight / 2 - (ScreenHeight / Distance);
            float DownBoard = ScreenHeight - UpBoard;

            short Shade;

            if (Distance < V.Depth / 10.f)          Shade = 0x2588; //Светлые полосы
            else if (Distance < V.Depth / 6.f)      Shade = 0x2593;
            else if (Distance < V.Depth / 4.f)      Shade = 0x2592;
            else if (Distance < V.Depth)            Shade = 0x2591;
            else                                    Shade = ' ';    //Темные полосы

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
                if (Player.Y + ConsoleY - MiniMapHeight / 2 < 0 || Player.Y + ConsoleY - MiniMapHeight / 2 >= 16) {
                    screen[(ConsoleY + 1) * ScreenWidth + MiniMapWidth - ConsoleX - 1] = '~';
                }
                else if (Player.X + ConsoleX - MiniMapWidth / 2 < 0 || Player.X + ConsoleX - MiniMapWidth / 2 >= 16) {
                    screen[(ConsoleY + 1) * ScreenWidth + MiniMapWidth - ConsoleX - 1] = '~';
                }
                else if (M.map[((int)Player.Y + ConsoleY - MiniMapHeight / 2) * M.MapWidth + Player.X + (int)ConsoleX - MiniMapHeight / 2] == '#') {
                    screen[(ConsoleY + 1) * ScreenWidth + MiniMapWidth - ConsoleX - 1] = '#';
                }
                else {
                    screen[(ConsoleY + 1) * ScreenWidth + MiniMapWidth - ConsoleX - 1] = '.';
                }

            }
        }

        screen[((MiniMapHeight / 2) + 1) * ScreenWidth + (MiniMapWidth / 2)] = 'P';
        //////////////////////////////////////////////













        screen[ScreenWidth * ScreenHeight] = '\0';  // Последний символ - окончание строки
        WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth * ScreenHeight, { 0, 0 }, &dwBytesWritten); // Запись в буфер
    }

}
