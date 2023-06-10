cmake_minimum_required(VERSION 3.17)

project(PixelEngine)
set(PROJECT_NAME PixelEngine)
set(CMAKE_CXX_STANDARD 17)


set(ROOT_NAMESPACE PixelEngine)
set(CMAKE_VS_PLATFORM_NAME "x64")




################################################################################zZ
# Source groups
################################################################################
set(Headers
    "source/PixelRenderer.h"
    "source/PixelWindow.h"
    "source/PixelGraphicsPipeline.h"
    "source/PixelObject.h"
    "source/PixelScene.h"
    "source/PixelImage.h"
    "source/Utility.h" 
    "source/stb_image.h"
    "source/PixelComputePipeline.h"
    "source/kb_input.h")
source_group("Headers" FILES ${Headers})

set(Sources
    "source/main.cpp"
    "source/PixelRenderer.cpp"
    "source/PixelWindow.cpp"
    "source/PixelGraphicsPipeline.cpp"
    "source/PixelObject.cpp"
    "source/PixelScene.cpp"
    "source/PixelImage.cpp"
    "source/PixelComputePipeline.cpp"
    "source/kb_input.cpp")

source_group("Sources" FILES ${Sources})

set(imguiSources
        "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/imgui/imgui.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/imgui/imgui.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/imgui/imgui_demo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/imgui/imgui_draw.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/imgui/imgui_widgets.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/imgui/imgui_tables.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/imgui/backends/imgui_impl_vulkan.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/imgui/backends/imgui_impl_glfw.cpp"
        )

source_group("imguiSources" FILES ${imguiSources})

set(ALL_FILES
    ${Headers}
    ${Sources}
    ${imguiSources})

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

################################################################################
# Include directories and Dependencies
################################################################################

if (WIN32)
    message("Starting the Windows Build")
    set(VULKAN_SDK $ENV{VULKAN_SDK})

    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_include_directories(${PROJECT_NAME} PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/GLFW64/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/GLM;"
            "${VULKAN_SDK}/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/assimp/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/imgui"
        )

        target_link_directories(${PROJECT_NAME} PRIVATE
                "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/GLFW64/lib-vc2022"
                "C:/VulkanSDK/1.3.246.1/Lib"
                "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/assimp/lib/Release"
                )

    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_include_directories(${PROJECT_NAME} PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/GLFW/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/GLM;"
            "${VULKAN_SDK}/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/assimp/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/imgui"
        )

        target_link_directories(${PROJECT_NAME} PRIVATE
                "${CMAKE_CURRENT_SOURCE_DIR}/external/windows/GLFW/lib-vc2022"
                "C:/VulkanSDK/1.3.246.1/Lib32"
                )
    endif()



    set(ADDITIONAL_LIBRARY_DEPENDENCIES
            "vulkan-1.lib"
            "glfw3.lib"
            "assimp-vc143-mt.lib"
            )
elseif(UNIX AND APPLE) # If the build is mac
    message("Starting the MacOS Build")
    set(VULKAN_SDK "/Users/hamzalah/VulkanSDK/1.3.239.0/macOS")

    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/external/macos/GLFW64/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/external/macos/GLM;"
        "${VULKAN_SDK}/include"
    )

    target_link_directories(${PROJECT_NAME} PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/external/macos/GLFW64/lib-x86_64"
            "${VULKAN_SDK}/lib"
            )

    set(ADDITIONAL_LIBRARY_DEPENDENCIES
            "libvulkan.1.3.239.dylib"
            "libglfw.3.dylib"
            )
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/external/windows/assimp/dll/assimp-vc143-mt.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cmake-build-debug)



