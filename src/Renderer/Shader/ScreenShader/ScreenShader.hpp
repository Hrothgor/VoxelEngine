#pragma once

#ifndef SCREENSHADER_HPP_
#define SCREENSHADER_HPP_

#include "Include.hpp"
#include "Renderer/Shader/ShaderProgram.hpp"

class ScreenShader : public ShaderProgram {
    public:
        ScreenShader();
        ~ScreenShader();

        void LoadResolution(const glm::vec2 &resolution);
        void LoadTextureAlbedo(int texture);
        void LoadTextureNormal(int texture);
        void LoadTexturePosition(int texture);
        void LoadTextureDepth(int texture);
 
    protected:
        void GetAllUniformLocation();

    private:
        int loc_Resolution;
        int loc_textureAlbedo;
        int loc_textureNormal;
        int loc_texturePosition;
        int loc_textureDepth;
};

#endif /* !SCREENSHADER_HPP_ */
