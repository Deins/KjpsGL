#include "kjpsGL.hpp"

using namespace std;
using namespace kjpsgl;

const float gravity = 50;
float x,y;
float speedX;
float speedY;
int score;
bool gameOver;

const int boxCount = 100;
float box[boxCount][2];

void reset()
{
    score = 0;
    gameOver = false;
    speedX = 3;
    speedY = 10;
    x=-1000; y=500;
    for (int i=0; i<boxCount; ++i){
        float w = randomInRange(150,400);
        box[i][0]= randomInRange(0,720-w);
        box[i][1]= box[i][0] + w;
    }
}

int main()
{
    init(1024,720);
    setVsync(true);
    reset();
    int kaspars = loadTexture("data/kaspars.png");
    int gameOverTex = loadTexture("data/go.png");

    // galvenais cikls
    while (!getKey("Escape")) // kamēr nav nospiesta escape poga
    {
        update();
        clearScreen(32,96,200);
        if (getKey("r")) reset();
        const float deltaT = getDeltaTime();
        speedY -= gravity*deltaT;
        y += speedY;
        x += speedX;

        if (!gameOver){
                if (y<0) y=0, speedY=10;
                else if (y>700) y=700, speedY=-1;
                if (getAnyKey()) speedY=12;
        }else{
                if (speedX>0) speedX-=deltaT;
        }

        int p= int(x/500)+1;
        if (p<1) p = 1;
        else if (!gameOver) score = p;
        for (int i=p-1; i<p+8; ++i){
            int cx = -x + i*500;
            if (cx<128+32 && cx>-48){
                if (y<box[i][0] || y+64>box[i][1]) gameOver = true;
            }
            drawRectangle(cx,0,cx+128,box[i][0]);
            drawRectangle(cx,box[i][1],cx+128,720);
        }

        if (gameOver) setColor(255,0,0);
        setTexture(kaspars);
        drawRectangle(64-4,y-4,64+64+8,y+64+8);
        setColor(255,255,255);

        if (gameOver){
            setTexture(gameOverTex);
            int w =296; int h= 48;
            //drawRectangle()
        }
        setTexture(-1);

        display();
        setWindowTitle("Score: "+toString(score));
    }
}
