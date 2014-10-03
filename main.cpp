#include "kjpsGL.hpp"

using namespace std;
using namespace kjpsgl;

int main()
{
    init(1024,720,false,0);
    setVsync(true);

    int tex = loadTexture("data/kaspars.png");

    // galvenais cikls
    while (!getKey("Escape")) // kamēr nav nospiesta escape poga
    {
        update();
        setTexture(tex);
        setColor(255,0,0);
        drawLine(0,0,666,0,64);
        drawRectangle(0,0,64,64);
        display();
    }
}
