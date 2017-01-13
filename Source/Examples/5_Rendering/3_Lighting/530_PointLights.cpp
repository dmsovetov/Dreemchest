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

static String s_vertexShader =
    "cbuffer Projection projection : 0;                                         \n"
    "cbuffer Camera     camera     : 1;                                         \n"
    "cbuffer Instance   instance   : 2;                                         \n"
    "                                                                           \n"
    "varying vec3 v_wsVertex;                                                   \n"
    "varying vec3 v_wsNormal;                                                   \n"
    "                                                                           \n"
    "void main()                                                                \n"
    "{                                                                          \n"
    "   v_wsNormal  = normalize(instance.inverseTranspose * vec4(gl_Normal, 1.0)).xyz;    \n"
    "   v_wsVertex  = (instance.transform * gl_Vertex).xyz;                     \n"
    "                                                                           \n"
    "   gl_Position = projection.transform                                      \n"
    "               * camera.transform                                          \n"
    "               * vec4(v_wsVertex, 1.0);                                    \n"
    "}                                                                          \n"
;

static String s_fragmentShader =
    "cbuffer Camera     camera   : 1;                                           \n"
    "cbuffer Instance   instance : 2;                                           \n"
    "cbuffer Light      light    : 3;                                           \n"
    "                                                                           \n"
    "varying vec3 v_wsVertex;                                                   \n"
    "varying vec3 v_wsNormal;                                                   \n"
    "                                                                           \n"
    "vec2 blinn(vec3 light, vec3 normal, vec3 view)                             \n"
    "{                                                                          \n"
    "    float ndotl     = dot(normal, light);                                  \n"
    "    vec3  reflected = light - 2.0 * ndotl * normal;                        \n"
    "    float rdotv     = dot(reflected, view);                                \n"
    "    return vec2(ndotl, rdotv);                                             \n"
    "}                                                                          \n"
    "                                                                           \n"
    "vec2 lit(float ndotl, float rdotv, float m)                                \n"
    "{                                                                          \n"
    "    float diff = max(0.0, ndotl);                                          \n"
    "    float spec = step(0.0, ndotl) * max(0.0, rdotv);                       \n"
    "    return vec2(diff, spec);                                               \n"
    "}                                                                          \n"
    "                                                                           \n"
    "float fresnel(float ndotl, float bias, float pow)                          \n"
    "{                                                                          \n"
    "    float facing = (1.0 - ndotl);                                          \n"
    "    return max(bias + (1.0 - bias) * pow(facing, pow), 0.0);               \n"
    "}                                                                          \n"
    "                                                                           \n"
    "void main()                                                                \n"
    "{                                                                          \n"
    "   vec3  norm  = normalize(v_wsNormal);                                    \n"
    "   vec3  ldir  = normalize(light.position - v_wsVertex);                   \n"
    "   vec3  cdir  = normalize(v_wsVertex - camera.position);                  \n"
    "   vec2  bln   = blinn(ldir, norm, cdir);                                  \n"
    "   vec2  lc    = lit(bln.x, bln.y, 0.95);                                  \n"
    "   float f     = fresnel(bln.x, 0.2, 5.0);                                 \n"
    "   float spec  = f * pow(lc.y, 64.0);                                      \n"
    "   vec3  final = light.color * light.intensity * (lc.x + spec);            \n"
    "   gl_FragColor = vec4(final, 1.0);                                        \n"
    "}                                                                          \n"
    ;

struct Light
{
    Vec3    position;
    f32     range;
    Rgb     color;
    f32     intensity;
    static UniformElement Layout[];
} s_light;

UniformElement Light::Layout[] =
{
      { "position",  UniformElement::Vec3,  offsetof(Light, position)  }
    , { "range",     UniformElement::Float, offsetof(Light, range)     }
    , { "color",     UniformElement::Vec3,  offsetof(Light, color)     }
    , { "intensity", UniformElement::Float, offsetof(Light, intensity) }
    , { NULL }
};

class PointLights : public RenderingApplicationDelegate
{
    StateBlock8     m_renderStates;
    RenderItem      m_platform;
    RenderItem      m_object;
    ConstantBuffer_ m_instanceConstantBuffer;
    ConstantBuffer_ m_lightConstantBuffer;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        // First initialize objects that will be rendered
        m_platform = Examples::createRenderItemFromMesh(m_renderingContext, "Assets/Meshes/platform.obj");
        m_object    = Examples::createRenderItemFromMesh(m_renderingContext, "Assets/Meshes/bunny_decimated.obj");

        static const Vec3 s_cameraPos    = Vec3(0.0f, 2.0f, -2.0f);
        static const Vec3 s_cameraTarget = Vec3(0.0f, 0.6f,  0.0f);
        
        // Configure projection constant buffer
        {
            Examples::Projection projection     = Examples::Projection::perspective(60.0f, m_window->width(), m_window->height(), 0.1f, 100.0f);
            UniformLayout        layout         = m_renderingContext->requestUniformLayout("Projection", Examples::Projection::Layout);
            ConstantBuffer_      constantBuffer = m_renderingContext->requestConstantBuffer(&projection, sizeof(projection), layout);
            m_renderStates.bindConstantBuffer(constantBuffer, 0);
        }
        
        // Configure camera constant buffer
        {
            Examples::Camera camera         = Examples::Camera::lookAt(s_cameraPos, s_cameraTarget);
            UniformLayout    layout         = m_renderingContext->requestUniformLayout("Camera", Examples::Camera::Layout);
            ConstantBuffer_  constantBuffer = m_renderingContext->requestConstantBuffer(&camera, sizeof(camera), layout);
            m_renderStates.bindConstantBuffer(constantBuffer, 1);
        }
        
        // Configure instance constant buffer
        {
            UniformLayout layout = m_renderingContext->requestUniformLayout("Instance", Examples::Instance::Layout);
            m_instanceConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Examples::Instance), layout);
            m_renderStates.bindConstantBuffer(m_instanceConstantBuffer, 2);
        }
        
        // Configure a light constant buffer
        {
            s_light.position  = Vec3(-1.0f, 1.0f, -1.0f);
            s_light.color     = Rgb(1.0f, 1.0f, 1.0f);
            s_light.intensity = 1.0f;
            s_light.range     = 1.0f;
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Light", Light::Layout);
            m_lightConstantBuffer = m_renderingContext->requestConstantBuffer(&s_light, sizeof(s_light), uniformLayout);
            m_renderStates.bindConstantBuffer(m_lightConstantBuffer, 3);
        }
        
        // Create a simple shader program
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
    }
 
    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        RenderFrame frame(m_renderingContext->defaultStateBlock());
        
        RenderCommandBuffer& commands = frame.entryPoint();
        
        // In this sample we will use a state stack
        StateStack& states = frame.stateStack();
        
        // Push the default state
        StateScope defaultScope = states.push(&m_renderStates);
        
        f32 time = currentTime() * 0.001f;
        
        // Update light parameters
        {
            f32 st           = sinf(time);
            f32 ct           = cosf(time);
            s_light.color    = Rgb(st, 1.0f, ct);
            commands.uploadConstantBuffer(m_lightConstantBuffer, &s_light, sizeof(s_light));
        }
        
        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);

        // Render the platform
        renderItem(commands, m_platform, Matrix4());
        
        const Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
        const Vec3 scale    = Vec3(0.9f, 0.9f, 0.9f);
        
        // Finally render the stanford bunny
        renderItem(commands, m_object, Matrix4::rotateXY(0.0f, time * 0.5f) * Matrix4::translation(position) * Matrix4::scale(scale));

        m_renderingContext->display(frame);
    }
    
    void renderItem(RenderCommandBuffer& commands, const RenderItem& item, const Matrix4& transform)
    {
        Examples::Instance instance = Examples::Instance::fromTransform(transform);
        commands.uploadConstantBuffer(m_instanceConstantBuffer, &instance, sizeof(instance));
        commands.drawItem(0, item);
    }
};

dcDeclareApplication(new PointLights)
