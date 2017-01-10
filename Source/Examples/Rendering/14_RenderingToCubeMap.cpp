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
#include "Examples.h"

DC_USE_DREEMCHEST

using namespace Platform;
using namespace Renderer;

static String s_fragmentBackground =
    "cbuffer Projection projection : 0                          \n"

    "uniform samplerCube Texture0;                              \n"

    "void main()                                                \n"
    "{                                                          \n"
    "   vec2 vp  = projection.viewport.zw;                      \n"
    "   vec3 ray = vec3(gl_FragCoord.xy / vp * 2.0 - 1.0, gl_FragCoord.z); \n"
    "   gl_FragColor = textureCube(Texture0, ray);              \n"
    "}                                                          \n"
    ;

static String s_vertexMesh =
    "cbuffer Projection projection : 0;                     \n"
    "cbuffer Camera     camera     : 1;                     \n"
    "cbuffer Instance   instance   : 2;                     \n"

    "varying vec3 v_normal;                                 \n"
    "varying vec3 v_camera;                                 \n"
    "varying vec3 v_vertex;                                 \n"

    "void main()                                            \n"
    "{                                                      \n"
    "   vec4 n   = vec4(gl_Normal, 1.0);                    \n"
    "   v_normal = (instance.inverseTranspose * n).xyz;     \n"
    "   v_vertex = (instance.transform * gl_Vertex).xyz;    \n"
    "   v_camera = camera.position;                         \n"
    "   mat4 mvp = projection.transform                     \n"
    "            * camera.transform                         \n"
    "            * instance.transform                       \n"
    "            ;                                          \n"
    "   gl_Position = mvp * gl_Vertex;                      \n"
    "}                                                      \n"
    ;

static String s_fragmentMesh =
    "uniform samplerCube Texture0;                          \n"
    "uniform samplerCube Texture1;                          \n"

    "varying vec3 v_normal;                                 \n"
    "varying vec3 v_camera;                                 \n"
    "varying vec3 v_vertex;                                 \n"

    "void main()                                            \n"
    "{                                                      \n"
    "   vec3  n = normalize(v_normal);                      \n"
    "   vec3  c = normalize(v_vertex - v_camera);           \n"
    "   vec3  r = reflect(c, n);                            \n"
    "   float f = f_schlick(dot(-c, n), 0.2, 5.0);          \n"

    "   vec4 diffuse    = textureCube(Texture0, n);         \n"
    "   vec4 reflection = textureCube(Texture1, r);         \n"

    "   gl_FragColor = mix(diffuse, reflection, f);         \n"
    "}                                                      \n"
    ;

static String s_vertexConvolution =
    "cbuffer Camera camera : 1;                                                 \n"

    "varying vec4 v_ray;                                                        \n"

    "void main()                                                                \n"
    "{                                                                          \n"
    "   v_ray         = camera.transform * vec4(gl_Vertex.xy, -1.0, 1.0); \n"
    "   gl_Position   = gl_Vertex;                                              \n"
    "}                                                                          \n"
    ;

static String s_fragmentConvolution =
    "uniform samplerCube Texture0;                          \n"
    "cbuffer Convolution convolution : 2;                   \n"
    "cbuffer Kernel      kernel      : 3;                   \n"

    "varying vec4 v_ray;                                    \n"

    "void main()                                                                        \n"
    "{                                                                                              \n"
    "   vec3 n = -normalize(v_ray.xyz);  \n"
    "   vec3 t = vec3(0.0, 1.0, 0.0);                                                               \n"
    "   vec3 b = cross(n, t);                                                                       \n"
    "   vec4 final = vec4(0.0);                                                                     \n"

    "   float factor = ((convolution.power + 2.0) / 2.0 * M_PI);                                    \n"

    "   for (int i = 0; i < kernel.size; i++)                                                       \n"
    "   {                                                                                           \n"
    "       vec4  sample = kernel.samples[i];                                                       \n"
    "       vec3  dir   = directionFromHemisphere(t, b, n, sample);                                 \n"
    "       final += textureCube(Texture0, dir) * pow(sample.z, convolution.power) * sample.w;      \n"
    "   }                                                                                           \n"

    "   gl_FragColor = factor * final / float(kernel.size) * convolution.multiplier;                \n"
    "}                                                                                              \n"
    ;

struct Convolution
{
    f32     multiplier;
    f32     power;
    static const UniformElement Layout[];
};

const UniformElement Convolution::Layout[] =
{
      { "multiplier", UniformElement::Float,   offsetof(Convolution, multiplier) }
    , { "power",      UniformElement::Float,   offsetof(Convolution, power) }
    , { NULL }
};

struct Kernel
{
    enum { MaxSamples = 512 };
    
    s32     size;
    Vec4    samples[MaxSamples];
    static const UniformElement Layout[];
};

const UniformElement Kernel::Layout[] =
{
      { "size",    UniformElement::Integer, offsetof(Kernel, size)                        }
    , { "samples", UniformElement::Vec4,    offsetof(Kernel, samples), Kernel::MaxSamples }
    , { NULL }
};

class RenderingToTexture : public RenderingApplicationDelegate
{
    StateBlock m_renderStates;
    RenderFrame m_renderFrame;
    ConstantBuffer_ m_cameraCBuffer;
    ConstantBuffer_ m_projectionCBuffer;
    ConstantBuffer_ m_instanceCBuffer;
    Examples::MeshStateBlock m_mesh;
    StateBlock m_fullscreenQuad;
    Texture_ m_envmap;
    Texture_ m_diffuse;
    Texture_ m_specular;
    Program m_meshProgram;
    Program m_backgroundProgram;
    
    Examples::Camera s_camera;
    Examples::Projection s_projection;
    Examples::Instance s_instance;

    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }

        // Load mesh from a file
        m_mesh = Examples::createMeshRenderingStates(m_renderingContext, m_renderFrame, "Assets/Meshes/bunny.obj");
        m_envmap = Examples::createEnvFromFiles(m_renderingContext, m_renderFrame, "Assets/Textures/Environments/Arches_E_PineTree_512");

        // Projection cbuffer
        {
            s_projection = Examples::Projection::perspective(60.0f, m_window->width(), m_window->height(), 0.1f, 100.0f);
            
            UniformLayout layout = m_renderingContext->requestUniformLayout("Projection", Examples::Projection::Layout);
            m_projectionCBuffer = m_renderingContext->requestConstantBuffer(&s_projection, sizeof(s_projection), layout);
        }
        
        // Camera cbuffer
        {
            s_camera = Examples::Camera::lookAt(Vec3(0.0f, 2.0f, -2.0f), Vec3(0.0f, 0.5f, 0.0f));
            
            UniformLayout layout = m_renderingContext->requestUniformLayout("Camera", Examples::Camera::Layout);
            m_cameraCBuffer = m_renderingContext->requestConstantBuffer(&s_camera, sizeof(s_camera), layout);
        }
        
        // Camera cbuffer
        {
            UniformLayout layout = m_renderingContext->requestUniformLayout("Instance", Examples::Instance::Layout);
            m_instanceCBuffer = m_renderingContext->requestConstantBuffer(&s_instance, sizeof(s_instance), layout);
        }
        
        m_fullscreenQuad = Examples::createFullscreenRenderingStates(m_renderingContext);
        
        // Create a program that consists from a vertex and fragment shaders.
        m_renderStates.disableBlending();
        m_renderStates.setDepthState(LessEqual, true);
        m_renderStates.bindConstantBuffer(m_projectionCBuffer, 0);
        
        m_meshProgram        = m_renderingContext->requestProgram(s_vertexMesh, s_fragmentMesh);
        m_backgroundProgram  = m_renderingContext->requestProgram(Examples::VertexIdentity, s_fragmentBackground);
        
        // Force a rendering context to construct all queued resources
        m_renderingContext->construct(m_renderFrame);
        
        // Now render a diffuse-convolved environment map
        m_specular = convolve(m_envmap, 128, 160, 128.0f);
        m_diffuse  = convolve(m_envmap, 128, 160);
    }
 
    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        m_renderFrame.clear();
        
        StateStack&         stateStack = m_renderFrame.stateStack();
        RenderCommandBuffer& commands   = m_renderFrame.entryPoint();
        
        StateScope defaults = stateStack.push(&m_renderStates);
        
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        commands.uploadConstantBuffer(m_cameraCBuffer, &s_camera, sizeof(s_camera));
        
        {
            s_projection.viewport = Vec4(0, 0, (f32)m_window->width(), (f32)m_window->height());
            StateScope backgroundPass = stateStack.push(&m_fullscreenQuad);
            StateScope backgroundProgram = stateStack.newScope();
            backgroundProgram->bindProgram(m_backgroundProgram);
            backgroundProgram->bindTexture(m_envmap, 0);
            commands.uploadConstantBuffer(m_projectionCBuffer, &s_projection, sizeof(s_projection));
            commands.drawPrimitives(1, PrimQuads, 0, 4, stateStack);
        }
        
        StateScope meshPass = stateStack.newScope();
        meshPass->bindConstantBuffer(m_cameraCBuffer, 1);
        meshPass->bindConstantBuffer(m_instanceCBuffer, 2);
        meshPass->bindProgram(m_meshProgram);
        meshPass->bindTexture(m_diffuse, 0);
        meshPass->bindTexture(m_specular, 1);
        
        StateScope meshStates = stateStack.push(&m_mesh.states);
        s_instance = Examples::Instance::fromTransform(Matrix4::rotateXY(currentTime() * 0.0003f * 0.0f, currentTime() * 0.0003f));
        commands.uploadConstantBuffer(m_instanceCBuffer, &s_instance, sizeof(s_instance));
        commands.drawPrimitives(0, m_mesh.primitives, 0, m_mesh.size, stateStack);

        m_renderingContext->display(m_renderFrame);
    }
    
    Texture_ convolve(Texture_ env, s32 size, s32 iterations, f32 power = 1.0f)
    {
        verbose("convolution", "performing with cosine kernel of power %2.2f with %d samples in %d iterations...\n", power, Kernel::MaxSamples, iterations);
        
        RenderFrame    frame;
        StateStack&    stateStack = frame.stateStack();
        Kernel         kernel;

        RenderCommandBuffer& commands = frame.entryPoint();

        // Create an output cube map texture
        Texture_ output = m_renderingContext->requestTextureCube(NULL, size, 1, PixelRgba32F, FilterLinear);
        
        // Create a convolution program instance
        Program program = m_renderingContext->requestProgram(s_vertexConvolution, s_fragmentConvolution);
        
        // Create the convolution cbuffer
        UniformLayout convolutionLayout    = m_renderingContext->requestUniformLayout("Convolution", Convolution::Layout);
        ConstantBuffer_ convolutionCBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Convolution), convolutionLayout);
        
        // Create the kernel cbuffer
        UniformLayout kernelLayout    = m_renderingContext->requestUniformLayout("Kernel", Kernel::Layout);
        ConstantBuffer_ kernelCBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Kernel), kernelLayout);
        
        // Configure a convolution render pass
        StateScope convolutionPass = stateStack.newScope();
        convolutionPass->bindProgram(program);
        convolutionPass->bindTexture(env, 0);
        convolutionPass->bindConstantBuffer(m_cameraCBuffer, 1);
        convolutionPass->bindConstantBuffer(convolutionCBuffer, 2);
        convolutionPass->bindConstantBuffer(kernelCBuffer, 3);
        convolutionPass->setBlend(BlendOne, BlendOne);
        
        // Now declare a convolution parameters constant buffer
        Convolution convolution;
        convolution.multiplier = 1.0f / iterations;
        convolution.power      = power;
        commands.uploadConstantBuffer(convolutionCBuffer, &convolution, sizeof(convolution));
        
        // Declare an array of camera up vectors to
        Vec3 up[] =
        {
               Vec3::axisY()
            ,  Vec3::axisY()
            ,  Vec3::axisZ()
            , -Vec3::axisZ()
            ,  Vec3::axisY()
            ,  Vec3::axisY()
        };
        
        Vec3 target[] =
        {
               Vec3::axisX()
            , -Vec3::axisX()
            , -Vec3::axisY()
            ,  Vec3::axisY()
            ,  Vec3::axisZ()
            , -Vec3::axisZ()
        };
        
        u32 commandBufferTime = currentTime();
        for (s32 j = 0; j < iterations; j++)
        {
            StateScope quadStates = stateStack.push(&m_fullscreenQuad);
            
            // Generate and upload a convolution kernel for this iteration
            generateConvolutionKernel(kernel);
            commands.uploadConstantBuffer(kernelCBuffer, &kernel, sizeof(kernel));
            
            // Now render each cube map face
            for (s32 i = 0; i < 6; i++)
            {
                RenderCommandBuffer& renderToCubeMap = commands.renderToCubeMap(output, i);
                if (j == 0)
                {
                    renderToCubeMap.clear(Rgba(0.0f, 0.0f, 0.0f, 0.0f), ClearAll);
                }
                
                Examples::Camera camera = Examples::Camera::lookAt(Vec3::zero(), target[i], up[i]);
                renderToCubeMap.uploadConstantBuffer(m_cameraCBuffer, &camera, sizeof(camera));
                renderToCubeMap.drawPrimitives(0, PrimQuads, 0, 4, stateStack);
            }
        }
        commandBufferTime = currentTime() - commandBufferTime;

        // Render a diffuse-convolved environment map
        u32 renderingTime = currentTime();
        m_renderingContext->display(frame, true);
        renderingTime = currentTime() - renderingTime;
        
        // Remove allocated resources
        m_renderingContext->deleteConstantBuffer(kernelCBuffer);
        m_renderingContext->deleteConstantBuffer(convolutionCBuffer);
        m_renderingContext->deleteUniformLayout(kernelLayout);
        m_renderingContext->deleteUniformLayout(convolutionLayout);
        m_renderingContext->deleteProgram(program);
        
        verbose("convolution", "\tfinished in %d ms, allocated %d/%d bytes [command buffer generated in %d ms, rendered in %d ms]\n", commandBufferTime + renderingTime, frame.allocatedBytes(), frame.allocationCapacity(), commandBufferTime, renderingTime);
        
        return output;
    }
    
    void generateConvolutionKernel(Kernel& kernel)
    {
        kernel.size = Kernel::MaxSamples;
        
        for (s32 i = 0; i < kernel.size; i++)
        {
            f32 theta = rand0to1() * Pi * 2.0f;
            f32 phi   = rand0to1() * Pi * 0.5f;
            kernel.samples[i] = Vec4(cosf(theta), sinf(theta), cosf(phi), sinf(phi));
        }
    }
};

dcDeclareApplication(new RenderingToTexture)
