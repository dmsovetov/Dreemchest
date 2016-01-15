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

#include "AssetPool.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** AbstractAssetPool::findFile
const AbstractAssetPool::File& AbstractAssetPool::findFile( const AssetId& id ) const
{
    AssetFiles::const_iterator i = m_files.find( id );
    DC_BREAK_IF( i == m_files.end() );
    return i->second;
}

// ** AbstractAssetPool::addFile
void AbstractAssetPool::addFile( const AssetId& id, AssetSlot asset, AbstractAssetFormat* format )
{
    DC_BREAK_IF( m_files.find( id ) != m_files.end() );
    m_files[id] = File( id, asset, format );
}

// ** AbstractAssetPool::removeFile
AssetSlot AbstractAssetPool::removeFile( const AssetId& id )
{
    AssetFiles::const_iterator i = m_files.find( id );
    DC_BREAK_IF( i == m_files.end() );
    AssetSlot asset = i->second.asset;
    m_files.erase( i );
    return asset;
}

} // namespace Scene

DC_END_DREEMCHEST