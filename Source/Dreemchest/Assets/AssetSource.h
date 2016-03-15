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

#ifndef __DC_Assets_AssetSource_H__
#define __DC_Assets_AssetSource_H__

#include "Assets.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

    //! Base class for all asset sources.
    class AbstractSource {
    public:

        virtual         ~AbstractSource( void ) {}

        //! Loads data to a specified asset data handle.
        virtual bool    construct( Assets& assets, Handle asset ) = 0;

        //! Returns the last modification timestamp of an asset source.
        virtual u32     lastModified( void ) const = 0;
    };

    //! This is a dummy asset source used for runtime created assets.
    class NullSource : public AbstractSource {
    protected:

        //! Does nothing with an asset
        virtual bool    construct( Assets& assets, Handle asset );

        //! Returns a zero timestamp.
        virtual u32     lastModified( void ) const;
    };

    //! Asset generator source used for generating assets in a runtime.
    template<typename TAsset>
    class GeneratorSource : public AbstractSource {
    public:

        //! Performs the type-cast of an asset handle and dispatches it to a protected abstract method.
        virtual bool    construct( Assets& assets, Handle asset ) DC_DECL_OVERRIDE;

        //! Returns the zero timestamp.
        virtual u32     lastModified( void ) const DC_DECL_OVERRIDE;

    protected:

        //! This method should be overridden in a subclass to generate an asset.
        virtual bool    generate( Assets& assets, TAsset& asset ) = 0;
    };

    // ** GeneratorSource::construct
    template<typename TAsset>
    bool GeneratorSource<TAsset>::construct( Assets& assets, Handle asset )
    {
        bool result = generate( assets, *asset.writeLock<TAsset>() );
        return result;
    }

    // ** GeneratorSource::lastModified
    template<typename TAsset>
    u32 GeneratorSource<TAsset>::lastModified( void ) const
    {
        return 0;
    }

    //! Asset file source used for loading assets from files.
    class AbstractFileSource : public AbstractSource {
    public:

                        //! Constructs AbstractFileSource instance.
                        AbstractFileSource( void );

        //! Opens the file stream and loads data from it.
        virtual bool    construct( Assets& assets, Handle asset ) DC_DECL_OVERRIDE;

        //! Returns the last file modification time stamp.
        virtual u32     lastModified( void ) const DC_DECL_OVERRIDE;

        //! Sets the last file modification timestamp.
        void            setLastModified( u32 value );

        //! Returns the source asset file name.
        const String&   fileName( void ) const;

        //! Sets the source asset file name.
        void            setFileName( const String& value );

    protected:

        //! This virtual method is used to dispatch the loading process to actual asset loading implementation.
        virtual bool    constructFromStream( Io::StreamPtr stream, Assets& assets, Handle asset ) = 0;

    private:

        String          m_fileName;     //!< Source file name to load asset from.
        u32             m_lastModified; //!< Timestamp when the file was modified last time.
    };

    //! Generic base class for all asset file sources.
    template<typename TAsset>
    class FileSource : public AbstractFileSource {
    protected:

        //! Type casts an asset handle and dispatches the loading process to an implementation.
        virtual bool    constructFromStream( Io::StreamPtr stream, Assets& assets, Handle asset ) DC_DECL_OVERRIDE;

        //! Performs an asset data parsing from a stream.
        virtual bool    constructFromStream( Io::StreamPtr stream, Assets& assets, TAsset& asset ) = 0;
    };

    // ** FileSource::parseFromStream
    template<typename TAsset>
    bool FileSource<TAsset>::constructFromStream( Io::StreamPtr stream, Assets& assets, Handle asset )
    {
        bool result = constructFromStream( stream, assets, *asset.writeLock<TAsset>() );
        return result;
    }

    //! Generic base class for all asset sources.
    template<typename TAsset, typename TSource>
    class AssetSource : public AbstractSource {
    public:

        //! Alias an asset handle type.
        typedef GenericHandle<TSource> AssetHandle;


                        //! Constructs an AssetSource instance.
                        AssetSource( AssetHandle asset );

        //! Loads data from an asset.
        virtual bool    construct( Assets& assets, Handle asset ) DC_DECL_OVERRIDE;

        //! Returns the last asset modification time stamp.
        virtual u32     lastModified( void ) const DC_DECL_OVERRIDE;

    protected:

        //! Performs an asset data construction from an asset.
        virtual bool    constructFromAsset( const TSource& source, Assets& assets, TAsset& asset ) = 0;

    protected:

        AssetHandle     m_asset;
    };

    // ** AssetSource::AssetSource
    template<typename TAsset, typename TSource>
    AssetSource<TAsset, TSource>::AssetSource( AssetHandle asset )
        : m_asset( asset )
    {
    
    }

    // ** AssetSource::construct
    template<typename TAsset, typename TSource>
    bool AssetSource<TAsset, TSource>::construct( Assets& assets, Handle asset )
    {
        bool result = constructFromAsset( m_asset.readLock(), assets, *asset.writeLock<TAsset>() );
        return result;
    }

    // ** AssetSource::lastModified
    template<typename TAsset, typename TSource>
    u32 AssetSource<TAsset, TSource>::lastModified( void ) const
    {
        return m_asset.asset().lastModified();
    }

} // namespace Assets

DC_END_DREEMCHEST

#endif    /*    !__DC_Assets_AssetSource_H__    */