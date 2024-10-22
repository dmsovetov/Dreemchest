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

#include "SystemGroup.h"
#include "../Entity/Entity.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

// ** SystemGroup::SystemGroup
SystemGroup::SystemGroup( EcsWPtr ecs, const String& name, u32 mask ) : m_ecs( ecs ), m_name( name ), m_mask( mask ), m_isLocked( false )
{

}

// ** SystemGroup::mask
u32 SystemGroup::mask( void ) const
{
    return m_mask;
}

// ** SystemGroup::update
void SystemGroup::update( u32 currentTime, f32 dt )
{
    m_isLocked = true;

    for( u32 i = 0, n = ( u32 )m_systems.size(); i < n; i++ ) {
        // Update the system
        m_systems[i].m_system->update( currentTime, dt );

    #if DC_ECS_ITERATIVE_INDEX_REBUILD
        // Now rebuild & cleanup entities
        m_ecs->rebuildChangedEntities();
        m_ecs->cleanupRemovedEntities();
    #endif  /*  DC_ECS_ITERATIVE_INDEX_REBUILD  */
    }

    m_isLocked = false;
}

} // namespace Ecs

DC_END_DREEMCHEST
