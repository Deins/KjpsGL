#include "kjpsGL.hpp"
#include "sdl.h"
#include "lodepng.h"

#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <array>

#define fatalError(x) do{ std::string msg; \
    msg += x; msg += " see "; msg += __FUNCTION__; msg+="()\nerror thrown from: "; \
    msg += toString(__FILE__); msg += " line "+toString(__LINE__); msg +="\n"; \
    std::cerr << "ERROR: " << msg; std::cerr.flush();\
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,("Function "+std::string(__FUNCTION__)+"() ERROR! ").c_str(),msg.c_str(),nullptr ); \
    assert(false); }while(0)

#define errorAssert(x,msg) do{if (!x) {fatalError(msg);}}while(0)

namespace kjpsgl
{
using namespace std;
//  variables:
SDL_Window* window = nullptr;
SDL_GLContext context;
Color color;
unsigned long long lastFrame;
float deltaTime;
vector<GLuint> textures;
vector<Vec2> textureSizes;
int boundTexture = -1;
uint64_t randomSeed;

array<bool,256> keys;
bool anyKey = false;
array<bool,16> mouseButtons;
Vec2 mousePos;
int mouseWheel;
bool inited = false;

//  struct & class implementations:
Vec2::Vec2(float x, float y): x(x), y(y) {}
float Vec2::magnitude()
{
    return sqrtf(x*x+y*y);
}
Vec2 Vec2::normalized()
{
    float m=magnitude();
    return Vec2(x/m,y/m);
}
float Vec2::dot(const Vec2& a, const Vec2& b)
{
    return (a.x*b.x)+(a.y*b.y);
}

Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a):r(r), g(g), b(b), a(a) {}

//  Function implementations:
void init(int width, int height, bool fullscreen,int msaa)
{
    errorAssert(!isInit(),"kjpsgl is already initialized!");
    {
        // Init SDL & window
        if (SDL_Init(SDL_INIT_EVERYTHING ^ SDL_INIT_HAPTIC) != 0) fatalError("SDL_Init failed!");
        inited = true;

        // Request opengl 2.0 context.
        if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2) != 0) fatalError("SDL_GL_SetAttribute fail!");
        if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0) != 0) fatalError("SDL_GL_SetAttribute fail!");

        // Buffer Sizes
        if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0) fatalError("SDL_GL_SetAttribute fail!");
        if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0) != 0) fatalError("SDL_GL_SetAttribute fail!");
        if (SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0) != 0) fatalError("SDL_GL_SetAttribute fail!");

        if (msaa >0)
        {
            if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1) !=0) fatalError("Can't create MSAA buffers!");
            if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaa) !=0) fatalError("Can't initialize MSAA buffers!");
        }

        // Open Window
        unsigned flags = SDL_WINDOW_OPENGL;
        bool nativeResolution = width<=0 || height<=0;
        if (fullscreen)
        {
            flags |= SDL_WINDOW_FULLSCREEN;
            if (nativeResolution)
            {
                SDL_DisplayMode dm;
                if (SDL_GetDesktopDisplayMode(0, &dm) == 0)
                {
                    width = dm.w;
                    height = dm.h;
                }
            }
        }
        else if (nativeResolution)
        {
            flags |= SDL_WINDOW_MAXIMIZED;
            width = 800;
            height = 600;
        }
        window = SDL_CreateWindow("KjpsGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
        if (!window) fatalError("SDL_CreateWindow failed!");
        SDL_GetWindowSize(window, &width, &height);

        // Create GL context
        context = SDL_GL_CreateContext(window);
        if (!context) fatalError("GL context creation failed!");

        SDL_GL_MakeCurrent(window, context);

        // Set up opengl
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Alpha blending
        glShadeModel(GL_SMOOTH);
        glViewport(0,0,width,height);
        resetView();

        lastFrame = SDL_GetPerformanceCounter();
        randomSeed = lastFrame;
        setColor(255,255,255); // set default color to white
    }
}

void update()
{
    errorAssert(isInit(),"kjpsgl not initialized!");
    {
        // calculate delta time
        unsigned long long delta = lastFrame;
        lastFrame = SDL_GetPerformanceCounter();
        delta = lastFrame - delta;
        deltaTime = float(double(delta)/double(SDL_GetPerformanceFrequency()));
    }
    mouseWheel = 0;
    SDL_Event event;
    anyKey = false;
    /* Grab all the events off the queue. */
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            exit(0);
            break;

        case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
//					case (SDL_WINDOWEVENT_FOCUS_GAINED) :
//						SDL_ShowCursor(0);
//						break;
//					case (SDL_WINDOWEVENT_FOCUS_LOST) :
//						SDL_ShowCursor(1);
//						break;
            }
            break;

        case(SDL_KEYDOWN) :
        {
            const unsigned code = event.key.keysym.scancode;
            assert(code < keys.size());
            keys[code] = 1;
            anyKey = true;
            break;
        }

        case(SDL_KEYUP) :
        {
            const unsigned code = event.key.keysym.scancode;
            assert(code < keys.size());
            keys[code] = 0;
            break;
        }

        case(SDL_MOUSEBUTTONDOWN) :
        {
            assert(event.button.button < 16);
            mouseButtons[event.button.button ] = 1;
            break;
        }

        case(SDL_MOUSEBUTTONUP) :
        {
            assert(event.button.button < 16);
            mouseButtons[event.button.button ] = 0;
            break;
        }

        case(SDL_MOUSEMOTION) :
            mousePos = Vec2(event.motion.x, event.motion.y);
            break;

        case(SDL_MOUSEWHEEL) :
            mouseWheel = event.wheel.y;
            break;

        default:
            //mInput.processEvent(event);
            break;
        }
    }
}

void display()
{
    errorAssert(isInit(),"kjpsgl not initialized!");
    SDL_GL_SwapWindow(window);
}

void setWindowTitle(const string& txt)
{
    errorAssert(isInit(),"kjpsgl not initialized!");
    SDL_SetWindowTitle(window,txt.c_str());
}

void clearScreen(unsigned char red, unsigned char green, unsigned char blue)
{
    glClearColor ( red/255.0f, green/255.0f,blue/255.0f,255.0f);
    glClear ( GL_COLOR_BUFFER_BIT );
}

void setColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
    color = Color(red,green,blue,alpha);
}

void drawTriangle(float ax, float ay, float bx, float by, float cx, float cy)
{
    if (boundTexture>=0)
    {
        const Vec2 s = boundTexture>=0?Vec2(getTextureWidth(boundTexture),getTextureHeight(boundTexture)):Vec2(1,1);
        drawArrays({Vec2(ax,ay),Vec2(bx,by),Vec2(cx,cy)},
        {Vec2(ax/s.x,ay/s.y),Vec2(bx/s.x,by/s.y),Vec2(cx/s.x,cy/s.y)},GL_TRIANGLES);
    }
    else
    {
        drawArrays({Vec2(ax,ay),Vec2(bx,by),Vec2(cx,cy)},vector<Color>(6,color),GL_TRIANGLES);
    }
}

void drawRectangle(float left, float bottom, float right, float top)
{
    if (boundTexture>=0)
    {
        drawArrays(
        {Vec2(left,bottom),Vec2(right,bottom),Vec2(right,top),Vec2(left,top)},
        {Vec2(0,1),Vec2(1,1),Vec2(1,0),Vec2(0,0)},
        {color,color,color,color},
        GL_QUADS);
    }
    else
    {
        drawArrays(
        {Vec2(left,bottom),Vec2(right,bottom),Vec2(right,top),Vec2(left,top)},
        {color,color,color,color},
        GL_QUADS);
    }
}

void drawRectangle(float left, float bottom, float right, float top, float uvLeft,float uvBottom, float uvRight,float uvTop)
{
    drawArrays(
    {Vec2(left,bottom),Vec2(right,bottom),Vec2(right,top),Vec2(left,top)},
    {Vec2(uvLeft,uvBottom),Vec2(uvRight,uvBottom),Vec2(uvRight,uvTop),Vec2(uvLeft,uvTop)},
    {color,color,color,color},
    GL_QUADS);
}

void drawLine(float x1, float y1, float x2, float y2,float w)
{
    drawLine({Vec2(x1,y1), Vec2(x2,y2)},w);
}

void drawLine(const vector<Vec2>& points,float w)
{
    w*=.5f;
    vector<Vec2> verts;
    verts.reserve(points.size()*4);

    verts.resize(2); // first two reserved for later
    for (int i=1; i<points.size(); ++i)
    {
        Vec2 dir = Vec2(points[i].x-points[i-1].x,points[i].y-points[i-1].y).normalized();
        // first normals instead of vertices
        swap(dir.x,dir.y);
        verts.emplace_back(-dir.x,dir.y);
        verts.emplace_back(dir.x,-dir.y);
    }
    // first two values
    verts[0] = verts[2];
    verts[1] = verts[3];

    for (int i=2; i< verts.size()-2; ++i)
    {
        float d = Vec2::dot(verts[i],verts[i+2])+1;
        verts[i].x += verts[i+2].x;
        verts[i].y += verts[i+2].y;
        //verts[i] = verts[i].normalized();

        verts[i].x *= 1/d;
        verts[i].y *= 1/d;

        verts[i].x *= w;
        verts[i].y *= w;
    }

    if (points.front().x == points.back().x && points.front().y == points.back().y)
    {
        {
            int i = 0;
            float d = Vec2::dot(verts[i],verts[verts.size()-2])+1;
            verts[i].x += verts[verts.size()-2].x;
            verts[i].y += verts[verts.size()-2].y;
            //verts[i] = verts[i].normalized();

            verts[i].x *= 1/d;
            verts[i].y *= 1/d;

            verts[i].x *= w;
            verts[i].y *= w;
        }
        {
            int i = 1;
            float d = Vec2::dot(verts[i],verts[verts.size()-1])+1;
            verts[i].x += verts[verts.size()-1].x;
            verts[i].y += verts[verts.size()-1].y;
            //verts[i] = verts[i].normalized();

            verts[i].x *= 1/d;
            verts[i].y *= 1/d;

            verts[i].x *= w;
            verts[i].y *= w;
        }
        verts[verts.size()-2] = verts[0];
        verts[verts.size()-1] = verts[1];
    }
    else
    {
        verts[0] = verts[0].normalized();
        verts[1] = verts[1].normalized();
        verts[verts.size()-2] = verts[verts.size()-2].normalized();
        verts[verts.size()-1] = verts[verts.size()-1].normalized();
        verts[0].x *= w;
        verts[1].x *= w;
        verts[verts.size()-2].x *= w;
        verts[verts.size()-1].x *= w;
        verts[0].y *= w;
        verts[1].y *= w;
        verts[verts.size()-2].y *= w;
        verts[verts.size()-1].y *= w;
    }

    for (int i=0; i<points.size(); ++i)
    {
        verts[i*2].x += points[i].x;
        verts[i*2].y += points[i].y;
        verts[i*2+1].x += points[i].x;
        verts[i*2+1].y += points[i].y;
    }

    if (boundTexture>=0)
    {
        vector<Vec2> uv = verts;
        const Vec2 s (getTextureWidth(boundTexture),getTextureHeight(boundTexture));
        for (int i=0; i<verts.size(); ++i)
        {
            uv[i].x/=s.x;
            uv[i].y/=s.y;
        }
        drawArrays(verts,uv,vector<Color>(verts.size(),color),GL_QUAD_STRIP);
    }
    else drawArrays(verts,GL_QUAD_STRIP);
}

void setView(float left, float bottom, float right, float top)
{
    float w = right-left;
    float h = top - bottom;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(2.0f,2.0f,1);
    glTranslatef(-.5f,-0.5,0);

    glScalef(1/w,1/h,1);
    glTranslatef(-left,-bottom,0.0f);
}

void rotateView(float angle)
{
    glRotatef(angle,0,0,1);
}

void moveView(float x,float y)
{
    glTranslatef(-x,-y,0);
}

void resetView()
{
    setView(0,0,getWindowWidth(),getWindowHeight());
}

float getDeltaTime()
{
    return deltaTime;
}

void setVsync(bool state)
{
    errorAssert(isInit(),"kjpsgl not initialized!");
    if (SDL_GL_SetSwapInterval(state) != 0) fatalError("Can't setVsync()!");
}

void drawArrays(const vector<Vec2>& vertices,unsigned mode)
{
    vector<Color> c(vertices.size(), color);
    drawArrays(vertices,c, mode);
}

void drawArrays(const vector<Vec2>& vertices,const vector<Color>& colors, unsigned mode)
{
    if (colors.size()<vertices.size()) fatalError("Can't draw triangles! Not enought vertex colors!");
    // activate and specify pointer to vertex array
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, vertices.data());
    glColorPointer(4,GL_UNSIGNED_BYTE,0,colors.data());
    glDrawArrays(mode,0,vertices.size());

    // deactivate vertex arrays after drawing
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void drawArrays(const vector<Vec2>& vertices,const vector<Vec2>& uv,const vector<Color>& colors, unsigned mode)
{
    if (colors.size()<vertices.size()) fatalError("Can't draw triangles! Not enought vertex colors!");
    if (uv.size()<vertices.size()) fatalError("Can't draw triangles! Not enought uv cords!");
    // activate and specify pointer to vertex array
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, vertices.data());
    glColorPointer(4,GL_UNSIGNED_BYTE,0,colors.data());
    glTexCoordPointer(2,GL_FLOAT,0, uv.data());

    glDrawArrays(mode,0,vertices.size());

    // deactivate vertex arrays after drawing
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void drawArrays(const vector<Vec2>& vertices,const vector<Vec2>& uv, unsigned mode)
{
    if (uv.size()<vertices.size()) fatalError("Can't draw triangles! Not enought uv cords!");
    // activate and specify pointer to vertex array
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, vertices.data());
    glTexCoordPointer(2,GL_FLOAT,0, uv.data());

    glDrawArrays(mode,0,vertices.size());

    // deactivate vertex arrays after drawing
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void drawCircle(float x, float y, float r, int segments)
{
    vector<Vec2> verts;
    verts.reserve(segments);
    for (int i=0; i<segments; ++i)
    {
        float a = (i/float(segments)) *(2.0f*3.141592653589);
        verts.emplace_back(x+sin(a)*r,y+cos(a)*r);
    }
    if (boundTexture>=0)
    {
        vector<Vec2> uvs = verts;
        const Vec2 s (getTextureWidth(boundTexture),getTextureHeight(boundTexture));
        for (unsigned i=0; i<uvs.size(); ++i) uvs[i].x/=s.x, uvs[i].y/=s.y;
        drawArrays(verts,uvs,vector<Color>(verts.size(),color),GL_TRIANGLE_FAN);
    }
    else drawArrays(verts,GL_TRIANGLE_FAN);
}

void drawTriangleOutline(float ax, float ay, float bx, float by, float cx, float cy,float w)
{
    vector<Vec2> v {Vec2(ax,ay),Vec2(bx,by), Vec2(cx,cy),Vec2(ax,ay)};
    drawLine(v,w);
}

void drawRectangleOutline(float left, float bottom, float right, float top,float w)
{
    vector<Vec2> v {Vec2(left,top),Vec2(left,bottom),Vec2(right,bottom), Vec2(right,top),Vec2(left,top)};
    drawLine(v,w);
}

void drawCircleOutline(float x, float y, float r,float w, int segments)
{
    vector<Vec2> verts;
    verts.reserve(segments+1);
    for (int i=0; i<segments; ++i)
    {
        float a = (i/float(segments)) *(2.0f*3.141592653589);
        verts.emplace_back(x+sin(a)*r,y+cos(a)*r);
    }
    verts.push_back(verts.front());
    drawLine(verts,w);
}

int loadTexture(const string& filename)
{
    errorAssert(isInit(),"kjpsgl not initialized!");
    vector<unsigned char> image;
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, filename);
    if(error != 0) fatalError(string("Can't decode image! reason: ")+lodepng_error_text(error));
    GLuint tex;
    glGenTextures(1, &tex);
    if (tex==0) fatalError("Can't create texture!");
    glBindTexture(GL_TEXTURE_2D, tex);
    int mode = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, GL_UNSIGNED_BYTE, image.data());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    textures.push_back(tex);
    textureSizes.push_back(Vec2(width,height));
    return textures.size()-1;
}

void destroyTexture(int id)
{
    errorAssert(isInit(),"kjpsgl not initialized!");
    if (id<0 || id>=int(textures.size())) fatalError("Texture ID don't exist!");
    GLuint tex = textures[id];
    if (tex==0) fatalError("Can't destroy Texture twice!");
    glDeleteTextures(1, &tex);
    textures[id] = 0;
}

int getTextureHeight(int id)
{
    if (id<0 || id>=int(textures.size())) fatalError("Texture ID don't exist!");
    return textureSizes[id].y;
}

int getTextureWidth(int id)
{
    if (id<0 || id>=int(textures.size())) fatalError("Texture ID don't exist!");
    return textureSizes[id].x;
}

void setTexture(int id)
{
    errorAssert(isInit(),"kjpsgl not initialized!");
    if (id<0)
    {
        if (boundTexture>=0)
        {
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
            boundTexture = -1;
        }
    }
    else
    {
        if (id<0 || id>=int(textures.size())) fatalError("Texture ID don't exist!");
        if (boundTexture != id)
        {
            unsigned tex = textures[id];
            if (tex==0) fatalError("Can't set texture which is destroyed!");
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tex);
            boundTexture = id;
        }
    }
}

bool getKey(const string& name)
{
    SDL_Scancode s = SDL_GetScancodeFromName(name.c_str());
    if (s==SDL_SCANCODE_UNKNOWN)
    {
        cerr<<"WARN: key '"<< name << "' unknown!\n";
        return 0;
    }
    assert(s<keys.size());
    return keys[s];
}

bool getAnyKey()
{
    return anyKey;
}

bool getMouseButton(int button)
{
    if (button<0 || button>=int(mouseButtons.size())) fatalError("Wrong mouse button!");
    return mouseButtons[button];
}

float getMouseX()
{
    return mousePos.x;
}
float getMouseY()
{
    return getWindowHeight()-mousePos.y;
}
int getMouseWheel()
{
    return mouseWheel;
}

int getWindowHeight()
{
    errorAssert(isInit(),"kjpsgl not initialized!");
    int w,h;
    SDL_GetWindowSize(window,&w,&h);
    return h;
}

int getWindowWidth()
{
    errorAssert(isInit(),"kjpsgl not initialized!");
    int w,h;
    SDL_GetWindowSize(window,&w,&h);
    return w;
}

long long random()
{
    errorAssert(isInit(),"kjpsgl not initialized!");
    randomSeed ^= randomSeed >> 12; // a
    randomSeed ^= randomSeed << 25; // b
    randomSeed ^= randomSeed >> 27; // c
    return randomSeed * 2685821657736338717LL;
}

// in range [a;b]
int randomInRange(int a, int b)
{
    errorAssert(a <= b,"wrong interval (a>b0");
    uint64_t r = random();
    r %= b - a + 1;
    return a + int(r);
}

void sleep(unsigned ms)
{
    SDL_Delay(ms);
}

unsigned getTime()
{
    return SDL_GetTicks();
}

bool isInit(){return inited;}
}
