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
    "varying vec3 v_wsVertex;                                                   \n"
    "varying vec3 v_wsNormal;                                                   \n"
    "                                                                           \n"
    "void main()                                                                \n"
    "{                                                                          \n"
    "   v_wsNormal  = (instance.inverseTranspose * vec4(gl_Normal, 1.0)).xyz;   \n"
    "   v_wsVertex  = (instance.transform * gl_Vertex).xyz;                     \n"
    "                                                                           \n"
    "   gl_Position = projection.transform                                      \n"
    "               * camera.transform                                          \n"
    "               * vec4(v_wsVertex, 1.0)                                     \n"
    "               ;                                                           \n"
    "}                                                                          \n"
    ;

static String s_fragmentShader =
    "cbuffer Projection projection : 0;                                                     \n"
    "cbuffer Camera     camera     : 1;                                                     \n"
    "cbuffer Instance   instance   : 2;                                                     \n"

    "uniform samplerCube Texture0;                                                          \n"
    "varying vec3 v_wsVertex;                                                               \n"
    "varying vec3 v_wsNormal;                                                               \n"
                                                  
    "const float Air   = 1.0;                                                               \n"
    "const float Glass = 1.51714;                                                           \n"
    "const float Eta   = Air / Glass;                                                       \n"
    "const float R0    = ((Air - Glass) * (Air - Glass)) / ((Air + Glass) * (Air + Glass)); \n"
    "const float D     = 0.2;                                                               \n"
                            
    "float fresnel(float dp, float f0)                                                      \n"
    "{                                                                                      \n"
    "   float facing = min(1.0 - dp, 1.0);                                                  \n"
    "   return max(f0 + (1.0 - f0) * pow(facing, 5.0), 0.0);                                \n"
    "}                                                                                      \n"

    "void main()                                                                            \n"
    "{                                                                                      \n"
    "   vec3 n  = normalize(v_wsNormal);                                                    \n"
    "   vec3 v  = normalize(camera.position - v_wsVertex);                                  \n"
    "   vec3 reflected = reflect(-v, n);                                                    \n"

    "   float fresnel    = fresnel(dot(n, v), R0);                                          \n"
    "   vec4  reflection = textureCube(Texture0, reflected);                                \n"
          
    "   vec4 refraction;                                                                    \n"
    "   refraction.r = textureCube(Texture0, refract(-v, n, R0 - D  )).r;                   \n"
    "   refraction.g = textureCube(Texture0, refract(-v, n, R0 + 0.0)).g;                   \n"
    "   refraction.b = textureCube(Texture0, refract(-v, n, R0 + D  )).b;                   \n"
    "   refraction.a = 1.0;                                                                 \n"

    "   gl_FragColor = mix(refraction, reflection, 0.0);                                    \n"
    "}                                                                                      \n"
    ;

class Envmapping : public Framework::ApplicationDelegate
{
    StateBlock8     m_renderStates;
    RenderItem      m_object;
    RenderItem      m_skyBox;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        // Create environment map
        Texture_ envmap = createCubeMap("Assets/Textures/Environments/MonValley_DirtRoad");

        // Now initialize objects that will be rendered
        m_object   = createMesh("Assets/Meshes/bunny_decimated.obj");
        m_skyBox   = createSkyBox(envmap);
        
        setCameraPosition(Vec3(0.0f, 2.0f, -1.5f));
        
        // Create a simple shader program
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
        m_renderStates.bindTexture(envmap, 0);
    }
 
    virtual void handleRenderFrame(RenderFrame& frame, StateStack& stateStack, RenderCommandBuffer& commands, f32 dt) NIMBLE_OVERRIDE
    {
        // Push the default state
        StateScope defaultScope = stateStack.push(&m_renderStates);
        
        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Render the skybox
        commands.drawItem(0, m_skyBox);
        
        // Finally render the stanford bunny
        renderItem(commands, m_object, Matrix4::rotateXY(0.0f, time()) * Matrix4::translation(Vec3(0.0f, 1.0f, 0.0f)));

        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new Envmapping)
