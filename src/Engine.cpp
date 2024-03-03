#include "Engine.hpp"

#include "Logger.hpp"
#include "Profiler.hpp"
#include "InputManager.hpp"
#include "Entities/Camera.hpp"
#include "Renderer/Renderer.hpp"

Engine *Engine::instance = nullptr;

Engine::Engine() {
    _Window = nullptr;
    _Width = 1400;
    _Height = 800;
    _Title = "Voxel Engine";
    _VSync = false;

    _MainCamera = std::make_shared<Camera>();
}

Engine::~Engine() {
}

void Engine::Start() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    _Window = glfwCreateWindow(_Width, _Height, _Title.c_str(), nullptr, nullptr);
    if (!_Window)
    {
        Logger::Get()->Log(Logger::LogType::ERROR, "Failed to create the GLFW window");
        Stop();
        return;
    }
    glfwMakeContextCurrent(_Window);
    glfwSwapInterval(_VSync);
    glfwSetFramebufferSizeCallback(_Window, [](GLFWwindow* window, int width, int height)
    {
        Engine::Get()->SetWidth(width);
        Engine::Get()->SetHeight(height);
    });
    InputManager::Get()->Bind(GLFW_KEY_ESCAPE, GLFW_PRESS, InputManager::InputType::KEY, [](GLFWwindow* window)
    {
        glfwSetWindowShouldClose(window, true);
    });

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Logger::Get()->Log(Logger::LogType::ERROR, "Failed to initialize GLAD");
        Stop();
        return;
    }

    Logger::Get()->Log(Logger::LogType::SUCCESS, "Window created successfully");
    Logger::Get()->Log(Logger::LogType::INFO, "OpenGL version: %s", glGetString(GL_VERSION));

    Renderer::Get()->Start(_Window);
}

void Engine::Update() {
    // render loop
    // -----------
    while (!glfwWindowShouldClose(_Window))
    {
        Profiler::Get()->StartFrame("Engine");
        Profiler::Get()->StartFrame("CPU Frame");
        {
            // input
            // -----
            InputManager::Get()->ProcessInput(_Window);

            // Update
            // ------
            UpdateAverageFPS();
            _MainCamera->Update(_Window);
        }
        Profiler::Get()->EndFrame();

        // render
        // ------
        Renderer::Get()->Render();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(_Window);
        glfwPollEvents();
        Profiler::Get()->EndFrame();
    }
}

void Engine::Stop() {
    Renderer::Get()->Stop();
    glfwTerminate();
}

float Engine::GetDeltaTime() {
    return Profiler::Get()->GetFrameTime("Engine");
}

int Engine::GetFPS() {
    return _averageFPS;
}

void Engine::UpdateAverageFPS() {
    _fpsTimes.push_back(1 / GetDeltaTime());
    if (_fpsTimes.size() > 60)
        _fpsTimes.erase(_fpsTimes.begin());

    _averageFPS = std::accumulate(_fpsTimes.begin(), _fpsTimes.end(), 0.0) / _fpsTimes.size();
}