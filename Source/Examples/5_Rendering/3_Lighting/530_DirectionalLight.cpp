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
    "cbuffer Projection projection : 0;                                         \n"
    "cbuffer Camera     camera     : 1;                                         \n"
    "cbuffer Instance   instance   : 2;                                         \n"
    "                                                                           \n"
    "attribute vec4 a_position;                                                 \n"
    "attribute vec3 a_normal;                                                   \n"
    "                                                                           \n"
    "varying float3 v_wsVertex;                                                 \n"
    "varying float3 v_wsNormal;                                                 \n"
    "                                                                           \n"
    "void main()                                                                \n"
    "{                                                                          \n"
    "   v_wsNormal  = (instance.inverseTranspose                                \n"
    "               * vec4(a_normal, 1.0)).xyz                                  \n"
    "               ;                                                           \n"

    "   v_wsVertex  = (instance.transform * a_position).xyz;                    \n"

    "   gl_Position = projection.transform                                      \n"
    "               * camera.transform                                          \n"
    "               * vec4(v_wsVertex, 1.0)                                     \n"
    "               ;                                                           \n"
    "}                                                                          \n"
;

static String s_fragmentShader =
    "cbuffer Camera     camera   : 1;                                           \n"
    "cbuffer Instance   instance : 2;                                           \n"
    "cbuffer Light      light    : 3;                                           \n"

    "varying float3 v_wsVertex;                                                 \n"
    "varying float3 v_wsNormal;                                                 \n"

    "vec2 phong(vec3 L, vec3 N, vec3 V)                                         \n"
    "{                                                                          \n"
    "   float diffuse  = max(dot(L, N), 0.0);                                   \n"
    "   vec3  R        = reflect(L, N);                                         \n"
    "   float specular = max(dot(V, R), 0.0);                                   \n"
    "   return vec2(diffuse, specular);                                         \n"
    "}                                                                          \n"

    "void main()                                                                \n"
    "{                                                                          \n"
    "   vec3 normal = normalize(v_wsNormal);                                    \n"
    "   vec3 view   = normalize(v_wsVertex - camera.position);                  \n"

    "   vec2 l     = phong(light.direction, normal, view);                      \n"
    "   vec3 color = light.intensity * light.color * (l.x + pow(l.y, 32.0));    \n"

    "   gl_FragColor = vec4(color, 1.0);                                        \n"
    "}                                                                          \n"
    ;

struct Light
{
    Rgb     color;
    f32     intensity;
    Vec3    direction;
    static UniformElement Layout[];
} s_light;

UniformElement Light::Layout[] =
{
      { "color",       UniformElement::Vec3,  offsetof(Light, color)        }
    , { "intensity",   UniformElement::Float, offsetof(Light, intensity)    }
    , { "direction",   UniformElement::Vec3,  offsetof(Light, direction)    }
    , { NULL }
};

class PointLights : public Framework::ApplicationDelegate
{
    StateBlock8     m_renderStates;
    ConstantBuffer_ m_lightConstantBuffer;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        setCameraPosition(Vec3(0.0f, 2.0f, -2.0f));

        // Configure a light constant buffer
        {
            UniformLayout layout  = m_renderingContext->requestUniformLayout("Light", Light::Layout);
            m_lightConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Light), layout);
            m_renderStates.bindConstantBuffer(m_lightConstantBuffer, 3);
        }
        
        // Create a simple shader program
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
    }
 
    virtual void handleRenderFrame(RenderFrame& frame, StateStack& stateStack, RenderCommandBuffer& commands, f32 dt) NIMBLE_OVERRIDE
    {
        // Push the default state
        StateScope defaultScope = stateStack.push(&m_renderStates);
        
        // Update light parameters
        {
            f32 st = sinf(time());
            f32 ct = cosf(time());

            s_light.color     = Rgb(1.0f, 1.0f, 1.0f);
            s_light.intensity = 0.5f;
            s_light.direction = -Vec3(st, -2.0f, ct);
            s_light.direction.normalize();
            commands.uploadConstantBuffer(m_lightConstantBuffer, &s_light, sizeof(s_light));
        }
        
        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);

        // Render the scene
        renderColumnsScene(commands);
        
        // Render light sources for debugging
        renderPinkItem(commands, stateStack, m_sphere, Matrix4::translation(s_light.direction * 2.0f) * Matrix4::scale(0.05f, 0.05f, 0.05f));
        
        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new PointLights)
