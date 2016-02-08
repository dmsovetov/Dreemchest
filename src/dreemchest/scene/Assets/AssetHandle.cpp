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

#include "AssetHandle.h"
#include "Assets.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** AssetHandle::AssetHandle
AssetHandle::AssetHandle( void ) : m_assets( NULL )
{
    
}

// ** AssetHandle::AssetHandle
AssetHandle::AssetHandle( Assets* assets, SlotIndex32 slot ) : m_assets( assets ), m_slot( slot )
{
    
}

// ** AssetHandle::AssetHandle
AssetHandle::AssetHandle( const AssetHandle& other ) : m_assets( other.m_assets ), m_slot( other.m_slot )
{
    
}

// ** AssetHandle::operator =
const AssetHandle& AssetHandle::operator = ( const AssetHandle& other )
{
    m_assets = other.m_assets;
    m_slot   = other.m_slot;
    return *this;
}

// ** AssetHandle::operator ==
bool AssetHandle::operator == ( const AssetHandle& other ) const
{
    return other.m_assets == m_assets && other.m_slot == m_slot;
}

// ** AssetHandle::operator <
bool AssetHandle::operator < ( const AssetHandle& other ) const
{
    if( other.m_assets != m_assets ) return other.m_assets < m_assets;
    if( other.m_slot != m_slot ) return other.m_slot < m_slot;
    return false;
}

// ** AssetHandle::operator ->
const Asset* AssetHandle::operator -> ( void ) const
{
    return isValid() ? &asset() : NULL;
}

// ** AssetHandle::operator ->
Asset* AssetHandle::operator -> ( void )
{
    return isValid() ? &asset() : NULL;
}

// ** AssetHandle::asset
const Asset& AssetHandle::asset( void ) const
{
    DC_BREAK_IF( !isValid() );
    return m_assets->assetAtSlot( m_slot );
}

// ** AssetHandle::asset
Asset& AssetHandle::asset( void )
{
    DC_BREAK_IF( !isValid() );
    return const_cast<Asset&>( m_assets->assetAtSlot( m_slot ) );
}

// ** AssetHandle::isValid
bool AssetHandle::isValid( void ) const
{
    return m_assets && m_assets->isValidSlot( m_slot );
}

// ** AssetHandle::isLoaded
bool AssetHandle::isLoaded( void ) const
{
    return isValid() && (operator->())->state() == Asset::Loaded;
}

// ** AssetHandle::slot
SlotIndex32 AssetHandle::slot( void ) const
{
    return m_slot;
}

// ** AssetHandle::assets
Assets* AssetHandle::assets( void ) const
{
    return m_assets;
}

} // namespace Scene

DC_END_DREEMCHEST