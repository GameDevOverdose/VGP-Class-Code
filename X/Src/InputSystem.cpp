//====================================================================================================
// Filename:	InputSystem.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "InputSystem.h"

using namespace X;

namespace
{
	InputSystem* sInputSystem = nullptr;
}

// GLFW Callbacks
void InputSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void)window;
	(void)scancode;
	(void)mods;
	
	if (sInputSystem == nullptr || key < 0 || key >= 512)
		return;
	
	if (action == GLFW_PRESS)
		sInputSystem->mCurrKeys[key] = true;
	else if (action == GLFW_RELEASE)
		sInputSystem->mCurrKeys[key] = false;
}

void InputSystem::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	(void)window;
	(void)mods;
	
	if (sInputSystem == nullptr || button < 0 || button >= 3)
		return;
	
	if (action == GLFW_PRESS)
		sInputSystem->mCurrMouseButtons[button] = true;
	else if (action == GLFW_RELEASE)
		sInputSystem->mCurrMouseButtons[button] = false;
}

void InputSystem::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (sInputSystem == nullptr)
		return;
	
	sInputSystem->mCurrMouseX = static_cast<int>(xpos);
	sInputSystem->mCurrMouseY = static_cast<int>(ypos);
	
	if (sInputSystem->mPrevMouseX == -1)
	{
		sInputSystem->mPrevMouseX = sInputSystem->mCurrMouseX;
		sInputSystem->mPrevMouseY = sInputSystem->mCurrMouseY;
	}
	
	// Update edge detection
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	sInputSystem->mMouseLeftEdge = sInputSystem->mCurrMouseX <= 0;
	sInputSystem->mMouseRightEdge = sInputSystem->mCurrMouseX >= width - 1;
	sInputSystem->mMouseTopEdge = sInputSystem->mCurrMouseY <= 0;
	sInputSystem->mMouseBottomEdge = sInputSystem->mCurrMouseY >= height - 1;
}

void InputSystem::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	(void)window;
	(void)xoffset;
	
	if (sInputSystem == nullptr)
		return;
	
	sInputSystem->mMouseWheel += static_cast<float>(yoffset);
}

void InputSystem::FocusCallback(GLFWwindow* window, int focused)
{
	(void)window;
	
	if (sInputSystem == nullptr)
		return;
	
	if (!focused)
	{
		sInputSystem->mMouseLeftEdge = false;
		sInputSystem->mMouseRightEdge = false;
		sInputSystem->mMouseTopEdge = false;
		sInputSystem->mMouseBottomEdge = false;
	}
}

void InputSystem::StaticInitialize(GLFWwindow* window)
{
	XASSERT(sInputSystem == nullptr, "[InputSystem] System already initialized!");
	sInputSystem = new InputSystem();
	sInputSystem->Initialize(window);
}

void InputSystem::StaticTerminate()
{
	if (sInputSystem != nullptr)
	{
		sInputSystem->Terminate();
		SafeDelete(sInputSystem);
	}
}

InputSystem* InputSystem::Get()
{
	XASSERT(sInputSystem != nullptr, "[InputSystem] No system registered.");
	return sInputSystem;
}

InputSystem::InputSystem()
	: mWindow(nullptr)
	, mClipMouseToWindow(false)
	, mCurrMouseX(-1)
	, mCurrMouseY(-1)
	, mPrevMouseX(-1)
	, mPrevMouseY(-1)
	, mMouseMoveX(0)
	, mMouseMoveY(0)
	, mMouseWheel(0)
	, mMouseLeftEdge(false)
	, mMouseRightEdge(false)
	, mMouseTopEdge(false)
	, mMouseBottomEdge(false)
	, mInitialized(false)
{
	memset(mCurrKeys, 0, sizeof(mCurrKeys));
	memset(mPrevKeys, 0, sizeof(mPrevKeys));
	memset(mPressedKeys, 0, sizeof(mPressedKeys));
	memset(mCurrMouseButtons, 0, sizeof(mCurrMouseButtons));
	memset(mPrevMouseButtons, 0, sizeof(mPrevMouseButtons));
	memset(mPressedMouseButtons, 0, sizeof(mPressedMouseButtons));
}

//----------------------------------------------------------------------------------------------------

InputSystem::~InputSystem()
{
	XASSERT(!mInitialized, "[InputSystem] Terminate() must be called to clean up!");
}

//----------------------------------------------------------------------------------------------------

void InputSystem::Initialize(GLFWwindow* window)
{
	// Check if we have already initialized the system
	if (mInitialized)
	{
		XLOG("[InputSystem] System already initialized.");
		return;
	}

	XLOG("[InputSystem] Initializing...");
	
	mWindow = window;
	
	// Set GLFW callbacks
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetWindowFocusCallback(window, FocusCallback);

	// Set flag
	mInitialized = true;

	XLOG("[InputSystem] System initialized.");
}

//----------------------------------------------------------------------------------------------------

void InputSystem::Terminate()
{
	// Check if we have already terminated the system
	if (!mInitialized)
	{
		XLOG("[InputSystem] System already terminated.");
		return;
	}

	XLOG("[InputSystem] Terminating...");

	// Clear callbacks
	if (mWindow)
	{
		glfwSetKeyCallback(mWindow, nullptr);
		glfwSetMouseButtonCallback(mWindow, nullptr);
		glfwSetCursorPosCallback(mWindow, nullptr);
		glfwSetScrollCallback(mWindow, nullptr);
		glfwSetWindowFocusCallback(mWindow, nullptr);
	}
	mWindow = nullptr;

	// Set flag
	mInitialized = false;

	XLOG("[InputSystem] System terminated.");
}

//----------------------------------------------------------------------------------------------------

void InputSystem::Update()
{
	XASSERT(mInitialized, "[InputSystem] System not initialized.");

	// Store the previous keyboard state
	for (int i = 0; i < 512; ++i)
	{
		mPressedKeys[i] = !mPrevKeys[i] && mCurrKeys[i];
	}
	memcpy(mPrevKeys, mCurrKeys, sizeof(mCurrKeys));

	// Update mouse movement
	mMouseMoveX = mCurrMouseX - mPrevMouseX;
	mMouseMoveY = mCurrMouseY - mPrevMouseY;
	mPrevMouseX = mCurrMouseX;
	mPrevMouseY = mCurrMouseY;

	// Store the previous mouse state
	for (int i = 0; i < 3; ++i)
	{
		mPressedMouseButtons[i] = !mPrevMouseButtons[i] && mCurrMouseButtons[i];
	}
	memcpy(mPrevMouseButtons, mCurrMouseButtons, sizeof(mCurrMouseButtons));

	// Reset mouse wheel for next frame
	mMouseWheel = 0.0f;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsKeyDown(uint32_t key) const
{
	return key < 512 && mCurrKeys[key];
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsKeyPressed(uint32_t key) const
{
	return key < 512 && mPressedKeys[key];
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMouseDown(uint32_t button) const
{
	return button < 3 && mCurrMouseButtons[button];
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMousePressed(uint32_t button) const
{
	return button < 3 && mPressedMouseButtons[button];
}

//----------------------------------------------------------------------------------------------------

int InputSystem::GetMouseMoveX() const
{
	return mMouseMoveX;
}

//----------------------------------------------------------------------------------------------------

int InputSystem::GetMouseMoveY() const
{
	return mMouseMoveY;
}

//----------------------------------------------------------------------------------------------------

float InputSystem::GetMouseMoveZ() const
{
	return mMouseWheel;
}

//----------------------------------------------------------------------------------------------------

int InputSystem::GetMouseScreenX() const
{
	return mCurrMouseX;
}

//----------------------------------------------------------------------------------------------------

int InputSystem::GetMouseScreenY() const
{
	return mCurrMouseY;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMouseLeftEdge() const
{
	return mMouseLeftEdge;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMouseRightEdge() const
{
	return mMouseRightEdge;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMouseTopEdge() const
{
	return mMouseTopEdge;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMouseBottomEdge() const
{
	return mMouseBottomEdge;
}

//----------------------------------------------------------------------------------------------------

void InputSystem::ShowSystemCursor(bool show)
{
	if (mWindow)
	{
		glfwSetInputMode(mWindow, GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
	}
}

//----------------------------------------------------------------------------------------------------

void InputSystem::SetMouseClipToWindow(bool clip)
{
	mClipMouseToWindow = clip;
	if (mWindow)
	{
		glfwSetInputMode(mWindow, GLFW_CURSOR, clip ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsMouseClipToWindow() const
{
	return mClipMouseToWindow;
}

//----------------------------------------------------------------------------------------------------

// GamePad stubs - GLFW joystick support could be added later
bool InputSystem::IsGamePadConnected(int player) const
{
	return glfwJoystickPresent(player) == GLFW_TRUE;
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsAPressed(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsBPressed(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsXPressed(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsYPressed(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsLeftShoulderPressed(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsLeftTriggerPressed(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsRightShoulderPressed(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsRightTriggerPressed(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsDPadUp(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsDPadDown(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsDPadLeft(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsDPadRight(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsLeftThumbStickUp(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsLeftThumbStickDown(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsLeftThumbStickLeft(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsLeftThumbStickRight(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsRightThumbStickUp(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsRightThumbStickDown(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsRightThumbStickLeft(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

bool InputSystem::IsRightThumbStickRight(int player) const
{
	(void)player;
	return false; // Stub
}

//----------------------------------------------------------------------------------------------------

float InputSystem::GetLeftAnalogX(int player) const
{
	(void)player;
	return 0.0f; // Stub
}

//----------------------------------------------------------------------------------------------------

float InputSystem::GetLeftAnalogY(int player) const
{
	(void)player;
	return 0.0f; // Stub
}

//----------------------------------------------------------------------------------------------------

float InputSystem::GetRightAnalogX(int player) const
{
	(void)player;
	return 0.0f; // Stub
}

//----------------------------------------------------------------------------------------------------

float InputSystem::GetRightAnalogY(int player) const
{
	(void)player;
	return 0.0f; // Stub
}
