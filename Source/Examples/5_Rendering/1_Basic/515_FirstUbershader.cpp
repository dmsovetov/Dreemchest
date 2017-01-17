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

// A ubershader is a regular shader with a set of options that dictate
// what features should be used in a compile time. Each set of options
// produces a unique shader permutation that is used during rendering.

// Here we define a shader that has a single option - F_Pink
// that controlls an output color of all fragments output
// by a shader program.
static String s_fragmentShader =
    "void main()                                    \n"
    "{                                              \n"
    "#if defined( F_Pink )                          \n"
    "    gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);   \n"
    "#else                                          \n"
    "    gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);   \n"
    "#endif  // F_Pink                              \n"
    "}"
    ;

// The last one thing we need is a feature layout that tells
// a rendering context how to map from pipeline features to
// actual shader options.

// Declare a user-defined feature constant
PipelineFeatures Pink = BIT(0);

static PipelineFeature s_features[] =
{
    // This item tells a rendering context that 'F_Pink' option
    // should be passed to a shader program each time the 'Pink'
    // option was set by user.
    { "F_Pink", PipelineFeature::user(Pink) },
    
    // A sentinel item
    { NULL }
};

class FirstUbershader : public RenderingApplicationDelegate
{
    StateBlock8 m_renderStates;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }

        InputLayout inputLayout = m_renderingContext->requestInputLayout(VertexFormat::Position);
        
        // Create a vertex buffer from a triangle vertex buffer preset.
        VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(Examples::Triangle, sizeof(Examples::Triangle));
        
        // Create a shader program. Nothing special in vertex shader, so use a preset.
        Program program = m_renderingContext->requestProgram(Examples::VertexIdentity, s_fragmentShader);
        
        // Create a feature layout
        FeatureLayout featureLayout = m_renderingContext->requestPipelineFeatureLayout(s_features);
        
        m_renderStates.bindVertexBuffer(vertexBuffer);
        m_renderStates.bindInputLayout(inputLayout);
        m_renderStates.bindProgram(program);
        
        // Bind feature layout and set a 'Pink' option
        m_renderStates.bindFeatureLayout(featureLayout);
        m_renderStates.enableFeatures(Pink);
    }

    virtual void handleRenderFrame(f32 dt) NIMBLE_OVERRIDE
    {
        RenderFrame& frame = m_renderingContext->allocateFrame();
        RenderCommandBuffer& commands = frame.entryPoint();

        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        commands.drawPrimitives(0, Renderer::PrimTriangles, 0, 3, m_renderStates);
        
        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new FirstUbershader)
