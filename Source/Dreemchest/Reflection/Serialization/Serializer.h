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

#ifndef __DC_Reflection_Serializer_H__
#define __DC_Reflection_Serializer_H__

#include "../Reflection.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

    //! Uses embedded meta-object instance to serialize object properties to a key-value storage.
    class Serializer {
    public:

		//! Function type used by custom type serializers.
		typedef cClosure<Variant(const Class&, const Property&, const Variant&)> TypeConverter;

		//! Function type used by property default value accessor.
		typedef cClosure<Variant(const KeyValue&)> PropertyDefault;

        virtual                 ~Serializer( void ) {}

        //! Writes an object instance to a key-value storage.
        virtual bool            serialize( InstanceConst instance, KeyValue& ar ) const;

        //! Reads an object instance from a key-value storage.
        virtual Instance        deserialize( AssemblyWPtr assembly, const KeyValue& ar );

        //! Reads instance properties from a key-value storage.
        void                    deserialize( const Instance& instance, const KeyValue& ar ) const;

		//! Registers a type converter.
		template<typename TFrom, typename TTo>
		void					registerTypeConverter( const TypeConverter& callback );

		//! Registers a property default value accessor.
		template<typename TType>
		void					registerPropertyDefault( const String& name, const PropertyDefault& callback );

    protected:

        //! Reads instance properties from a key-value storage.
        Instance                createAndDeserialize( AssemblyWPtr assembly, const String& name, const KeyValue& ar ) const;

		//! Reads a property value from an archive.
		Variant					readPropertyValue( const Class* cls, const Property* property, const KeyValue& ar ) const;

        //! Returns a value converter.
        TypeConverter           findTypeConverter( const Type* from, const Type* to ) const;

		//! Calculates a property reader hash value.
		String64				calculatePropertyReaderHash( const Class* cls, CString name ) const;

        //! Calculates a type converter hash.
        u64                     calculateTypeConverterHash( const Type* from, const Type* to ) const;

        //! Deserializes a list property value.
        void                    deserializeList( const Class& cls, const Property& property, const Instance& instance, const Variant& value, Reflection::ListIterator& iterator ) const;

        //! Deserializes a map property value.
        void                    deserializeMap( const Class& cls, const Property& property, const Instance& instance, const Variant& value, Reflection::MapIterator& iterator ) const;

        //! Deserializes a primitive value.
        void                    deserializeValue( const Class& cls, const Property& property, const Instance& instance, const Variant& value ) const;

        //! Serializes a list property value.
        void                    serializeList( const Class& cls, const Property& property, const InstanceConst& instance, const Reflection::ListIterator& iterator, KeyValue& ar ) const;

        //! Serializes a map property value.
        void                    serializeMap( const Class& cls, const Property& property, const InstanceConst& instance, const Reflection::MapIterator& iterator, KeyValue& ar ) const;

        //! Serializes a primitive value.
        void                    serializeValue( const Class& cls, const Property& property, const InstanceConst& instance, KeyValue& ar ) const;

	private:

		//! Container type to store type conversions.
		typedef HashMap<u64, TypeConverter> TypeConverters;

		//! Container type to store property serializers/deserializers.
		typedef HashMap<String64, PropertyDefault, String64Hasher> PropertyDefaults;

		TypeConverters			m_typeConverters;	    //!< Custom type converters.
		PropertyDefaults		m_defaults;				//!< Property default value callbacks.
    };

	// ** Serializer::registerTypeConverter
	template<typename TFrom, typename TTo>
	void Serializer::registerTypeConverter( const TypeConverter& callback )
	{
		u64 hash = calculateTypeConverterHash( Type::fromClass<TFrom>(), Type::fromClass<TTo>() );
		m_typeConverters[hash] = callback;
	}

	// ** Serializer::registerPropertyDefault
	template<typename TType>
	void Serializer::registerPropertyDefault( const String& name, const PropertyDefault& callback )
	{
		String64 hash = calculatePropertyReaderHash( TType::staticMetaObject(), name.c_str() );
		m_defaults[hash] = callback;
	}

} // namespace Reflection

DC_END_DREEMCHEST

#endif    /*    !__DC_Reflection_Serializer_H__    */