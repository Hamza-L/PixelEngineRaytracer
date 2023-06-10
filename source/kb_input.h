//
// Created by hlahm on 2021-11-07.
//

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//imgui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

static const bool TEXTURE = true;
static bool UP_PRESS = false;
static bool DOWN = false;
static bool RIGHT = false;
static bool LEFT = false;
static bool SHIFT = false;
static bool COM = false;
static float scroll = 0;
static bool ENTER = false;
static bool ENTER_FLAG = true;
static bool ESC = false;
static bool MPRESS_R_Release = true;

//mouse events
static bool MPRESS_R = false;
static bool MFLAG_R = true;
static bool MPRESS_L = false;
static bool MFLAG_L = true;
static bool MPRESS_M = false;
static bool MFLAG_M = true;

//arcball parameter;
static float FIT = 1.5f;
static float GAIN = 2.0f;

void static key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_W && action == GLFW_PRESS){
        UP_PRESS = true;
    } else if(key == GLFW_KEY_W && action == GLFW_RELEASE){
        UP_PRESS = false;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        ESC = true;
    } else if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE){
        ESC = false;
    }

    /*
    if (key == GLFW_KEY_W && action == GLFW_RELEASE){
        UP_PRESS_FLAG = true;
        DOWN_FLAG = true;
    }
     */

    if (key == GLFW_KEY_S && action == GLFW_PRESS){
        DOWN = true;
    } else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
        DOWN = false;
    }/*
        if (key == GLFW_KEY_S && action == GLFW_RELEASE){
            UP_PRESS_FLAG = true;
            DOWN_FLAG = true;
        }*/

    if (key == GLFW_KEY_D && action == GLFW_PRESS){
        RIGHT = true;
    } else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
        RIGHT = false;
    }/*
        if (key == GLFW_KEY_D && action == GLFW_RELEASE){
            LEFT_FLAG = true;
            RIGHT_FLAG = true;
        }*/

    if (key == GLFW_KEY_A && action == GLFW_PRESS){
        LEFT = true;
    } else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        LEFT = false;
    }/*
        if (key == GLFW_KEY_A && action == GLFW_RELEASE){
            LEFT_FLAG = true;
            RIGHT_FLAG = true;
        }*/

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS){
        SHIFT = true;
    } else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
        SHIFT = false;
    }

    if (key == GLFW_KEY_LEFT_SUPER && action == GLFW_PRESS){
        COM = true;
    } else if (key == GLFW_KEY_LEFT_SUPER && action == GLFW_RELEASE) {
        COM = false;
    }

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS){
        ENTER = true;
    }
    if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE) {
        ENTER = false;
        ENTER_FLAG = true;
    }
}
void static mouse_callback(GLFWwindow *window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)){
        MPRESS_L = true;
    } else {
        MPRESS_L = false;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        MFLAG_L = true;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)){
        MPRESS_R = true;
    } else {
        MPRESS_R = false;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE){
        MFLAG_R = true;
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)){
        MPRESS_M = true;
    } else {
        MPRESS_M = false;
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE){
        MFLAG_M = true;
    }
}

void static scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    //std::cout<<yoffset<<std::endl;
    scroll += 2.0f*yoffset;
}