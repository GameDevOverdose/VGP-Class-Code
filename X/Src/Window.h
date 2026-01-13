#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <cstdint>

namespace X
{

class Window
{
public:
    void Initialize(const char* appName, uint32_t width, uint32_t height);
    void Terminate();
    void ProcessMessage();

    GLFWwindow* GetWindowHandle() const;
    bool IsActive() const;

    uint32_t GetWidth() const { return mWidth; }
    uint32_t GetHeight() const { return mHeight; }

private:
    GLFWwindow* mWindow = nullptr;
    std::string mAppName;
    uint32_t mWidth = 0;
    uint32_t mHeight = 0;
    bool mIsActive = false;
};

} // namespace X
