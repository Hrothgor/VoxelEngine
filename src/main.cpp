#include "Include.hpp"
#include "Engine.hpp"
#include "Logger.hpp"

#include "Loader.hpp"

int main()
{
    srand(time(NULL));

    try {
        Engine::Get()->Start();
        Engine::Get()->Update();
    }
    catch (...)
	{
        Logger::Get()->Log(Logger::LogType::ERROR, "CRASHED!");
        Logger::Get()->PrintLogToFile("log.txt");
		Engine::Get()->Stop();
		return 84;
	}
    Logger::Get()->PrintLogToFile("log.txt");
	Engine::Get()->Stop();

    return 0;
}