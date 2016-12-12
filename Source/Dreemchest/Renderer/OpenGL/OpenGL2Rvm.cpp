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

#include "OpenGL2Rvm.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

#if !DEV_DEPRECATED_HAL
// ** OpenGL2Rvm::OpenGL2Rvm
OpenGL2Rvm::OpenGL2Rvm(OpenGL2RenderingContext* renderingContext)
    : RenderingContext(renderingContext)
{
    
}

// ** OpenGL2Rvm::executeCommandBuffer
void OpenGL2Rvm::executeCommandBuffer(const RenderFrame& frame, const CommandBuffer& commands)
{
    NIMBLE_NOT_IMPLEMENTED;
}
    
// ** OpenGL2Rvm::reset
void OpenGL2Rvm::reset( void )
{
    // Reset the face culling
    m_hal->setCulling( Renderer::TriangleFaceBack );
    
    // Set the default polygon mode
    m_hal->setPolygonMode( Renderer::PolygonFill );
    
    // Set the default shader
    m_hal->setShader( NULL );
    
    // Set the default vertex buffer
    m_hal->setVertexBuffer( NULL );
    
    // Set default textures
    for( s32 i = 0; i < 8; i++ ) {
        m_hal->setTexture( i, NULL );
    }
    
    // Enable the depth test back
    m_hal->setDepthTest( true, Renderer::LessEqual );
    
    // Disable the alpha test
    m_hal->setAlphaTest( Renderer::CompareDisabled );
}
#endif  /*  #if !DEV_DEPRECATED_HAL */
    
} // namespace Renderer

DC_END_DREEMCHEST
