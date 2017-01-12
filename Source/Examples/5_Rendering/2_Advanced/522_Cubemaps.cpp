/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#include <Dreemchest.h>
#include "../Examples.h"

DC_USE_DREEMCHEST

using namespace Platform;
using namespace Renderer;

// Declare a skybox unit cube
f32 s_cubeVertices[] =
{
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

// Now let's define a vertex ubershader with F_Transform, F_TexCoord and
// F_NormalAsColor feature options.
static String s_vertexShader =
    "cbuffer Projection projection : 0;                         \n"

    "varying vec3 v_texCoord;                                   \n"

    "void main()                                                \n"
    "{                                                          \n"
    "   gl_Position = projection.transform * gl_Vertex;         \n"
    "   v_texCoord  = gl_Vertex.xyz;                            \n"
    "}                                                          \n"
    ;

// And the same for a fragment shader
static String s_fragmentShader =
    "uniform samplerCube Texture0;                              \n"

    "varying vec3 v_texCoord;                                   \n"

    "void main()                                                \n"
    "{                                                          \n"
    "   gl_FragColor = textureCube(Texture0, v_texCoord);       \n"
    "}                                                          \n"
    ;

class Cubemaps : public RenderingApplicationDelegate
{
    StateBlock8 m_renderStates;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        // Create a cube vertex buffer and bind it to a render state block
        {
            InputLayout   inputLayout  = m_renderingContext->requestInputLayout(VertexFormat::Position);
            VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(s_cubeVertices, sizeof(s_cubeVertices));
            
            m_renderStates.bindVertexBuffer(vertexBuffer);
            m_renderStates.bindInputLayout(inputLayout);
        }
        
        // Create a projection constant buffer
        {
            Examples::Projection projection     = Examples::Projection::perspective(120.0f, m_window->width(), m_window->height(), 0.01f, 100.0f);
            UniformLayout        layout         = m_renderingContext->requestUniformLayout("Projection", Examples::Projection::Layout);
            ConstantBuffer_      constantBuffer = m_renderingContext->requestConstantBuffer(&projection, sizeof(projection), layout);
            
            // Bind this constant buffer to the slot #0
            m_renderStates.bindConstantBuffer(constantBuffer, 0);
        }
        
        // Load a cubemap texture and bind it to a sampler #0
        {
            const String envName = "MonValley_DirtRoad";
            const String files[] =
            {
                  "Assets/Textures/Environments/" + envName + "/right.tga"
                , "Assets/Textures/Environments/" + envName + "/left.tga"
                , "Assets/Textures/Environments/" + envName + "/top.tga"
                , "Assets/Textures/Environments/" + envName + "/bottom.tga"
                , "Assets/Textures/Environments/" + envName + "/front.tga"
                , "Assets/Textures/Environments/" + envName + "/back.tga"
            };
            
            Examples::Surface pixels;
            Examples::Image   faces[6];
            
            for (s32 i = 0; i < 6; i++)
            {
                faces[i] = Examples::tgaFromFile(files[i]);
                pixels.insert(pixels.end(), faces[i].pixels.begin(), faces[i].pixels.end());
            }
            
            Texture_ env = m_renderingContext->requestTextureCube(&pixels[0], faces[0].width, 1, faces[0].format);
            m_renderStates.bindTexture(env, 0);
        }
        
        // Create a program that consists from a vertex and fragment shaders.
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
    }
 
    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        RenderFrame frame(m_renderingContext->defaultStateBlock());
        
        RenderCommandBuffer& commands = frame.entryPoint();
        
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        commands.drawPrimitives(0, PrimTriangles, 0, 36, m_renderStates);

        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new Cubemaps)
