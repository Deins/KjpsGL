#include "kjpsGL.hpp"
#include <cmath>

using namespace std;
using namespace kjpsgl;

int main()
{
    init(1024,600,false,8);
    setVsync(true);

    int frame=1;
    float t = 0;
    char b=122;
    int tex = loadTexture("data/bricks.png");
    while (!getKey("Escape"))
    {
        b+= getMouseWheel()*8;
        t+=getDeltaTime();
        ++frame;
        update();
        clearScreen(b,b,b);
        setWindowTitle("FPS: "+toString(1/getDeltaTime()));

        setColor(255,255,255);
        setTexture(tex);
        drawRectangle(256,256,512,512);
        setTexture(-1);

        setColor(0,0,255);
        if (getMouseButton(1)) setColor(0,255,0);
        drawLine(0,0,0+getMouseX(),0+getMouseY(),3);

        vector<Vec2> v {Vec2(100,100), Vec2(200,100), Vec2(200,200)};
        vector<Color> c {Color(255,0,0),Color(0,255,0),Color(0,0,0,0)};
        drawArrays(v,c);

        drawTriangle(200,200,300,300,400,100);
        setColor(255,0,0);
        drawRectangle(500,0,700,88);

        setColor(0,0,0,255);
        drawCircle(200,100,32);
        setColor(255,255,255);
        drawCircleOutline(200,100,32,64,3);

        //setView(1024*.5,0,0,600*.5);

        setTexture(tex);
        drawTriangle(333,444,255,333,111,666);
        drawCircle(800,333,60);
        drawRectangle(400,400,500,500,0,0,.75,.5);
setTexture(-1);

        display();
    }
    return 0;
}
