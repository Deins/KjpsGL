#include "kjpsGL.hpp"

using namespace std;
using namespace kjpsgl;

int main()
{
    init(1024,720);

    // galvenais cikls
    while (!getKey("Escape")) // kamēr nav nospiesta escape poga
    {
        update();

        clearScreen(0,0,0);
        setColor(44,44,44);
        drawRectangle(512-111,350-250,512+111,350+250);
        setColor(255,0,0);
        drawCircle(512,350+155,64);
        setColor(255,255,0);
        drawCircle(512,350,64);
        setColor(0,255,0);
        drawCircle(512,350-155,64);

        setColor(0,0,0,122);
        for (int i=0; i<3; ++i){
            if ((getTime()/1000)%3!=i)
                drawCircle(512,350-155+155*i,64);
        }
        display();
    }
}
