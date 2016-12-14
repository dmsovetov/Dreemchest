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

DC_USE_DREEMCHEST

using namespace Platform;
using namespace Renderer;

class RendererInitialization : public ApplicationDelegate
{
    virtual void handleLaunched( Application* application )
    {
        // Set the default log handler.
        Logger::setStandardLogger();

        // Create a 800x600 window like we did in previous example.
        // This window will contain a rendering viewport.
        Window* window = Window::create( 800 / 4, 600 / 4 );

        // Create a rendering view.
        RenderView* view   = Renderer::createOpenGLView( window->handle(), Renderer::PixelD24S8 );

    #if DEV_DEPRECATED_HAL
        // Now create the main renderer interface called HAL (hardware abstraction layer).
        m_hal = Hal::create( OpenGL, view );
        m_renderingContext = Renderer::createDeprecatedRenderingContext(m_hal);
    #else
        m_renderingContext = Renderer::RenderingContext::create(Renderer::RenderingContext::OpenGL2);
    #endif  /*  #if DEV_DEPRECATED_HAL  */

        static f32 vertices[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f,  1.0f, 0.0f,
        };
        static u16 indices[] = {
            0, 1, 2
        };
        
        // Request all required resources from a rendering context
        Renderer::InputLayout   inputLayout   = m_renderingContext->requestInputLayout(0);
        Renderer::VertexBuffer_ vertexBuffer  = m_renderingContext->requestVertexBuffer(vertices, sizeof(vertices));
        Renderer::IndexBuffer_  indexBuffer   = m_renderingContext->requestIndexBuffer(indices, sizeof(indices));
        
        // Setup a render state block that will be used during rendering
        m_renderState.bindVertexBuffer(vertexBuffer);
        m_renderState.bindIndexBuffer(indexBuffer);
        m_renderState.bindInputLayout(inputLayout);
        
        // Finally subscribe to updates events.
        window->subscribe<Window::Update>( dcThisMethod( RendererInitialization::handleWindowUpdate ) );
    }

    // Called each frame and renders a single frame
    virtual void handleWindowUpdate( const Window::Update& e )
    {
        // Get an entry point command buffer
        Renderer::CommandBuffer& commands = m_renderFrame.entryPoint();

        // Now fill a command buffer with required commands
        
        // First clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), Renderer::ClearAll);
        
        // Now emit a drawIndexed command with a render state block configured upon initialization
        commands.drawIndexed(0, Renderer::PrimTriangles, 0, 3, &m_renderState);
        
        // Rendering frame is now ready, so pass it to RVM to display it on a screen.
        m_renderingContext->display(m_renderFrame);

    #if DEV_DEPRECATED_HAL
        m_hal->present();
    #endif  /*  #if DEV_DEPRECATED_HAL  */
    }
    
#if DEV_DEPRECATED_HAL
    HalPtr                          m_hal;              //!< Rendering HAL.
#endif  /*  #if DEV_DEPRECATED_HAL  */
    Renderer::RenderingContextPtr   m_renderingContext; //!< Rendering context instance.
    
    Renderer::StateBlock            m_renderState;      //!< Render state block is a composition of rendering states required for rendering.
    Renderer::RenderFrame           m_renderFrame;      //!< A render frame instance records all data required to render a single frame.
};

// Now declare an application entry point with RendererInitialization application delegate.
dcDeclareApplication( new RendererInitialization )
