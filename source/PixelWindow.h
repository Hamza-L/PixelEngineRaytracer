#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN //includes vulkan automatically
#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>

class PixelWindow
{
public:
	PixelWindow();
	PixelWindow(const PixelWindow&) = delete;
	PixelWindow& operator=(const PixelWindow&) = delete;
	~PixelWindow();

	void initWindow(std::string wName = "Default Window", int width = 800, int height = 600);
	bool shouldClose();
	GLFWwindow* getWindow();

private:
	GLFWwindow* window;
	std::string windowName;
	int windowWidth;
	int windowHeight;
};

