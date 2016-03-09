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

#include "Introspection.h"

DC_BEGIN_DREEMCHEST

namespace Introspection {

// ------------------------------------------------------------- Member ------------------------------------------------------------- //

// ** Member::Member
Member::Member( CString name )
    : m_name( name )
{

}

// ** Member::name
CString Member::name( void ) const
{
    return m_name;
}

// ----------------------------------------------------------- MetaObject ----------------------------------------------------------- //

// ** MetaObject::MetaObject
MetaObject::MetaObject( CString name, Member** members, s32 memberCount )
    : m_super( NULL ), m_name( name ), m_members( members ), m_memberCount( memberCount )
{
}

// ** MetaObject::MetaObject
MetaObject::MetaObject( const MetaObject* super, CString name, Member** members, s32 memberCount )
    : m_super( super ), m_name( name ), m_members( members ), m_memberCount( memberCount )
{
}

// ** MetaObject::super
const MetaObject* MetaObject::super( void ) const
{
    return m_super;
}

// ** MetaObject::memberCount
s32 MetaObject::memberCount( void ) const
{
    return m_memberCount;
}

// ** MetaObject::member
const Member* MetaObject::member( s32 index ) const
{
    DC_BREAK_IF( index < 0 || index >= memberCount() );
    return m_members[index];
}

// ** MetaObject::member
Member* MetaObject::member( s32 index )
{
    DC_BREAK_IF( index < 0 || index >= memberCount() );
    return m_members[index];
}

// ** MetaObject::findMember
const Member* MetaObject::findMember( CString name ) const
{
    return const_cast<MetaObject*>( this )->findMember( name );
}

// ** MetaObject::findMember
Member* MetaObject::findMember( CString name )
{
    for( s32 i = 0; i < memberCount(); i++ ) {
        Member* m = member( i );

        if( strcmp( m->name(), name ) == 0 ) {
            return m;
        }
    }

    return NULL;
}

} // namespace Introspection

DC_END_DREEMCHEST