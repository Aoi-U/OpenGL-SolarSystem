#include "Game.hpp"
#include "Log.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();//MUST call this first to set up environment (There is a terminate pair after the loop)
    {// Encapsulation here is necessary to free memory and avoid segmentation fault.
		Game game {};
	    game.Run();
	}
	glfwTerminate(); // Clean up GLFW
	return 0;
}
