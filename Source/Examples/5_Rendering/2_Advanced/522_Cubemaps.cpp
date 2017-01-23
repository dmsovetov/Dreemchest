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
#include <Framework.h>

DC_USE_DREEMCHEST

using namespace Platform;
using namespace Renderer;

static String s_vertexShader =
    "cbuffer Projection projection : 0;                         \n"
    "cbuffer Camera     camera     : 1;                         \n"

    "varying vec3 v_texCoord;                                   \n"

    "void main()                                                \n"
    "{                                                          \n"
    "   gl_Position = projection.transform                      \n"
    "               * camera.rotation                           \n"
    "               * gl_Vertex;                                \n"
    "   v_texCoord  = gl_Vertex.xyz;                            \n"
    "}                                                          \n"
    ;

static String s_fragmentShader =
    "uniform samplerCube Texture0;                              \n"

    "varying vec3 v_texCoord;                                   \n"

    "void main()                                                \n"
    "{                                                          \n"
    "   gl_FragColor = textureCube(Texture0, v_texCoord);       \n"
    "}                                                          \n"
    ;

// Application delegate is now subclassed from a Examples::ViewerApplicationDelegate to add camera and arcball functionality.
class Cubemaps : public Framework::ApplicationDelegate
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
            VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(Framework::UnitCube, sizeof(Framework::UnitCube));
            
            m_renderStates.bindVertexBuffer(vertexBuffer);
            m_renderStates.bindInputLayout(inputLayout);
        }
        
        // Load a cubemap texture and bind it to a sampler #0
        {
            const String envName = "Sierra_Madre_B";
            const String files[] =
            {
                  "Assets/Textures/Environments/" + envName + "/posx.tga"
                , "Assets/Textures/Environments/" + envName + "/negx.tga"
                , "Assets/Textures/Environments/" + envName + "/posy.tga"
                , "Assets/Textures/Environments/" + envName + "/negy.tga"
                , "Assets/Textures/Environments/" + envName + "/posz.tga"
                , "Assets/Textures/Environments/" + envName + "/negz.tga"
            };
            
            Framework::Surface pixels;
            Framework::Image   faces[6];
            
            for (s32 i = 0; i < 6; i++)
            {
                faces[i] = Framework::tgaFromFile(files[i]);
                pixels.insert(pixels.end(), faces[i].pixels.begin(), faces[i].pixels.end());
            }
            
            Texture_ env = m_renderingContext->requestTextureCube(&pixels[0], faces[0].width, 1, faces[0].format);
            m_renderStates.bindTexture(env, 0);
        }
        
        // Create a program that consists from a vertex and fragment shaders.
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
    }
    
    // This method is declared inside the Examples::ViewerApplicationDelegate.
    virtual void handleRenderFrame(RenderFrame& frame, StateStack& stateStack, RenderCommandBuffer& commands, f32 dt) NIMBLE_OVERRIDE
    {
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        commands.drawPrimitives(0, PrimTriangles, 0, 36, m_renderStates);
    }
};

dcDeclareApplication(new Cubemaps)
