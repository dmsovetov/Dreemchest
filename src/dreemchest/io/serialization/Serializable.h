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

#ifndef		__DC_Io_Serializable_H__
#define		__DC_Io_Serializable_H__

#include	"../Io.h"

//! Macro definition for serializer fields declaration
#define IoBeginSerializer                                                   \
    virtual io::detail::FieldSerializers fieldSerializers( void ) const {   \
        io::detail::FieldSerializers result;

//! Macro definition for adding a serializable field.
#define IoField( field )                            \
    result.push_back( io::detail::createFieldSerializer( #field, field ) );

//! Macro definition for adding a serializable array.
#define IoArray( field )                            \
    result.push_back( io::detail::createFieldSerializer( #field, field ) );

//! Macro definition for serializer fields declaration
#define IoEndSerializer                             \
        return result;                              \
    }

//! Macro definition to declare a type info
#define IoTypeInfo( T, _Type, _Size ) template<> struct TypeInfo<T> { enum { Type = _Type }; enum { Size = _Size }; };

//! Macro definition for overriding field serializers
#define IoBeginFieldSerializer( T )                                                 \
    class T##FieldSerializer : public io::detail::FieldSerializer {                 \
    public:                                                                         \
                    T##FieldSerializer( CString name, const T& field )              \
                        : m_name( name ), m_pointer( const_cast<T*>( &field ) ) {}  \
        CString     m_name;                                                         \
        T*          m_pointer;

#define IoWriteField( ... )                                             \
    virtual void write( Storage* storage ) const {                      \
        __VA_ARGS__                                                     \
    }

#define IoReadField( ... )                                              \
    virtual void read( const Storage* storage ) {                       \
        __VA_ARGS__                                                     \
    }

#define IoEndFieldSerializer( T )                                                                           \
    };                                                                                                      \
    template<>                                                                                              \
    io::detail::FieldSerializerPtr io::detail::createFieldSerializer<T>( CString name, const T& field ) {   \
        return io::detail::FieldSerializerPtr( new T##FieldSerializer( name, field ) );                     \
    }

DC_BEGIN_DREEMCHEST

namespace io {

    //! Primitive field data types.
    enum PrimitiveType {
        TypeUnknown,
        TypeBool,
        TypeNumber,
        TypeString,
        TypeArray,
        TypeObject
    };

    //! Primitive type info.
    template<typename T>
    struct TypeInfo {
        enum { Type = TypeUnknown };
        enum { Size = 0 };
    };

    IoTypeInfo( bool,   TypeBool,   1 )
    IoTypeInfo( u8,     TypeNumber, 1 )
    IoTypeInfo( s8,     TypeNumber, 1 )
    IoTypeInfo( u16,    TypeNumber, 2 )
    IoTypeInfo( s16,    TypeNumber, 2 )
    IoTypeInfo( u32,    TypeNumber, 4 )
    IoTypeInfo( s32,    TypeNumber, 4 )
    IoTypeInfo( u64,    TypeNumber, 8 )
    IoTypeInfo( s64,    TypeNumber, 8 )
    IoTypeInfo( f32,    TypeNumber, 4 )
    IoTypeInfo( f64,    TypeNumber, 8 )
    IoTypeInfo( String, TypeString, 8 )

    namespace detail {

        //! Field serializer
        class FieldSerializer : public RefCounted {
        public:

            virtual         ~FieldSerializer( void ) {}

            //! Writes field data to a storage.
            virtual void    write( Storage* storage ) const = 0;

            //! Reads field data from a storage.
            virtual void    read( const Storage* storage )  = 0;
        };

        //! Serializable field info
        template<typename T>
        class TypedFieldSerializer : public FieldSerializer {
        public:

                            //! Constructs a TypedFieldSerializer instance.
                            TypedFieldSerializer( CString name, const T& field )
                                : m_name( name ), m_pointer( const_cast<T*>( &field ) ) {}

            //! Writes field data to a storage.
            virtual void    write( Storage* storage ) const;

            //! Reads field data from a storage.
            virtual void    read( const Storage* storage );

        protected:

            CString         m_name;     //!< Field name.
            T*              m_pointer;  //!< Field data pointer.
        };

        // ** TypedFieldSerializer::write
        template<typename T>
        void TypedFieldSerializer<T>::write( Storage* storage ) const
        {
            storage->write( m_name, *m_pointer );
        }

        // ** TypedFieldSerializer::read
        template<typename T>
        void TypedFieldSerializer<T>::read( const Storage* storage )
        {
            storage->read( m_name, *m_pointer );
        }

        //! Serializable array info
        template<typename T>
        class TypedArraySerializer : public FieldSerializer {
        public:

                            //! Constructs a TypedArraySerializer instance.
                            TypedArraySerializer( CString name, const Array<T>& field )
                                : m_name( name ), m_pointer( const_cast<Array<T>*>( &field ) ) {}

            //! Writes field data to a storage.
            virtual void    write( Storage* storage ) const;

            //! Reads field data from a storage.
            virtual void    read( const Storage* storage );

        private:

            CString         m_name;     //!< Field name.
            Array<T>*       m_pointer;  //!< Field data pointer.
        };

        // ** TypedArraySerializer::write
        template<typename T>
        void TypedArraySerializer<T>::write( Storage* storage ) const
        {
            storage->write( m_name, *m_pointer );
        }

        // ** TypedArraySerializer::read
        template<typename T>
        void TypedArraySerializer<T>::read( const Storage* storage )
        {
            storage->read( m_name, *m_pointer );
        }

        //! Allocates a TypedFieldSerializer instance.
        template<typename T>
        FieldSerializerPtr createFieldSerializer( CString name, const T& field )
        {
            return FieldSerializerPtr( new TypedFieldSerializer<T>( name, field ) );
        }

        template<typename T>
        FieldSerializerPtr createFieldSerializer( CString name, const Array<T>& field )
        {
            return FieldSerializerPtr( new TypedArraySerializer<T>( name, field ) );
        }

    } // namespace detail

    //! Base class for all serializable data structs.
    class Serializable {

        ClassEnableTypeInfo( Serializable )

    public:

        //! Reads data from a storage.
        void                                read( const Storage* storage );

        //! Writes data to a storage.
        void                                write( Storage* storage ) const;

    protected:

        //! Returns an array of field serializers.
        virtual detail::FieldSerializers    fieldSerializers( void ) const;
    };

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Serializable_H__	*/