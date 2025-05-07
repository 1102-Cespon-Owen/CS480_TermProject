#ifndef ENGINE_H
#define ENGINE_H


#include <assert.h>
#include "window.h"
#include "graphics.h"

class Engine
{
public:
    Engine(const char* name, int width, int height);

    ~Engine();
    bool Initialize();
    void Run();
    void ProcessInput();
    unsigned int getDT();
    long long GetCurrentTimeMillis();
    void Display(GLFWwindow*, double);


private:
    // Window related variables
    Window* m_window;
    const char* m_WINDOW_NAME;
    int m_WINDOW_WIDTH;
    int m_WINDOW_HEIGHT;
    bool m_FULLSCREEN;


    Graphics* m_graphics;

    bool m_running;

    // Camera related variables
    float lastX;
    float lastY;
    bool firstMouse;

    static void cursorPositionCallBack(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static Engine* instance;  // Needed for static callback
};

#endif // ENGINE_H