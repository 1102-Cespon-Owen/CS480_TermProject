#include "engine.h"

Engine* Engine::instance = nullptr;

Engine::Engine(const char* name, int width, int height)
{
    m_WINDOW_NAME = name;
    m_WINDOW_WIDTH = width;
    m_WINDOW_HEIGHT = height;
    instance = this;

}




Engine::~Engine()
{
    delete m_window;
    delete m_graphics;
    m_window = NULL;
    m_graphics = NULL;
}


bool Engine::Initialize()
{
    // Start a window
    m_window = new Window(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT);
    if (!m_window->Initialize())
    {
        printf("The window failed to initialize.\n");
        return false;
    }


    // Start the graphics
    m_graphics = new Graphics();
    if (!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT))
    {
        printf("The graphics failed to initialize.\n");
        return false;
    }


    //Setup for camera mouse movement
    lastX = (float)m_WINDOW_WIDTH / 2;
    lastY = (float)m_WINDOW_HEIGHT / 2;
    firstMouse = true;

    glfwSetCursorPosCallback(m_window->getWindow(), cursorPositionCallBack);
    glfwSetScrollCallback(m_window->getWindow(), scrollCallback);
    glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);




    // No errors
    return true;
}


void Engine::Run()
{
    m_running = true;

    lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_window->getWindow()))
    {
        // Update camera based on current mode
        m_graphics->getUser()->UpdateDT();
        m_graphics->getUser()->updateModel();

        // NEW
        float currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
		//


        ProcessInput();
        Display(m_window->getWindow(), glfwGetTime());
        glfwPollEvents();
    }
    m_running = false;


}


void Engine::ProcessInput()
{

    GLFWwindow* window = m_window->getWindow();
    User* user = m_graphics->getUser(); // user is accessed via Graphics
    float dt = getDeltaTime();

    // Exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Mode toggle (C)
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        user->ToggleCameraMode();

    // Forward WASD, T keys to Scene
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		user->Accelerate();
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		user->Brake();
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		user->YawLeft();
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		user->YawRight();
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		user->RollRight();
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		user->RollLeft();
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		user->Stop();


    /*
    // Move camera based on WASD input
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
    {
        m_graphics->getCamera()->MoveForward();
    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
    {
        m_graphics->getCamera()->MoveBackward();

    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
    {
        m_graphics->getCamera()->MoveLeft();

    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
    {
        m_graphics->getCamera()->MoveRight();

    }
    */

}


unsigned int Engine::getDT()
{
    //long long TimeNowMillis = GetCurrentTimeMillis();
    //assert(TimeNowMillis >= m_currentTimeMillis);
    //unsigned int DeltaTimeMillis = (unsigned int)(TimeNowMillis - m_currentTimeMillis);
    //m_currentTimeMillis = TimeNowMillis;
    //return DeltaTimeMillis;
    return glfwGetTime();
}


long long Engine::GetCurrentTimeMillis()
{
    //timeval t;
    //gettimeofday(&t, NULL);
    //long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    //return ret;
    return (long long)glfwGetTime();
}


void Engine::Display(GLFWwindow* window, double time) {

    m_graphics->getUser()->updateModel();
    m_graphics->Render();
    m_window->Swap();

    m_graphics->HierarchicalUpdate2(time);
}


void Engine::cursorPositionCallBack(GLFWwindow* window, double xpos, double ypos) {
    if (instance->firstMouse) {
        instance->lastX = xpos;
        instance->lastY = ypos;
        instance->firstMouse = false;
    }

    float xoffset = xpos - instance->lastX;;
    float yoffset = instance->lastY - ypos;
    instance->lastX = xpos;
    instance->lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    //instance->m_graphics->getCamera()->ProcessMouseMovement(xoffset, yoffset);
    instance->m_graphics->getUser()->ProcessMouseMovement(xoffset, yoffset);

}


void Engine::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (Engine::instance && Engine::instance->m_graphics) {
        Engine::instance->m_graphics->getUser()->ProcessMouseScroll((float)yoffset);
    }
}
