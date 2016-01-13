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

#ifndef __DC_Scene_Asset_H__
#define __DC_Scene_Asset_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    class Asset : public RefCounted {
    public:
        template<typename T>
        static StrongPtr<Asset> create( const T& ) { return StrongPtr<Asset>(); }
    };

	//! Asset stores info about a single asset file, like it's unique identifier or physical file path.
	class _Asset {
	public:

        //! Alias an asset type.
        typedef TypeIdx         Type;

		//! Returns asset name.
		const String&			name( void ) const;

		//! Returns asset uuid.
		const String&			uuid( void ) const;

        //! Returns an asset file path.
        const String&           filePath( void ) const;

        //! Returns an asset type id.
        template<typename TAsset>
        static Type             assetType( void );

	private:

        String					m_uniqueId;	    //!< This id is used to reference asset.
		String					m_name;			//!< Asset name.
        String                  m_filePath;     //!< Physical file path.
	};

    // ** Asset::assetType
    template<typename TAsset>
    _Asset::Type _Asset::assetType( void )
    {
        return GroupedTypeIndex<TAsset, _Asset>::idx();
    }

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Asset_H__    */