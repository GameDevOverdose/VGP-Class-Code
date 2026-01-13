//====================================================================================================
// Filename:	Gui.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "Gui.h"

#include "GraphicsSystem.h"
#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_dx11.h>
#include <ImGui/backends/imgui_impl_glfw.h>

using namespace X;

namespace
{
	GLFWwindow* sWindow = nullptr;
}

void Gui::Initialize(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

	auto device = GraphicsSystem::Get()->GetDevice();
	auto context = GraphicsSystem::Get()->GetContext();

	// Initialize GLFW backend for ImGui
	ImGui_ImplGlfw_InitForOther(window, true);
	ImGui_ImplDX11_Init(device, context);

	sWindow = window;
}

void Gui::Terminate()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	sWindow = nullptr;
}

void Gui::BeginRender()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Gui::EndRender()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}
