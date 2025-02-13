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
    "cbuffer Projection projection : 0;                     \n"
    "cbuffer Camera     camera     : 1;                     \n"
    "cbuffer Instance   instance   : 2;                     \n"

    "attribute vec4 a_position;                             \n"
    "attribute vec3 a_normal;                               \n"

    "varying float3 v_color;                                \n"

    "void main()                                            \n"
    "{                                                      \n"
    "   vec4  normal    = normalize(instance.inverseTranspose * vec4(a_normal, 1.0));    \n"
    "   vec3  light     = normalize(vec3(-1.0, 1.0, -1.0));   \n"
    "   float intensity = max(dot(light, normal.xyz), 0.0);  \n"
    "   v_color         = vec3(intensity);                  \n"
    "   gl_Position     = projection.transform              \n"
    "                   * camera.transform                  \n"
    "                   * instance.transform                \n"
    "                   * a_position;                       \n"
    "}                                                      \n"
;

static String s_fragmentShader =
    "cbuffer Instance instance : 2;                     \n"

    "varying float3 v_color;                            \n"

    "void main()                                        \n"
    "{                                                  \n"
    "   gl_FragColor = vec4(v_color, instance.alpha);   \n"
    "}                                                  \n"
    ;

class StencilBuffer : public Framework::ApplicationDelegate
{
    RenderItem      m_platform;
    RenderItem      m_object;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        // First initialize render items from mesh files
        m_platform = createMesh("Assets/Meshes/platform.obj");
        m_object   = createMesh("Assets/Meshes/column.obj");

        // Create a simple shader program
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        
        // Modify a default state block by adding common resource bindings
        StateBlock& defaults = m_renderingContext->defaultStateBlock();
        defaults.bindProgram(program);
        
        // Configure projection constant buffer
        {
            Framework::Projection projection    = Framework::Projection::perspective(60.0f, m_window->width(), m_window->height(), 0.1f, 100.0f);
            UniformLayout        layout         = m_renderingContext->requestUniformLayout("Projection", Framework::Projection::Layout);
            ConstantBuffer_      constantBuffer = m_renderingContext->requestConstantBuffer(&projection, sizeof(projection), layout);
            
            // And bind it to a default state block
            defaults.bindConstantBuffer(constantBuffer, 0);
        }
        
        // Configure camera constant buffer
        {
            Framework::Camera camera         = Framework::Camera::lookAt(Vec3(0.0f, 2.0f, -3.0f), Vec3(0.0f, 0.6f, 0.0f));
            UniformLayout    layout         = m_renderingContext->requestUniformLayout("Camera", Framework::Camera::Layout);
            ConstantBuffer_  constantBuffer = m_renderingContext->requestConstantBuffer(&camera, sizeof(camera), layout);
            
            // And bind it to a default state block
            defaults.bindConstantBuffer(constantBuffer, 1);
        }
    }
 
    virtual void handleRenderFrame(RenderFrame& frame, StateStack& stateStack, RenderCommandBuffer& commands, f32 dt) NIMBLE_OVERRIDE
    {
        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Render the platform
        renderItem(commands, m_platform);
        
        const Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
        const Vec3 scale    = Vec3(0.09f, 0.09f, 0.09f);
        
        // Write a platform to the stencil buffer to create a reflection mask.
        {
            StateScope mask = stateStack.newScope();
            mask->setDepthState(LessEqual, false);
            mask->setColorMask(0);
            mask->setStencilFunction(Always, 1);
            mask->setStencilOp(StencilKeep, StencilKeep, StencilReplace);
            
            renderItem(commands, m_platform);
        }
        
        // Now render the reflection
        {
            commands.clear(Rgba(), ClearDepth);
            StateScope reflection = stateStack.newScope();
            reflection->setStencilFunction(Equal, 1);
            reflection->setStencilOp(StencilKeep, StencilKeep, StencilKeep);
            reflection->setCullFace(TriangleFaceFront);
            reflection->setBlend(BlendSrcAlpha, BlendInvSrcAlpha);
            
            renderItem(commands, m_object, Matrix4::scale(1.0f, -1.0f, 1.0f) * Matrix4::rotateXY(0.0f, time()) * Matrix4::translation(position) * Matrix4::scale(scale), 0.3f);
        }

        // Finally render the stanford bunny
        renderItem(commands, m_object, Matrix4::rotateXY(0.0f, time()) * Matrix4::translation(position) * Matrix4::scale(scale));

        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new StencilBuffer)
