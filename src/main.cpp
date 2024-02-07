#include "Include.hpp"

#include <chrono>

#include "InputManager.hpp"
#include "SimpleRenderer.hpp"
#include "Camera.hpp"

void ProcessInput(GLFWwindow* window)
{
    InputManager::Get()->ProcessInput(window);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
    GLFWwindow* window = glfwCreateWindow(800, 600, "Voxel Engine", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create the GLFW window\n";
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
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    SimpleRenderer renderer;
    Camera camera;

	// render loop
	// -----------
    while (!glfwWindowShouldClose(window))
    {
        auto start = std::chrono::high_resolution_clock::now();
   
        // input
		// -----
        ProcessInput(window);

        camera.Update(window);

		// render
		// ------
        renderer.StartFrame();
        renderer.DrawFullScreenTriangle(camera);
        renderer.EndFrame();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        auto finish = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(finish - start).count() << "ms" << std::endl;
    }

    glfwTerminate();
    return 0;
}