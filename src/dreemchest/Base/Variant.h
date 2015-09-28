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

#ifndef __Base_Variant_H__
#define __Base_Variant_H__

DC_BEGIN_DREEMCHEST

	namespace impl {
		typedef StrongPtr<class Value> ValuePtr;
	} // namespace impl

	//! Array of variants type.
	typedef Array<class Variant> VariantArray;

	//! Variant registry pointer type.
	typedef StrongPtr<class VariantRegistry> VariantRegistryPtr;

	//! The Variant type act like a union for most common data types.
	class Variant {
	public:

		//! Variant data type.
		enum Type {
			UnknownValue,	//!< Unknown value type is stored inside this variant.
			BooleanValue,	//!< Boolean value is stored inside this variant.
			NumberValue,	//!< Numeric value is stored inside this variant.
			StringValue,	//!< String value is stored inside this variant.
			ArrayValue,		//!< Array of values is stored inside this variant.
			GuidValue,		//!< Guid value is stored inside this variant.
			RegistryValue,	//!< Variant registry is stored inside this variant.
		};

							//! Constructs void Variant.
							Variant( void ) {}

							//! Constructs a Variant from boolean value.
							Variant( bool value );

							//! Constructs a Variant from integer value.
							Variant( u8 value );
							Variant( s8 value );
							Variant( u16 value );
							Variant( s16 value );
							Variant( u32 value );
							Variant( s32 value );

							//! Constructs a Variant from float value.
							Variant( f32 value );

							//! Constructs a Variant from double value.
							Variant( f64 value );

							//! Constructs a Variant from string value.
							Variant( const String& value );
							Variant( CString value );

							//! Constructs a Variant from GUID value.
							Variant( const Guid& value );

							//! Constructs a Variant from a registry value.
							Variant( const VariantRegistryPtr& value );

							//! Constructs a Variant from an array of values.
							template<typename T>
							Variant( const Array<T>& value );

							//! Constructs a Variant with a specified type.
							Variant( Type type );

		//! Returns true if this value is valid.
		bool				isValid( void ) const;

		//! Returns the value type.
		Type				type( void ) const;

		//! Converts value to boolean.
		bool				toBool( void ) const;

		//! Converts value to integer.
		s32					toInt( void ) const;

		//! Converts value to float.
		f32					toFloat( void ) const;

		//! Converts value to double.
		f64					toDouble( void ) const;

		//! Converts value to string.
		String				toString( void ) const;
		CString				toCString( void ) const;

		//! Converts value to array.
		const VariantArray&	toArray( void ) const;
		VariantArray&		toArray( void );

		//! Converts value to GUID.
		const Guid&			toGuid( void ) const;

		//! Converts value to a registry.
		VariantRegistryPtr	toRegistry( void ) const;

		//! Casts a value to a specified type.
		template<typename T> const T& as( const T& defaultValue = T() ) const;

		//! Returns a type name by it's id.
		CString				typeName( void ) const;

	private:

		//! Actual variant value.
		impl::ValuePtr		m_value;
	};

	//! VariantRegistry is used to store variant values in a key-value structure.
	class VariantRegistry : public RefCounted {
	public:

		//! A container type to store object properties.
		typedef Map<String, Variant> Properties;

		//! Returns a reference to object property with a specified key.
		const Variant&		operator[]( CString key ) const;
		Variant&			operator[]( CString key );

		//! Returns a registry properties.
		const Properties&	properties( void ) const;

	private:

		//! Actual object properties.
		Properties			m_properties;
	};

	namespace impl {

		//! A generic value holder.
		class Value : public RefCounted {
		public:

											ClassEnableTypeInfo( Value )

			virtual							~Value( void ) {}

			//! Returns the value type.
			virtual Variant::Type			type( void ) const { return Variant::UnknownValue; }

			//! Converts value to boolean.
			virtual bool					toBool( void ) const { return false; }

			//! Converts value to integer.
			virtual s32						toInt( void ) const { return 0; }

			//! Converts value to float.
			virtual f32						toFloat( void ) const { return 0.0f; }

			//! Converts value to double.
			virtual f64						toDouble( void ) const { return 0.0; }

			//! Converts value to string.
			virtual String					toString( void ) const { return ""; }

			//! Converts value to GUID.
			virtual const Guid&				toGuid( void ) const { static Guid empty; return empty; }

			//! Converts value to array.
			virtual const VariantArray&		toArray( void ) const { static VariantArray empty; return empty; }

			//! Converts value to registry.
			virtual VariantRegistryPtr		toRegistry( void ) const { return VariantRegistryPtr(); }
		};

		//! A base class for all value holder implementations.
		template<typename T>
		class ValueType : public Value {
		public:

									ClassEnableTypeInfo( ValueType )

									//! Constructs a value holder.
									ValueType( const T& value, Variant::Type type )
										: m_type( type ), m_value( value ) {}

			//! Returns the value type.
			virtual Variant::Type	type( void ) const { return m_type; }

			//! Returns a stored value.
			const T&				get( void ) const { return m_value; }

			//! Sets a new value.
			void					set( const T& value ) { m_value = value; }

		protected:

			//! Value type.
			Variant::Type			m_type;

			//! Actual value holder.
			T						m_value;
		};

		//! Bolean value holder
		class BooleanValue : public ValueType<bool> {
		public:

								//! Constructs a boolean value.
								BooleanValue( bool value )
									: ValueType( value, Variant::BooleanValue ) {}

			//! Converts value to boolean.
			virtual bool		toBool( void ) const { return m_value; }

			//! Converts a boolean value to integer
			virtual s32			toInt( void ) const { return m_value ? 1 : 0; }

			//! Converts a boolean value to float
			virtual f32			toFloat( void ) const { return m_value ? 1.0f : 0.0f; }

			//! Converts a boolean value to double
			virtual f64			toDouble( void ) const { return m_value ? 1.0 : 0.0; }

			//! Converts a boolean value to integer
			virtual String		toString( void ) const { return m_value ? "true" : "false"; }
		};

		//! Numeric value holder.
		class NumericValue : public ValueType<f64> {
		public:

								//! Constructs numeric value.
								NumericValue( f64 value )
									: ValueType( value, Variant::NumberValue ) {}

			//! Converts value to boolean.
			virtual bool		toBool( void ) const { return m_value != 0.0 ? true : false; }

			//! Converts a boolean value to integer
			virtual s32			toInt( void ) const { return static_cast<s32>( m_value ); }

			//! Converts a boolean value to float
			virtual f32			toFloat( void ) const { return static_cast<f32>( m_value ); }

			//! Converts a boolean value to double
			virtual f64			toDouble( void ) const { return m_value; }

			//! Converts a boolean value to integer
			virtual String		toString( void ) const;
		};

		// ** NumericValue::toString
		inline String NumericValue::toString( void ) const
		{
			s8 result[32];
			sprintf( result, "%f", m_value );
			return result;
		}

		//! GUID value holder.
		class GuidValue : public ValueType<Guid> {
		public:

								//! Constructs GUID value.
								GuidValue( const Guid& value )
									: ValueType( value, Variant::GuidValue ) {}

			//! Converts a GUID value to string
			virtual String		toString( void ) const { return m_value.toString(); }

			//! Returns a GUID value.
			virtual const Guid&	toGuid( void ) const { return m_value; }
		};

		//! String value holder.
		class StringValue : public ValueType<String> {
		public:

								//! Constructs string value.
								StringValue( const String& value )
									: ValueType( value, Variant::StringValue ) {}

			//! Converts value to boolean.
			virtual bool		toBool( void ) const;

			//! Converts a boolean value to integer
			virtual s32			toInt( void ) const;

			//! Converts a boolean value to float
			virtual f32			toFloat( void ) const;

			//! Converts a boolean value to double
			virtual f64			toDouble( void ) const;

			//! Converts a boolean value to string
			virtual String		toString( void ) const { return m_value; }
		};

		// ** StringValue::toBool
		inline bool StringValue::toBool( void ) const
		{
			return m_value == "true" ? true : false;
		}

		// ** StringValue::toInt
		inline s32 StringValue::toInt( void ) const
		{
			return atoi( m_value.c_str() );
		}

		// ** StringValue::toFloat
		inline f32 StringValue::toFloat( void ) const
		{
			return static_cast<f32>( atof( m_value.c_str() ) );
		}

		// ** StringValue::toDouble
		inline f64 StringValue::toDouble( void ) const
		{
			return atof( m_value.c_str() );
		}

		//! Array value holder.
		class ArrayValue : public ValueType<VariantArray> {
		public:

								//! Constructs array value.
								ArrayValue( const VariantArray& value )
									: ValueType( value, Variant::ArrayValue ) {}

			//! Converts value to array.
			virtual const VariantArray&	toArray( void ) const { return m_value; }
		};

		//! Variant registry holder.
		class RegistryValue : public ValueType<VariantRegistryPtr> {
		public:

									//! Constructs registry value.
									RegistryValue( const VariantRegistryPtr& value )
										: ValueType( value, Variant::RegistryValue ) {}

			//! Converts value to a registry.
			virtual VariantRegistryPtr	toRegistry( void ) const { return m_value; }
		};

	} // namespace impl

	// ** Variant::Variant
	inline Variant::Variant( bool value )
	{
		m_value = impl::ValuePtr( DC_NEW impl::BooleanValue( value ) );
	}

	// ** Variant::Variant
	inline Variant::Variant( u8 value )
	{
		m_value = impl::ValuePtr( DC_NEW impl::NumericValue( value ) );
	}

	// ** Variant::Variant
	inline Variant::Variant( s8 value )
	{
		m_value = impl::ValuePtr( DC_NEW impl::NumericValue( value ) );
	}

	// ** Variant::Variant
	inline Variant::Variant( u16 value )
	{
		m_value = impl::ValuePtr( DC_NEW impl::NumericValue( value ) );
	}

	// ** Variant::Variant
	inline Variant::Variant( s16 value )
	{
		m_value = impl::ValuePtr( DC_NEW impl::NumericValue( value ) );
	}

	// ** Variant::Variant
	inline Variant::Variant( u32 value )
	{
		m_value = impl::ValuePtr( DC_NEW impl::NumericValue( value ) );
	}

	// ** Variant::Variant
	inline Variant::Variant( s32 value )
	{
		m_value = impl::ValuePtr( DC_NEW impl::NumericValue( value ) );
	}

	// ** Variant::Variant
	inline Variant::Variant( f32 value )
	{
		m_value = impl::ValuePtr( DC_NEW impl::NumericValue( value ) );
	}

	// ** Variant::Variant
	inline Variant::Variant( f64 value )
	{
		m_value = impl::ValuePtr( DC_NEW impl::NumericValue( value ) );
	}

	// ** Variant::Variant
	inline Variant::Variant( const String& value )
	{
		m_value = impl::ValuePtr( DC_NEW impl::StringValue( value ) );
	}

	// ** Variant::Variant
	inline Variant::Variant( CString value )
	{
		m_value = impl::ValuePtr( DC_NEW impl::StringValue( value ) );
	}

	// ** Variant::Variant
	inline Variant::Variant( const Guid& value )
	{
		m_value = impl::ValuePtr( DC_NEW impl::GuidValue( value ) );
	}

	// ** Variant::Variant
	template<typename T>
	inline Variant::Variant( const Array<T>& value )
	{
		VariantArray array;

		for( typename Array<T>::const_iterator i = value.begin(), end = value.end(); i != end; ++i ) {
			array.push_back( Variant( *i ) );
		}

		m_value = impl::ValuePtr( DC_NEW impl::ArrayValue( array ) );
	}

	// ** Variant::Variant
	inline Variant::Variant( Type type )
	{
		switch( type ) {
		case UnknownValue:	m_value = impl::ValuePtr();
							break;
		case BooleanValue:	m_value = impl::ValuePtr( DC_NEW impl::BooleanValue( false ) );
							break;
		case NumberValue:	m_value = impl::ValuePtr( DC_NEW impl::NumericValue( 0.0f ) );
							break;
		case StringValue:	m_value = impl::ValuePtr( DC_NEW impl::StringValue( "" ) );
							break;
		case ArrayValue:	m_value = impl::ValuePtr( DC_NEW impl::ArrayValue( VariantArray() ) );
							break;
		case GuidValue:		m_value = impl::ValuePtr( DC_NEW impl::GuidValue( Guid() ) );
							break;
		case RegistryValue: m_value = impl::ValuePtr( DC_NEW impl::RegistryValue( DC_NEW VariantRegistry ) );
							break;
		}
	}

	// ** Variant::as
	template<typename T>
	inline const T& Variant::as( const T& defaultValue ) const
	{
		::impl::ValueType<T>* casted = castTo< ::impl::ValueType<T> >( m_value.get() );
		return casted ? casted->get() : defaultValue;
	}
	
	// ** Variant::isValid
	inline bool Variant::isValid( void ) const
	{
		return m_value != ::impl::ValuePtr();
	}

	// ** Variant::type
	inline Variant::Type Variant::type( void ) const
	{
		return isValid() ? m_value->type() : UnknownValue;
	}

	// ** Variant::toBool
	inline bool Variant::toBool( void ) const
	{
		return isValid() ? m_value->toBool() : false;
	}

	// ** Variant::toInt
	inline s32 Variant::toInt( void ) const
	{
		return isValid() ? m_value->toInt() : 0;
	}

	// ** Variant::toFloat
	inline f32 Variant::toFloat( void ) const
	{
		return isValid() ? m_value->toFloat() : 0.0f;
	}

	// ** Variant::toDouble
	inline f64 Variant::toDouble( void ) const
	{
		return isValid() ? m_value->toDouble() : 0.0;
	}

	// ** Variant::toString
	inline String Variant::toString( void ) const
	{
		return isValid() ? m_value->toString() : "";
	}

	// ** Variant::toString
	inline CString Variant::toCString( void ) const
	{
		return isValid() ? m_value->toString().c_str() : "";
	}

	// ** Variant::toGuid
	inline const Guid& Variant::toGuid( void ) const
	{
		static Guid empty;
		return isValid() ? m_value->toGuid() : empty;
	}

	// ** Variant::toArray
	inline const VariantArray& Variant::toArray( void ) const
	{
		static VariantArray empty;
		return isValid() ? m_value->toArray() : empty;
	}

	// ** Variant::toArray
	inline VariantArray& Variant::toArray( void )
	{
		DC_BREAK_IF( !isValid() );
		return const_cast<VariantArray&>( m_value->toArray() );
	}

	// ** Variant::toRegistry
	inline VariantRegistryPtr Variant::toRegistry( void ) const
	{
		return isValid() ? m_value->toRegistry() : VariantRegistryPtr();
	}

	// ** Variant::typeName
	inline CString Variant::typeName( void ) const
	{
		switch( type() ) {
		case BooleanValue:	return "Boolean";
		case NumberValue:	return "Number";
		case StringValue:	return "String";
		case ArrayValue:	return "Array";
		}

		return "Unknown";
	}

	// ** VariantRegistry::operator[]
	inline Variant& VariantRegistry::operator[]( CString key )
	{
		Properties::iterator i = m_properties.find( key );

		if( i != m_properties.end() ) {
			return i->second;
		}

		m_properties[key] = Variant();
		return m_properties[key];
	}

	// ** VariantRegistry::properties
	inline const VariantRegistry::Properties& VariantRegistry::properties( void ) const
	{
		return m_properties;
	}

DC_END_DREEMCHEST

#endif	/*	!__Base_Variant_H__	*/
