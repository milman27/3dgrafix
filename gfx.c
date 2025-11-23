#include "./raylib-5.5/src/raylib.h"
#include <malloc.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080
typedef struct v3{
    union {
        struct {
            float x,y,z;
        };
        float E[3];
        struct {
        float deg1, deg2, dist;
        };
    };
}v3;
struct v3list {
    size_t number;
    v3 v3[];
};
float V3length(v3 x){
     return sqrt(x.E[0]*x.E[0]+x.E[1]*x.E[1]+x.E[2]*x.E[2]);
}
v3 V3normalize(v3 x){
    v3 y = {0};
    float mag = sqrt(x.E[0]*x.E[0]+x.E[1]*x.E[1]+x.E[2]*x.E[2]);
     y.E[0] = x.E[0]/mag;
     y.E[1] = x.E[1]/mag;
     y.E[2] = x.E[2]/mag;

     return y;
}
v3 V3add(v3 a, v3 b){
    return (v3){a.E[0] + b.E[0],a.E[1] + b.E[1],a.E[2] + b.E[2]};
}
v3 V3scalar(float a, v3 b){
    return (v3){a * b.E[0], a * b.E[1], a * b.E[2]};
}
float V3dotProduct(v3 x,v3 y){
   return (x.E[0]*y.E[0] + x.E[1]*y.E[1] + x.E[2]*y.E[2]);
}
float V3lengthSq(v3 x){
    return (x.E[0]*x.E[0]+x.E[1]*x.E[1]+x.E[2]*x.E[2]);
}
v3 updatePos(v3 pos, v3 camera, int dir){
    float scale = 0.05f;
    if(dir > 1){
        camera = (v3){.x = -camera.y, .y = camera.x, 0 };
        dir -= 3;
    }
    v3 normalized = V3normalize((v3){.x = camera.x, .y = camera.y, .z = 0});
    v3 pos2 = {.x = pos.x + normalized.x*scale*dir, .y = pos.y + normalized.y*scale*dir, .z = pos.z}; 
    return pos2;
}
v3 V3rotate(v3 axis, float angle, v3 vector){
    if(angle == 0)
        return vector;
    v3 horizontal;
    if(V3dotProduct(axis, vector)*V3dotProduct(axis,vector) < 0.00001f){
       horizontal = V3normalize(vector);    
    }else{
       horizontal = V3normalize((v3){.x = -axis.y, .y =axis.x, 0});
    }
    axis = V3normalize(axis);
    v3 normal = V3normalize((v3){.x = axis.y*horizontal.z - axis.z*horizontal.y, .y = axis.z*horizontal.x - axis.x*horizontal.z, .z = axis.x*horizontal.y - axis.y*horizontal.x} );
    float len = V3length(vector);
    vector = V3normalize(vector);
    float dotAxis = V3dotProduct(axis, vector);
    float dotNormal = V3dotProduct(vector, normal);
    float dotHoriz = V3dotProduct(vector, horizontal);
    float lenHoriz = (sinf(angle)*dotNormal + cosf(angle)*dotHoriz);
    float lenNormal = (cosf(angle)*dotNormal - sinf(angle)*dotHoriz);
    v3 l = V3scalar(lenHoriz, horizontal);
    v3 j = V3scalar(lenNormal, normal);
    v3 p = V3scalar(dotAxis, axis);
    vector = V3scalar(len, V3add(l, V3add(j, p)));
    return vector;
}
void updateView(v3* camera, float dx, float dy, float dth){
    *camera = V3normalize(*camera);
    v3 horizontal = V3normalize((v3){.x = -camera->y, .y =camera->x, 0});
    horizontal = V3rotate(*camera, dth, horizontal);
    v3 normal = V3normalize((v3){.x = camera->y*horizontal.z - camera->z*horizontal.y, .y = camera->z*horizontal.x - camera->x*horizontal.z, .z = camera->x*horizontal.y - camera->y*horizontal.x} );
    //*camera = V3normalize(V3add(V3scalar(dx, horizontal), *camera));
    *camera = V3rotate(normal, -dx, *camera);
    if(camera->z*camera->z > 0.999f && (camera->z - dy)*(camera->z - dy) > 0.999f){
    }else{
        *camera = V3rotate(horizontal, -dy, *camera);
    //    *camera = V3normalize(V3add(V3scalar(-dy, normal), *camera));
    } 
}

struct v3list * worldToCamera(struct v3list * world, v3 camera, v3 cameraPos, float dth){
    struct v3list * screen = malloc(world->number * 2 * sizeof(v3)); 
    screen->number = world->number;
    camera = V3normalize(camera);
    v3 horizontal = V3normalize((v3){.x = -camera.y, .y =camera.x, 0});
    horizontal = V3rotate(camera, dth, horizontal);
    v3 normal = V3normalize((v3){.x = camera.y*horizontal.z - camera.z*horizontal.y, .y = camera.z*horizontal.x - camera.x*horizontal.z, .z = camera.x*horizontal.y - camera.y*horizontal.x} );
    for(int i = 0; i < screen->number; i++){
        float dotHoriz = V3dotProduct(horizontal, V3add(world->v3[i], V3scalar(-1, cameraPos)));
        float dotNorm = V3dotProduct(normal, V3add(world->v3[i], V3scalar(-1, cameraPos)));
        float dotLength = V3dotProduct(horizontal, V3add(world->v3[i], V3scalar(-1, cameraPos)));
        v3 projy = V3add(V3scalar(-dotHoriz, horizontal), V3add(world->v3[i], V3scalar(-1, cameraPos)));
        v3 projx = V3add(V3scalar(-dotNorm, normal), V3add(world->v3[i], V3scalar(-1, cameraPos)));
        v3 projlen = V3add(V3scalar(-dotLength, horizontal), V3add(world->v3[i], V3scalar(-1, cameraPos)));
        float anglex = asinf(V3dotProduct(V3normalize(projx),V3normalize(horizontal)));
        float angley = -asinf(V3dotProduct(V3normalize(projy),V3normalize(normal)));
        float length = V3dotProduct(V3normalize(projlen),V3normalize(camera));
        screen->v3[i] = (v3){anglex, angley, length};
    }
    return screen;
}
Image createWindow(int x, int y){
    InitWindow(x,y, "gfx");
     static Image image = {};
     image = LoadImageFromScreen();
    return image;
}
void displayScreen(struct v3list * pixels, Image img){
    ImageClearBackground(&img, (Color){0,0,0,255});
    for(int i = 0; i < pixels->number; i++){
       if(pixels->v3[i].deg1 < PI/4 && pixels->v3[i].deg1 > -PI/4
           && pixels->v3[i].deg2 < PI/4 && pixels->v3[i].deg2 > -PI/4 && pixels->v3[i].dist > 0)
       {
        float PId2 = PI/2;
        int x = (int)(((pixels->v3[i].deg1 + (PI/4))*WIN_WIDTH)/PId2);
        int y = (int)(((pixels->v3[i].deg2 + (PI/4))*WIN_HEIGHT)/PId2);
        
        ImageDrawPixel(&img, x, y, (Color){255,255,255,255});
       }
    }
    
    Texture2D texture = LoadTextureFromImage(img);
    DrawTexture(texture, 0, 0 ,(Color){255,255,255,255});
    EndDrawing();
    UnloadTexture(texture);
}

int main(){
    v3 cameraVector = {.x = 1.0f, .y = 0.5f, .z = 0.0f};
    float cameraTilt = 0;
    v3 playerPos = { 0,0,0};
    struct v3list * worldpoints = malloc(400*sizeof(v3));
    worldpoints->number = 351;
    for(int i = 0; i < worldpoints->number; i++){
        worldpoints->v3[i].E[0] = (float)(i/11) ;
        worldpoints->v3[i].E[1] = (float)(i%11) -5;
        worldpoints->v3[i].E[2] = (i % 2) ?  1.0f : -1.0f ;
    }
    worldpoints->v3[320] = (v3){0.25, 0.25, 0.25}; 
    worldpoints->v3[329] = (v3){0.5, 0.5, 0.5}; 
    worldpoints->v3[350] = (v3){0.25, 0.25, 0.25}; 
    worldpoints->v3[349] = (v3){0.5, 0.5, 0.5}; 
    Image img = createWindow(WIN_WIDTH, WIN_HEIGHT);
    struct v3list * pixels; 
    float sens = 0.001f;
    while(!WindowShouldClose()){
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON) && IsCursorOnScreen()){
           DisableCursor(); 
        }
        if(IsKeyDown(KEY_K)){
            EnableCursor();
        }
        worldpoints->v3[350] = V3rotate((v3){1,1,0}, sens*50, worldpoints->v3[350]);
        worldpoints->v3[349] = V3rotate((v3){1,1,0}, sens*50, worldpoints->v3[349]);
        pixels = worldToCamera(worldpoints, cameraVector, playerPos, cameraTilt);
        Vector2 dM = GetMouseDelta();
        updateView(&cameraVector, dM.x*sens, dM.y*sens, cameraTilt);
        if(IsKeyDown(KEY_W)){
            playerPos = updatePos(playerPos, cameraVector, 1);
        }
        if(IsKeyDown(KEY_S)){
            playerPos = updatePos(playerPos, cameraVector, -1);
        }
        if(IsKeyDown(KEY_A)){
            playerPos = updatePos(playerPos, cameraVector, 2);
        }
        if(IsKeyDown(KEY_D)){
            playerPos = updatePos(playerPos, cameraVector, 4);
        }
        if(IsKeyDown(KEY_Q)){
            cameraTilt -= 0.01f;
        }
        if(IsKeyDown(KEY_E)){
            cameraTilt += 0.01f;
        }
        if(IsKeyDown(KEY_SPACE)){
           playerPos.z += 0.05f; 
        }
        if(IsKeyDown(KEY_LEFT_CONTROL)){
            playerPos.z -= 0.05f;
        }
        if(playerPos.z > 0.0001f){
            playerPos.z -= 0.02f;
        }
        printf("playerPos:%f,%f,%f\ncameraPos:%f,%f,%f\nfps:%d\n",playerPos.x, playerPos.y,playerPos.z,cameraVector.x,cameraVector.y,cameraVector.z, GetFPS());
        displayScreen(pixels, img);
        free(pixels);
    }
}
