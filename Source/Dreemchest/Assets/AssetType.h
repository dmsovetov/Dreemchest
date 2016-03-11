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

#ifndef __DC_Assets_Type_H__
#define __DC_Assets_Type_H__

#include "Assets.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

    //! Asset type id.
    class Type {
    public:

        //! Invalid asset type constant.
        static const Type           Invalid;


                                    //! Constructs invalid asset type.
                                    Type( void );

                                    //! Copies Type instance.
                                    Type( const Type& other );

        //! Compares two asset types.
        bool                        operator == ( const Type& other ) const;

        //! Compares two asset type.
        bool                        operator < ( const Type& other ) const;

        //! Returns true if this is a valid asset type.
        bool                        isValid( void ) const;

        //! Returns asset type bit mask.
        u32                         bit( void ) const;

        //! Returns true if an asset type matches the specified class.
        template<typename TAsset>
        bool                        is( void ) const;

        //! Returns an asset type for specified class.
        template<typename TAsset>
        static Type                 fromClass( void );

    private:

                                    //! Constructs Type instance.
                                    Type( TypeIdx type );

    private:

        TypeIdx                     m_type;         //!< Actual asset type value.
    };

    // ** Type::fromClass
    template<typename TAsset>
    Type Type::fromClass( void )
    {
        return GroupedTypeIndex<TAsset, Type>::idx();
    }

    // ** Type::is
    template<typename TAsset>
    bool Type::is( void ) const
    {
        return *this == fromClass<TAsset>();
    }

} // namespace Assets

DC_END_DREEMCHEST

#endif    /*    !__DC_Assets_Type_H__    */