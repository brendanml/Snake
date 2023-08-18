#include <stdio.h>
#include "raylib.h"
#include <cstdlib>
#include <iostream>
#include <deque>
#include <raymath.h>
using namespace std;

int cellSize = 25;
int cellCount = 25;
double currentTime = 0;
double lastUpdateTime = 0;



bool TimePassed(double interval) {
    double currentTime = GetTime();
    if(currentTime-lastUpdateTime>=interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake {
    public:
    deque<Vector2> body = {Vector2{4,4},Vector2{4,5},Vector2{4,6}};
    Vector2 direction = {1,0};
    bool alive = true;
    bool eating = false;
    void draw() {
        checkDirection();
        for(int i = 0; i<body.size();i++) { 
            Rectangle rect = Rectangle{(float)body[i].x*cellSize,(float)body[i].y*cellSize,(float)cellSize,(float)cellSize};
            if(alive) {
                DrawRectangleRounded(rect,.5,3,Color{255,255,255,255});
                if(i==0 && eating) {
                    DrawCircle((float)body[i].x*cellSize+(cellSize/4),(float)body[i].y*cellSize+(cellSize/4),3,BLACK);
                    DrawCircle((float)body[i].x*cellSize+(16),(float)body[i].y*cellSize+(cellSize/4),3,BLACK);
                }
            } else {
                DrawRectangleRounded(rect,.5,3,Color{240,93,94,255});
            }
        }
        if (!alive) {
            DrawText("you died.", 20,20,20,BLACK);
            DrawText("press space to restart", cellSize*cellCount-500,cellSize*cellCount-100,20,BLACK);
        }
    }
    void update() {
        body.pop_back();
        body.push_front(Vector2Add(body[0], direction));
        eating = false;
    }
    void checkDirection() {
        if(IsKeyPressed(KEY_DOWN) && direction.y != -1) {
            direction = {0,1};
        }
        if(IsKeyPressed(KEY_UP) && direction.y != 1) {
            direction = {0,-1};
        }
        if(IsKeyPressed(KEY_RIGHT)&& direction.x != -1) {
            direction = {1,0};
        }
        if(IsKeyPressed(KEY_LEFT)&& direction.x != 1) {
            direction = {-1,0};
        }
        return;
    }
    
};


class Food {
    public:
    Vector2 position;
    Texture texture;
    //constructor for food
    Food() {
        Image img = LoadImage("./resources/carrot-square.png");
        texture = LoadTextureFromImage(img);
        UnloadImage(img);
        position = GenerateRandomPos();
    }
    //destructor for unloading resources upon object deletion
    ~Food() {
        UnloadTexture(texture);
    }
    void draw() {
        DrawTexture(texture, cellSize*position.x, cellSize*position.y, WHITE);
    }
    Vector2 GenerateRandomPos() {
        float x = GetRandomValue(0, cellCount-1);
        float y = GetRandomValue(0, cellCount-1);
        return Vector2{x,y};
    }
};

class Game {
    public:
    Snake snake = Snake();
    Food food = Food();

    void draw() {
        food.draw();
        snake.draw();
    }
    void update() {
        if(snake.alive) {
            snake.update();
        }
    }
    void check() {
        checkEating();
        checkCollision();
    }
    void checkEating() {
        if(Vector2Equals(snake.body[0],food.position)) {
            cout << "eating!" << endl;
            snake.eating = true;
            food.position = food.GenerateRandomPos();
            Vector2 tail = snake.body[-1];
            Vector2 direction = snake.direction;
            Vector2 extension = Vector2Subtract(tail,direction);
            snake.body.push_back(extension);
        }
    }
    void checkCollision() {
        if(snake.body[0].x < 0 || snake.body[0].y < 0 || snake.body[0].x >= cellSize || snake.body[0].y >= cellSize) {
            cout << "you lose" << endl;
            snake.alive = false;
        }
        for(int i = 1;i<snake.body.size();i++) {
            if(Vector2Equals(snake.body[0],snake.body[i])) {
                snake.alive = false;
            }
        }
    }
};

int main() {
    InitWindow(cellSize*cellCount, cellSize*cellCount, "snek");
    SetWindowPosition(200, 200);   
    SetTargetFPS(60);

    Game game = Game();
    while(!WindowShouldClose()) {
        BeginDrawing();
        //without this it would leave trace of ball (redraw)
        ClearBackground((Color){170,246,131,0});
        game.check();
        if(TimePassed(0.2) && game.snake.alive) {
            game.update(); 
        }
        game.draw();
        if(IsKeyPressed(KEY_SPACE)) {
            game.snake = Snake();
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}