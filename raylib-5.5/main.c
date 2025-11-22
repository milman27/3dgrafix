#include <unistd.h>
#include "./src/raylib.h"
#define W_HEIGHT 800
#define MAXCHAR 255
#define W_WIDTH 800
int main(){
    Color color = {255, 0, 0, 255};
    InitWindow(W_WIDTH,W_HEIGHT, "Test");
    while(!WindowShouldClose()){
        BeginDrawing();
        Color color = {0,0,0,255};
        
        for(int x = 0; x < W_WIDTH; x++){
            color.r = MAXCHAR*x/W_WIDTH;
            for(int y = 0; y < W_HEIGHT; y++){
                 
                DrawPixel(x, y, color);
               
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
