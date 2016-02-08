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

#ifndef __DC_Assets_AssetType_H__
#define __DC_Assets_AssetType_H__

#include "../Dreemchest.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

    //! Asset type id.
    class AssetType {
    public:

        //! Invalid asset type constant.
        static const AssetType          Invalid;


                                        //! Constructs invalid Asset type.
                                        AssetType( void );

                                        //! Copies AssetType instance.
                                        AssetType( const AssetType& other );

        //! Compares two asset types.
        bool                            operator == ( const AssetType& other ) const;

        //! Compares two asset type.
        bool                            operator < ( const AssetType& other ) const;

        //! Returns true if this is a valid asset type.
        bool                            isValid( void ) const;

        //! Converts an asset type to a string.
        String                          toString( void ) const;

        //! Returns asset type bit mask.
        u32                             bit( void ) const;

        //! Returns true if an asset type matches the specified class.
        template<typename TAsset>
        bool                            is( void ) const;

        //! Returns an asset type for specified class.
        template<typename TAsset>
        static AssetType                fromClass( void );

        //! Returns an asset type from a string.
        static AssetType                fromString( const String& value );

        //! Registers a new asset type.
        template<typename TAsset>
        static bool                     declare( void );

    private:

                                        //! Constructs AssetType instance.
                                        AssetType( TypeIdx type );

    private:

        TypeIdx                         m_type;         //!< Actual asset type value.
        static Map<String, AssetType>   s_nameToType;   //!< Maps asset name to type.
        static Map<AssetType, String>   s_typeToName;   //!< Maps asset type to name.
    };

    // ** AssetType::declare
    template<typename TAsset>
    bool AssetType::declare( void )
    {
        AssetType type = fromClass<TAsset>();
        String    name = TypeInfo<TAsset>::name();

        s_nameToType[name] = type;
        s_typeToName[type] = name;

        return true;
    }

    // ** AssetType::fromClass
    template<typename TAsset>
    AssetType AssetType::fromClass( void )
    {
        return GroupedTypeIndex<TAsset, AssetType>::idx();
    }

    // ** Assets::is
    template<typename TAsset>
    bool AssetType::is( void ) const
    {
        return *this == fromClass<TAsset>();
    }

} // namespace Assets

DC_END_DREEMCHEST

#endif    /*    !__DC_Assets_AssetType_H__    */