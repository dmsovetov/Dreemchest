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

#include "Rvm.h"
#include "../Assets/Renderable.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Rvm::Command::Command
Rvm::Command::Command( void )
    : key( 0 )
{
}

// ** Rvm::Command::operator <
bool Rvm::Command::operator < ( const Command& other ) const
{
    return key < other.key;
}

// ** Rvm::Rvm
Rvm::Rvm( const Array<RenderableHandle>& renderables, const Array<TechniqueHandle>& techniques, Renderer::HalWPtr hal )
    : m_hal( hal )
    , m_renderables( renderables )
    , m_techniques( techniques )
{
}

// ** Rvm::push
void Rvm::push( const Matrix4* transform, s32 renderable, s32 technique )
{
    Instance instance;
    instance.transform = transform;
    m_instances.push_back( instance );

    Command cmd;
    cmd.bits.renderable = renderable;
    cmd.bits.technique  = technique;
    cmd.bits.instance   = m_instances.size() - 1;
    m_commands.push_back( cmd );
}

// ** Rvm::flush
void Rvm::flush( void )
{
    // Sort accumulated commands
    std::sort( m_commands.begin(), m_commands.end() );

    // Process all commands
    for( s32 i = 0, n = m_commands.size(); i < n; i++ ) {
        // Decode the command
        const Command& cmd = m_commands[i];

        // Read-lock renderable asset
        const Renderable& renderable = m_renderables[cmd.bits.renderable].readLock();

        // Read-lock material technique
        const Technique& technique = m_techniques[cmd.bits.technique].readLock();

        // Render all chunks
        for( s32 j = 0; j < renderable.chunkCount(); j++ ) {
            m_hal->setVertexBuffer( renderable.vertexBuffer( j ) );
            m_hal->renderIndexed( renderable.primitiveType(), renderable.indexBuffer( j ), 0, renderable.indexBuffer( j )->size() );
        }        
    }

    // Clear processed commands
    m_commands.clear();
    m_instances.clear();
}

} // namespace Scene

DC_END_DREEMCHEST