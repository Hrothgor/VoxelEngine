#include <iostream>
#include <algorithm>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <FastNoise/FastNoise.h>
#include "SVO.h"

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    auto fnSimplex = FastNoise::New<FastNoise::Value>();

	// render loop
	// -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
		// -----
        ProcessInput(window);

		// render
		// ------
        glClearColor(1.00f, 0.49f, 0.04f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);


        auto start = std::chrono::high_resolution_clock::now();

        // SVO svo(6);

        // std::vector<float> noiseOutput(64 * 64);
        // fnSimplex->GenUniformGrid2D(noiseOutput.data(), 0, 0, 256, 256, 0.2, 1337);

        // for (auto& value : noiseOutput)
        // {
        //     std::cout << value << std::endl;
		// }


        auto finish = std::chrono::high_resolution_clock::now();

        std::cout << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(finish - start).count() << "ms" << std::endl;


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}