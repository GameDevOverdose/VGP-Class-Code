#include "Precompiled.h"
#include "Window.h"

using namespace X;

void Window::Initialize(const char* appName, uint32_t width, uint32_t height)
{
    mAppName = appName;
    mWidth = width;
    mHeight = height;

    // Initialize GLFW
    if (!glfwInit())
    {
        return;
    }

    // GLFW window hints for DirectX (no OpenGL context)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create window
    mWindow = glfwCreateWindow(width, height, appName, nullptr, nullptr);

    if (mWindow == nullptr)
    {
        glfwTerminate();
        return;
    }

    mIsActive = true;
}

void Window::Terminate()
{
    if (mWindow != nullptr)
    {
        glfwDestroyWindow(mWindow);
        mWindow = nullptr;
    }
    glfwTerminate();
    mIsActive = false;
}

void Window::ProcessMessage()
{
    glfwPollEvents();

    // Check if window should close
    if (glfwWindowShouldClose(mWindow))
    {
        mIsActive = false;
    }
}

GLFWwindow* Window::GetWindowHandle() const
{
    return mWindow;
}

bool Window::IsActive() const
{
    return mIsActive;
}
