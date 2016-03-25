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

#include "Asset.h"
#include "AssetHandle.h"
#include "Assets.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

// ** Handle::Handle
Handle::Handle( void ) : m_assets( NULL )
{
    setHandle( NULL, 0 );
}

// ** Handle::Handle
Handle::Handle( Assets* assets, Index index ) : m_assets( NULL ), m_index( 0 )
{
    setHandle( assets, index );
}

// ** Handle::Handle
Handle::Handle( const Handle& other ) : m_assets( NULL ), m_index( 0 )
{
    setHandle( other.assets(), other.index() );
}

// ** Handle::operator =
const Handle& Handle::operator = ( const Handle& other )
{
    setHandle( other.assets(), other.index() );
    return *this;
}

// ** Handle::operator ==
bool Handle::operator == ( const Handle& other ) const
{
    return other.m_assets == m_assets && other.index() == index();
}

// ** Handle::operator <
bool Handle::operator < ( const Handle& other ) const
{
    if( other.m_assets != m_assets ) return other.m_assets < m_assets;
    if( other.index()  != index()  ) return other.index()  < index();
    return false;
}

// ** Handle::operator ->
const Asset* Handle::operator -> ( void ) const
{
    return isValid() ? &asset() : NULL;
}

// ** Handle::operator ->
Asset* Handle::operator -> ( void )
{
    return isValid() ? &asset() : NULL;
}

// ** Handle::setHandle
void Handle::setHandle( Assets* assets, Index index )
{
    m_assets = assets;
    m_index  = index;

#ifdef DC_DEBUG
    m_asset = isValid() ? &asset() : NULL;
#endif  /*  DC_DEBUG    */
}

// ** Handle::forceLoad
bool Handle::forceLoad( void )
{
    return assets()->loadAssetToCache( *this );
}

} // namespace Assets

DC_END_DREEMCHEST