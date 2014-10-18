#include "kjpsGL.hpp"
#include <algorithm>

using namespace std;
using namespace kjpsgl;

float gravity;
float x,y;
float speedX;
float speedY;
int score;
bool gameOver;

const int boxCount = 100;
float box[boxCount][2];

void reset()
{
    gravity = 3000;
    score = 0;
    gameOver = false;
    speedX =  5000;
    speedY = 10;
    x=-1000; y=500;
    for (int i=0; i<boxCount; ++i){
        float w = randomInRange(200,400);
        box[i][0]= randomInRange(0,max(0.0f,720-w));
        box[i][1]= box[i][0] + w;
    }
}

int main()
{
    init(1024,720,false,0);
    setVsync(true);

    int kaspars = loadTexture("data/kaspars.png");
    int gameOverTex = loadTexture("data/go.png");
    int wallTex = loadTexture("data/codewall.png");
    int bckg = loadTexture("data/code_bckg.png");
    int textTex = loadTexture("data/numbers.png");
    reset();

    // galvenais cikls
    while (!getKey("Escape")) // kamēr nav nospiesta escape poga
    {
        update();
        setTexture(bckg);
        setTexturingMode(TexturingMode::Manual);
        setColor(255,255,255,200);
        setTexturingRect(-x*.621f,0,getTextureWidth(bckg)-x*.621f,getTextureHeight(bckg));
        drawRectangle(0,0,1024,720);

        setColor(255,255,255,64);
        setTexturingRect(-x*.821f,0,getTextureWidth(bckg)-x*.821f,getTextureHeight(bckg));
        drawRectangle(0,0,1024,720);
        setColor(255,255,255,255);
        setTexturingMode(TexturingMode::Stretch);
        setTexture(-1);

        if (getKey("r")) reset();
        const float deltaT = getDeltaTime();
        speedX = 250 + score*10;
        speedY -= gravity*deltaT;
        y += speedY*deltaT;
        x += speedX*deltaT;

        if (!gameOver){
                if (y<0) y=0, speedY=10;
                else if (y>700) y=700, speedY=-1;
                if (getAnyKey()) speedY=800;
        }

        int p= int(x/500)+1;
        if (p<1) p = 1;
        else if (!gameOver) score = p;
        setTexture(wallTex);
        setTexturingMode(TexturingMode::Manual);
        for (int i=p-1; i<p+3; ++i){
            int cx = -x + i*500;
            if (cx<128+32 && cx>-48){
                if (y<box[i][0] || y+64>box[i][1]) gameOver = true;
            }
            setTexturingRect(cx,0,cx+128,128);
            drawRectangle(cx,0,cx+128,box[i][0]);
            drawRectangle(cx,box[i][1],cx+128,720);
        }

        setTexture(-1);
        setLineWidth(3);
        setColor(155,155,155);
        for (int i=p-1; i<p+3; ++i){
                int cx = -x + i*500;
            drawRectangleOutline(cx,0,cx+128,box[i][0]);
            drawRectangleOutline(cx,box[i][1],cx+128,720);
        }
        setColor(255,255,255);


        if (gameOver) setColor(255,0,0);
        else {
            setColor(255,255,0,64);
            drawCircle(64+32,y+32,48);
            setColor(255,255,255);
        }
        setTexture(kaspars);
        setTexturingMode(TexturingMode::Stretch);
        drawRectangle(64-4,y-4,64+64+8,y+64+8);
        setTexture(-1);

        setColor(255,255,255);

        if (gameOver){
            if (speedX>0) speedX-=deltaT;
            float w = getTextureWidth(gameOverTex);
            float h = getTextureHeight(gameOverTex);
            float x = 1024*.5 - w*.5;
            float y= 720*.5 - h*.5;
            setTexture(gameOverTex);
            drawRectangle(x,y,x+w,y+h);
        }
        setTexture(-1);

        {
            setTexture(textTex);
            setTexturingMode(TexturingMode::Manual);
            string s = toString(score);
            for (int i=0; i<s.size(); ++i){
                float x = i*64+32;
                float y=16;
                setTexturingRect(x-(90*(s[i]-'0')),y,x+90*10-(90*(s[i]-'0')),y+64);
                drawRectangle(x,y,x+90,y+64);
            }
        }

        display();
        setWindowTitle("Score: "+toString(score));
    }
}
