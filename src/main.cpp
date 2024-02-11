#include "Include.hpp"

#include <chrono>

#include "InputManager.hpp"
#include "SimpleRenderer.hpp"
#include "Camera.hpp"
#include "Engine.hpp"

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
        Engine::Get()->Update(window);

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

        std::cout << "FPS: " << Engine::Get()->GetFPS() << "   \t" << "Delta Time: " << Engine::Get()->GetDeltaTime() * 1000.0 << "ms" << std::endl;
    }

    glfwTerminate();
    return 0;
}


// {
//             float rayOrigin[3] = {0.5, 1.5, -0.1};
//             float rayDirection[3] = {0.85, 0.1, 1};
//             //t1 = -rayOrigin / rayDirection
//             float t1[3] = {-rayOrigin[0] / rayDirection[0], -rayOrigin[1] / rayDirection[1], -rayOrigin[2] / rayDirection[2]};
//             //t2 = (2 - rayOrigin) / rayDirection
//             float t2[3] = {(2.0 - rayOrigin[0]) / rayDirection[0], (2.0 - rayOrigin[1]) / rayDirection[1], (2.0 - rayOrigin[2]) / rayDirection[2]};

//             // float tMin[3] = {0.8, 0.6, 0.0};
//             // float tMax[3] = {2.0, 0.2, 0.5};
//             float tMedian[3] = {(t1[0] + t2[0]) / 2.0, (t1[1] + t2[1]) / 2.0, (t1[2] + t2[2]) / 2.0};

//             std::cout << "t1: " << t1[0] << ", " << t1[1] << ", " << t1[2] << std::endl;
//             std::cout << "t2: " << t2[0] << ", " << t2[1] << ", " << t2[2] << std::endl;
//             std::cout << "tMedian: " << tMedian[0] << ", " << tMedian[1] << ", " << tMedian[2] << std::endl;

//             float tx0 = t1[0];
//             float ty0 = t1[1];
//             float tz0 = t1[2];
//             float txm = tMedian[0];
//             float tym = tMedian[1];
//             float tzm = tMedian[2];

//             int answer = 0;
//             // select the entry plane and set bits
//             if (tx0 > ty0)
//             {
//                 if (tx0 > tz0)
//                 { // PLANE YZ
//                     if (tym < tx0) answer |= 2; // set bit at position 1
//                     if (tzm < tx0) answer |= 4; // set bit at position 2
//                     std::cout << "YZ: " << answer << std::endl;
//                 }
//             }
//             else
//             {
//                 if (ty0 > tz0)
//                 { // PLANE XZ
//                     if (txm < ty0) answer |= 1; // set bit at position 0
//                     if (tzm < ty0) answer |= 4; // set bit at position 2
//                     std::cout << "XZ: " << answer << std::endl;
//                 }
//             }
//             // PLANE XY
//             if (txm < tz0) answer |= 1; // set bit at position 0
//             if (tym < tz0) answer |= 2; // set bit at position 1
//             std::cout << "XY: " << answer << std::endl;
//         }
//         return 0;