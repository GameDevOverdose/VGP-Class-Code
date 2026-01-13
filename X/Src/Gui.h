//====================================================================================================
// Filename:	Gui.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_GUI_H
#define INCLUDED_XENGINE_GUI_H

#include <GLFW/glfw3.h>

namespace X {
namespace Gui {

void Initialize(GLFWwindow* window);
void Terminate();
void BeginRender();
void EndRender();

} // namespace Gui
} // namespace X

#endif // #ifndef INCLUDED_XENGINE_GUI_H
