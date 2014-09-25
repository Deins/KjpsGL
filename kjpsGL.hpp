#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <GL/gl.h>
#include <GL/glu.h>

namespace kjpsgl
{
    struct Vec2
    {
        float x,y;
        Vec2(float x = 0.0f, float y = 0.0f);
        float magnitude();
        Vec2 normalized();
    };

    struct Color
    {
        unsigned char r,g,b,a;
        Color(unsigned char r=0, unsigned char g=0, unsigned char b=0, unsigned char a = 255);
    };

    template <typename T>
    std::string toString(const T& v){
        std::ostringstream ss;
        ss << v;
        return ss.str();
    }

    void init(int width = 0, int height = 0, bool fullscreen = false, int msaa = 8);
    void update();
    void display();

    void setWindowTitle(const std::string&);
    int getWindowWidth();
    int getWindowHeight();
    float getDeltaTime();
    bool setVsync(bool state);
    void clearScreen(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0);

    void setView(float left, float bottom, float right, float top);
    void setColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);
    void drawTriangle(float ax, float ay, float bx, float by, float cx, float cy);
    void drawRectangle(float left, float bottom, float right, float top);
    void drawCircle(float x, float y, float r, int segments = 64);

    void drawTriangleOutline(float ax, float ay, float bx, float by, float cx, float cy,float w = 1);
    void drawRectangleOutline(float left, float bottom, float right, float top,float w = 1);
    void drawCircleOutline(float x, float y, float r, int segments = 64, float w = 1);
    void drawLine(float x1, float y1, float x2, float y2,float w = 1,bool caps = false);
    void drawLine(const std::vector<Vec2>& points,float w=1,bool caps=true);

    void drawArrays(const std::vector<Vec2>& vertices, unsigned mode = GL_TRIANGLES);
    void drawArrays(const std::vector<Vec2>& vertices,const std::vector<Color>& colors, unsigned mode = GL_TRIANGLES);
    void drawArrays(const std::vector<Vec2>& vertices,const std::vector<Vec2>& uv, unsigned mode = GL_TRIANGLES);
    void drawArrays(const std::vector<Vec2>& vertices,const std::vector<Vec2>& uv,const std::vector<Color>& colors, unsigned mode = GL_TRIANGLES);

    int loadTexture(const std::string& filename);
    void destroyTexture(int id);
    void setTexture(int id = -1);

    bool getKey(const std::string&);
    bool getMouseButton(int button = 1);
    float getMouseX();
    float getMouseY();
    int getMouseWheel();
}
