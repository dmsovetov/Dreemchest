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

#ifndef __DC_Assets_AssetHandle_Hpp__
#define __DC_Assets_AssetHandle_Hpp__

#include "Assets.h"

DC_BEGIN_DREEMCHEST

namespace Assets {
    
// ------------------------------------------------------- Handle ------------------------------------------------------- //

// ** Handle::readOnlyData
template<typename TAsset>
const TAsset& Handle::readOnlyData( void ) const
{
    return assets()->readOnlyAssetData<TAsset>( asset() );
}

// ** Handle::writableData
template<typename TAsset>
TAsset& Handle::writableData( void )
{
    return assets()->writableAssetData<TAsset>( asset() );
}

// ** Handle::index
NIMBLE_INLINE const Index& Handle::index( void ) const
{
    return m_index;
}

// ** Handle::isValid
NIMBLE_INLINE bool Handle::isValid( void ) const
{
    return m_assets.valid() && m_assets->isIndexValid( index() );
}

// ** Handle::isLoaded
NIMBLE_INLINE bool Handle::isLoaded( void ) const
{
    return isValid() && (operator->())->state() == Asset::Loaded;
}

// ** Handle::asset
NIMBLE_INLINE const Asset& Handle::asset( void ) const
{
    NIMBLE_ABORT_IF( !isValid(), "asset handle is invalid" );
    return m_assets->assetAtIndex( index() );
}

// ** Handle::asset
NIMBLE_INLINE Asset& Handle::asset( void )
{
    NIMBLE_ABORT_IF( !isValid(), "asset handle is invalid" );
    return m_assets->assetAtIndex( index() );
}

// ** Handle::assets
NIMBLE_INLINE Assets* Handle::assets( void ) const
{
    return m_assets.get();
}

// ** Handle::readLock
template<typename TAsset>
const TAsset& Handle::readLock( void ) const
{
    return assets()->acquireReadLock<TAsset>( asset() );
}

// ** Handle::writeLock
template<typename TAsset>
WriteLock<TAsset> Handle::writeLock( void )
{
    return assets()->acquireWriteLock<TAsset>( *this );
}

// ------------------------------------------------------- DataHandle ------------------------------------------------------- //

// ** DataHandle::DataHandle
template<typename TAsset>
DataHandle<TAsset>::DataHandle( void )
{
    
}

// ** DataHandle::DataHandle
template<typename TAsset>
DataHandle<TAsset>::DataHandle( const Handle& asset )
{
    NIMBLE_ABORT_IF( isValid() && asset.isValid() && asset->type() != Asset::typeId<TAsset>(), "an asset handle types do not match" );
    this->setHandle( asset.assets(), asset.index() );
}

// ** DataHandle::operator =
template<typename TAsset>
const DataHandle<TAsset>& DataHandle<TAsset>::operator = ( const DataHandle<TAsset>& other )
{
    this->setHandle( other.assets(), other.index() );
    return *this;
}

// ** DataHandle::operator ->
template<typename TAsset>
const TAsset* DataHandle<TAsset>::operator -> ( void ) const
{
    return &readOnlyData<TAsset>();
}

// ** DataHandle::operator *
template<typename TAsset>
const TAsset& DataHandle<TAsset>::operator * ( void ) const
{
    return readOnlyData<TAsset>();
}

#ifdef DC_DEBUG
// ** DataHandle<TAsset>::setHandle
template<typename TAsset>
void DataHandle<TAsset>::setHandle( Assets* assets, Index index )
{
    Handle::setHandle( assets, index );
    m_data = &data<TAsset>();
}
#endif  /*  DC_DEBUG    */

// ** DataHandle::name
template<typename TAsset>
const String& DataHandle<TAsset>::name( void ) const
{
    return asset().name();
}

// ** DataHandle::uniqueId
template<typename TAsset>
const AssetId& DataHandle<TAsset>::uniqueId( void ) const
{
    return asset().uniqueId();
}

// ** DataHandle::readLock
template<typename TAsset>
NIMBLE_INLINE const TAsset& DataHandle<TAsset>::readLock( void ) const
{
    return Handle::readLock<TAsset>();
}

// ** DataHandle::writeLock
template<typename TAsset>
WriteLock<TAsset> DataHandle<TAsset>::writeLock( void )
{
    return Handle::writeLock<TAsset>();
}

// ------------------------------------------------------- WriteLock ------------------------------------------------------- //

// ** WriteLock::WriteLock
template<typename TAsset>
WriteLock<TAsset>::WriteLock( const DataHandle<TAsset>& asset ) : m_asset( asset )
{
    NIMBLE_ABORT_IF( !asset.isValid(), "invalid asset handle" );
}

// ** WriteLock::WriteLock
template<typename TAsset>
WriteLock<TAsset>::~WriteLock( void )
{
    m_asset.assets()->releaseWriteLock( m_asset );
}

// ** WriteLock::operator ->
template<typename TAsset>
TAsset* WriteLock<TAsset>::operator -> ( void )
{
    return &m_asset.template writableData<TAsset>();
}

// ** WriteLock::operator *
template<typename TAsset>
TAsset& WriteLock<TAsset>::operator * ( void )
{
    return m_asset.template writableData<TAsset>();
}
    
} // namespace Assets

DC_END_DREEMCHEST

#endif    /*    !__DC_Assets_AssetHandle_Hpp__    */
