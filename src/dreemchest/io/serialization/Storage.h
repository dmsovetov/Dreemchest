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

#ifndef		__DC_Io_Storage_H__
#define		__DC_Io_Storage_H__

#include	"../Io.h"
#include    "../streams/Stream.h"

#define IoStoreAbstract( T )                                \
    virtual void write( CString key, const T& value ) = 0;  \
    virtual void read ( CString key, T& value ) const = 0;

#define IoStoreImplement( T )                               \
    virtual void write( CString key, const T& value );      \
    virtual void read ( CString key, T& value ) const;

#define IoStoreBinary( T, size )                                                            \
    virtual void write( CString key, const T& value ) { m_stream->write( &value, size ); }  \
    virtual void read ( CString key, T& value ) const { m_stream->read ( &value, size ); }

#define IoStoreInterface( T )                               \
    virtual void write( CString key, const T& value ) {     \
        m_impl->write( key, value );                        \
    }                                                       \
    virtual void read( CString key, T& value ) const {      \
        m_impl->read( key, value );                         \
    }

DC_BEGIN_DREEMCHEST

namespace io {

    //! Storage state info.
    struct StorageState {
        enum State { Unknown, Property, Object, Array, ArrayItem };

                //! Constructs a storage state instance
                StorageState( void )
                    : m_key( NULL ), m_size( 0 ), m_state( Unknown ) {}

                //! Constructs an Object storage state.
                StorageState( CString key )
                    : m_key( key ), m_size( 0 ), m_state( Object ) {}

                //! Constructs an Array storage state instance.
                StorageState( CString key, u32 size )
                    : m_key( key ), m_size( size ), m_state( Array ) {}

                //! Constructs an ArrayItem storage state instance.
                StorageState( u32 index )
                    : m_key( NULL ), m_index( index ), m_size( 0 ), m_state( ArrayItem ) {}

        bool    operator == ( State state ) const { return m_state == state; }

        CString m_key;
        u32     m_index;
        u32     m_size;
        State   m_state;
    };

    //! A data storage interface.
    class IStorage {
    public:

        virtual         ~IStorage( void ) {}

        IoStoreAbstract( bool )
        IoStoreAbstract( u8 )
        IoStoreAbstract( s8 )
        IoStoreAbstract( u16 )
        IoStoreAbstract( s16 )
        IoStoreAbstract( u32 )
        IoStoreAbstract( s32 )
        IoStoreAbstract( u64 )
        IoStoreAbstract( s64 )
        IoStoreAbstract( f32 )
        IoStoreAbstract( f64 )
        IoStoreAbstract( String )

        //! Pushes a write state.
        virtual void            pushWrite( const StorageState& state ) = 0;

        //! Pushes a read state.
        virtual void            pushRead( StorageState& state ) const = 0;

        //! Pops a state.
        virtual StorageState    pop( void ) const = 0;
    };

    //! A data storage into which the data is serialized.
    class Storage {
    friend class Serializable;
    public:

                        //! Constructs a Storage instance.
                        Storage( StorageType type, const StreamPtr& stream );
        virtual         ~Storage( void );

                        //! Returns true if this storage is valid.
                        operator bool( void ) const;

        IoStoreInterface( bool )
        IoStoreInterface( u8 )
        IoStoreInterface( s8 )
        IoStoreInterface( u16 )
        IoStoreInterface( s16 )
        IoStoreInterface( u32 )
        IoStoreInterface( s32 )
        IoStoreInterface( u64 )
        IoStoreInterface( s64 )
        IoStoreInterface( f32 )
        IoStoreInterface( f64 )
        IoStoreInterface( String )

        IoStoreImplement( Serializable )

        template<typename T>
        void            write( CString key, const Array<T>& array );

        template<typename T>
        void            read( CString key, Array<T>& array ) const;

        //! Pushes a write state.
        virtual void            pushWrite( const StorageState& state );

        //! Pushes a read state.
        virtual void            pushRead( StorageState& state ) const;

        //! Pops a state.
        virtual StorageState    pop( void ) const;

    private:

        IStorage*       m_impl;
    };

    namespace detail {

        template<typename T>
        FieldSerializerPtr createFieldSerializer( CString name, const T& field );

        template<typename T>
        FieldSerializerPtr createFieldSerializer( CString name, const Array<T>& field );

    } // namespace detail

    // ** Storage::write
    template<typename T>
    void Storage::write( CString key, const Array<T>& array )
    {
        pushWrite( StorageState( key, ( u32 )array.size() ) );

        for( u32 i = 0; i < ( u32 )array.size(); i++ ) {
            pushWrite( StorageState( i ) );
            detail::createFieldSerializer( NULL, array[i] )->write( *this );
        }

        pop();
    }

    // ** Storage::read
    template<typename T>
    void Storage::read( CString key, Array<T>& array ) const
    {
        StorageState state( key, 0 );

        pushRead( state );

        array.resize( state.m_size );
        for( u32 i = 0; i < ( u32 )array.size(); i++ ) {
            StorageState state( i );
            pushRead( state );
            detail::createFieldSerializer( NULL, array[i] )->read( *this );
        }

        pop();
    }
	/*
	//! A generic value wrapper.
	class Value {
	public:
	
		//! Casts value to a specified type.
		template<typename T>
		T					as( void ) const;
	};
	
	//! BinaryStorage class used for writing data in binary format.
	class BinaryStorage {
	public:
	
		virtual				~BinaryStorage( void ) {}
		
		//! Writes the data to a binary storage.
		virtual void		write( const void* ptr, u32 size )
		{
			m_stream->write( ptr, size );
		}
		
		//! Reads the data from binary storage.
		virtual void		read( void* ptr, u32 size )
		{
			m_stream->read( ptr, size );
		}
		
	protected:
	
		//! Target stream.
		StreamPtr			m_stream;
	};

	//!	KeyValueStorage class is used for pairs of key & value (JSON for example).
	class KeyValueStorage {
	public:
	
		virtual				~KeyValueStorage( void ) {}
		
		//! Begins writing of an array.
		virtual void		pushArray( const String& key );
		
		//! Ends writing of an array.
		virtual void		popArray( void );
		
			//! Begins writing of an array.
		virtual void		pushObject( const String& key );
		
		//! Ends writing of an array.
		virtual void		popObject( void );

		//! Writes value to a key-value storage.
		virtual void		write( const String& key, const Value& value );
		
		//! Reads value from a key-value storage.
		virtual void		read( const String& key, Value& value );
		
		//! Writes value to an array.
		virtual void		write( s32 index, const Value& value );
		
		//! Reads value from an array.
		virtual void		read( s32 index, Value& value );
	};
	
	namespace detail {
	
		//! Base class for field serialization.
		class Field {
		public:
		
			virtual			~Field( void ) {}
			
			//! Writes field to binary storage.
			virtual void	write( const void* ptr, BinaryStorage& storage ) = 0;
			
			//! Reads field from a binary storage.
			virtual void	read( void* ptr, BinaryStorage& storage ) = 0;
			
			//! Writes field to key-value storage.
			virtual void	write( const void* ptr, KeyValueStorage& storage ) = 0;
			
			//! Reads field from a key-value storage.
			virtual void	read( void* ptr, KeyValueStorage& storage ) = 0;
		};
		
		//! A serializable field of a POD type.
		template<typename T>
		class PodField : public Field {
		public:
		
						PodField( const String& name )
							: m_name( name ) {}
		
			//! Writes field to binary storage.
			virtual void write( const void* ptr, BinaryStorage& storage )
			{
				storage.write( ptr, sizeof( T ) );
			}
			
			//! Reads field from binary storage.
			virtual void read( void* ptr, BinaryStorage& storage )
			{
				storage.read( ptr, sizeof( T ) );
			}
			
			//! Writes field to key-value storage.
			virtual void write( const void* ptr, KeyValueStorage& storage )
			{
			//	storage.write( m_name, *static_cast<T>( ptr ) );
			}
			
			//! Reads field from key-value storage.
			virtual void read( void* ptr, KeyValueStorage& storage )
			{
			//	Value value;
			//	storage.read( m_name, value );
			//	*static_cast<T>( ptr ) = value.as<T>();
			}
			
		private:
		
			//! Field name.
			String		m_name;
		};
		
		//! Array field.
		template<T>
		class CollectionField : public Field {
		public:
		
						ArrayField( const String& name )
							: m_name( name ) {}
							
			//! Writes field to binary storage.
			virtual void write( const void* ptr, BinaryStorage& storage )
			{
				const T* collection = static_cast<const T*>( ptr );
				Field*	 field		= createField<T>();
				u32		 size		= collection->size();
				
				storage.write( &size, 4 );
				for( T::const_iterator i = collection->begin(), end = collection->end(); i != end; ++i ) {
					field->write( &(*i), storage );
				}
			}
			
			//! Reads field from binary storage.
			virtual void read( void* ptr, BinaryStorage& storage )
			{
				T* 		collection = static_cast<T*>( ptr );
				Field*	field		= createField<T>();
				u32		size		= 0;
				
				storage.read( &size, 4 );
				for( T::const_iterator i = collection->begin(), end = collection->end(); i != end; ++i ) {
					field->read( &(*i), storage );
				}
			}
			
			//! Writes field to key-value storage.
			virtual void write( const void* ptr, KeyValueStorage& storage )
			{
			//	const T* collection = static_cast<const T*>( ptr );
			//	Field*	 field		= createField<T>();
			//	u32		 size		= collection->size();
			//	s32		 index		= 0;
				
			//	storage.pushArray( m_name );
			//	for( T::const_iterator i = collection->begin(), end = collection->end(); i != end; ++i ) {
			//		field->write( index++, *i );
			//	}
			//	storage.popArray();
			}
			
			//! Reads field from key-value storage.
			virtual void read( void* ptr, KeyValueStorage& storage )
			{
			//	Value value;
			//	storage.read( m_name, value );
			//	*static_cast<T>( ptr ) = value.as<T>();
			}
		};
		
		//! Creates field
		template<typename T>
		Field* createField( const String& name = String(), const T* value = NULL )
		{
			return NULL;
		}
		
		//! Creates field of type.
		template<>
		Field* createField( const String& name = String(), const Array<T>* value = NULL )
		{
			return new CollectionField< Array<T> >( name );
		}
		
		//! Creates field of type.
		template<>
		Field* createField( const String& name = String(), const List<T>* value = NULL )
		{
			return new CollectionField< List<T> >( name );
		}
	
	} // namespace detail
	*/

} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Storage_H__	*/