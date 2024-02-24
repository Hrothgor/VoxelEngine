#include "Include.hpp"

#include <chrono>

#include "InputManager.hpp"
#include "SimpleRenderer.hpp"
#include "Camera.hpp"
#include "Engine.hpp"
#include "Logger.hpp"

void ProcessInput(GLFWwindow* window)
{
    InputManager::Get()->ProcessInput(window);
}

int main()
{
    try {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // glfw window creation
        // --------------------
        GLFWwindow* window = glfwCreateWindow(800, 600, "Voxel Engine", nullptr, nullptr);
        if (!window)
        {
            Logger::Get()->Log(Logger::LogType::ERROR, "Failed to create the GLFW window");
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
        });
        InputManager::Get()->Bind(GLFW_KEY_ESCAPE, GLFW_PRESS, InputManager::InputType::KEY, [](GLFWwindow* window)
        {
            glfwSetWindowShouldClose(window, true);
        });


        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            Logger::Get()->Log(Logger::LogType::ERROR, "Failed to initialize GLAD");
            return -1;
        }

        Logger::Get()->Log(Logger::LogType::SUCCESS, "Window created successfully");
        Logger::Get()->Log(Logger::LogType::INFO, "OpenGL version: %s", glGetString(GL_VERSION));

        SimpleRenderer::Get()->Init(window);
        Camera camera;

        // render loop
        // -----------
        while (!glfwWindowShouldClose(window))
        {
            Engine::Get()->Update(window);

            // input
            // -----
            ProcessInput(window);

            camera.Update(window);

            // render
            // ------
            SimpleRenderer::Get()->StartFrame();
            SimpleRenderer::Get()->DrawFullScreenTriangle(camera);
            SimpleRenderer::Get()->EndFrame();

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        Logger::Get()->PrintLogToFile("log.txt");

        glfwTerminate();
        return 0;
    } catch(...) {
        Logger::Get()->Log(Logger::LogType::ERROR, "CRASHHHHHHHH");
        Logger::Get()->PrintLogToFile("log.txt");
    }
}