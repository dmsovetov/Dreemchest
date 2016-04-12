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

#ifndef __DC_Io_KeyValue_H__
#define __DC_Io_KeyValue_H__

#include "serialization/Serializable.h"

DC_BEGIN_DREEMCHEST

namespace Io {

    //! Default KeyValue binary writer.
    class BinaryVariantStream {
    public:

                    //! Constructs the BinaryVariantStream instance.
                    BinaryVariantStream( StreamPtr stream );

        //! Writes the KeyValue object to a binary stream.
        s32         write( const Variant& value );

        //! Reads the KeyValue object from a binary stream.
        s32         read( Variant& value );

    private:

		//! Key-value value types.
		enum Type {
			  kNull
			, kBoolean
			, kInt8
			, kInt16
			, kInt32
			, kInt64
			, kFloat32
			, kFloat64
			, kString
			, kGuid
            , kVec2
            , kVec3
            , kVec4
            , kQuat
            , kRgb
            , kRgba
			, kArray
			, kObject
		};

        //! Reads the single variant value from a stream.
        Variant     readValue( void );

        //! Writes the single variant value to a stream.
        void        writeValue( const Variant& value );

        //! Returns the variant value type.
        Type        valueType( const Variant& value ) const;

    private:

        StreamPtr   m_stream;   //!< The source binary stream.
    };

	template<>
	inline KeyValue BinarySerializer::read( const Array<u8>& bytes )
	{
		KeyValue data;

		if( bytes.empty() ) {
			return data;
		}

        Variant value;
		BinaryStorage storage( ByteBuffer::createFromData( &bytes[0], bytes.size() ) );
		BinaryVariantStream( storage.isBinaryStorage()->stream() ).read( value );

		return value.as<KeyValue>();
	}

} // namespace Io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_KeyValue_H__	*/