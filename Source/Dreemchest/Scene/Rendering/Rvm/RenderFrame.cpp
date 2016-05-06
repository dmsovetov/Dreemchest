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

#include "RenderFrame.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderFrame::RenderFrame
RenderFrame::RenderFrame( void )
    : m_stateStack( 4096, MaxStateStackDepth )
    , m_allocator( 1024 )
{
    m_entryPoint = &createCommandBuffer();
}

// ** RenderFrame::internBuffer
const void* RenderFrame::internBuffer( const void* data, s32 size )
{
    u8* interned = m_allocator.allocate( size );
    memcpy( interned, data, size );
    return interned;
}

// ** RenderFrame::entryPoint
const RenderCommandBuffer& RenderFrame::entryPoint( void ) const
{
    return *m_entryPoint;
}

// ** RenderFrame::entryPoint
RenderCommandBuffer& RenderFrame::entryPoint( void )
{
    return *m_entryPoint;
}

// ** RenderFrame::createCommandBuffer
RenderCommandBuffer& RenderFrame::createCommandBuffer( void )
{
    m_commandBuffers.push_back( DC_NEW RenderCommandBuffer( *this ) );
    return *m_commandBuffers.back().get();
}

// ** RenderFrame::stateStack
RenderStateStack& RenderFrame::stateStack( void )
{
    return m_stateStack;
}

} // namespace Scene

DC_END_DREEMCHEST