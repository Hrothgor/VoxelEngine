#pragma once

#ifndef INPUTMANAGER_HPP_
#define INPUTMANAGER_HPP_

#include "Include.hpp"

class InputManager {
    public:

        enum InputType {
            KEY,
            MOUSE
        };

        struct KeyAction {
            InputType type;
            int key;
            int action;

            bool operator<(const KeyAction &other) const {
                return std::tie(type, key, action) < std::tie(other.type, other.key, other.action);
            }
        };

        InputManager();
        ~InputManager();

        void ProcessInput(GLFWwindow *window);
        void Bind(int key, int action, InputType type,std::function<void(GLFWwindow *)> callback);

    protected:
    private:
        std::map<KeyAction, std::function<void(GLFWwindow *)>> _keysCallback;

// SINGLETON
    protected:
        static InputManager *instance;
    public:
        InputManager(InputManager &other) = delete;
        void operator=(const InputManager &) = delete;
        static InputManager *Get() {
            if (instance == nullptr)
                instance = new InputManager();
            return instance;
        };
};

#endif /* !INPUTMANAGER_HPP_ */