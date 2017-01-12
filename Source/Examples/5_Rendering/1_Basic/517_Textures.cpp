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

// Declare a vertex shader code that will output a received vertex.
static String s_vertexShader =
    "varying vec2 v_texCoord;                       \n"
    "                                               \n"
    "void main()                                    \n"
    "{                                              \n"
    "   v_texCoord  = gl_MultiTexCoord0.xy;         \n"
    "   gl_Position = gl_Vertex;                    \n"
    "}                                              \n"
;

// Declare a fragment shader that will output fragments with a constant color.
static String s_fragmentShader =
    "uniform sampler2D Texture0;                        \n"
    "                                                   \n"
    "varying vec2 v_texCoord;                           \n"
    "                                                   \n"
    "void main()                                        \n"
    "{                                                  \n"
    "   gl_FragColor = texture2D(Texture0, v_texCoord); \n"
    "}                                                  \n"
    ;

class Textures : public RenderingApplicationDelegate
{
    StateBlock8 m_renderStates;
    RenderFrame m_renderFrame;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        // First load an image from a file
        static Examples::Image image = Examples::tgaFromFile("Assets/Textures/checker.tga");
        
        if (!image)
        {
            application->quit(-1);
        }
        
        // Now create a texture...
        Texture_ texture = m_renderingContext->requestTexture2D(&image.pixels[0], image.width, image.height, image.format);
        
        // ...and bind it to a first texture slot
        m_renderStates.bindTexture(texture, 0);
        
        // Create a program that consists from a vertex and fragment shaders.
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
        
        // Finally configure mesh rendering states as before
        InputLayout inputLayout = m_renderingContext->requestInputLayout(VertexFormat::Position | VertexFormat::TexCoord0);
        
        // Create vertex and index buffer from a textured quad preset
        VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(Examples::TexturedQuad, sizeof(Examples::TexturedQuad));
        IndexBuffer_ indexBuffer = m_renderingContext->requestIndexBuffer(Examples::TriangulatedQuadIndices, sizeof(Examples::TriangulatedQuadIndices));

        m_renderStates.bindVertexBuffer(vertexBuffer);
        m_renderStates.bindIndexBuffer(indexBuffer);
        m_renderStates.bindInputLayout(inputLayout);
    }
 
    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        m_renderFrame.clear();
        
        RenderCommandBuffer& commands = m_renderFrame.entryPoint();
        
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        commands.drawIndexed(0, Renderer::PrimTriangles, 0, 6, &m_renderStates);
    
        m_renderingContext->display(m_renderFrame);
    }
};

dcDeclareApplication(new Textures)
