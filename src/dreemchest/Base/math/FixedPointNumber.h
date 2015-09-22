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

#ifndef ___Base_FixedPointNumber_H__
#define ___Base_FixedPointNumber_H__

DC_BEGIN_DREEMCHEST

	//! A FixedPointNumber class is used for packing a floating point values to integers and use integer math.
	template<typename T, unsigned char F>
	class FixedPointNumber {
	public:

		typedef T Type;

                                //! Constructs a fixed point value.
                                template<typename Type>
                                FixedPointNumber( const Type& value )
                                {
                                    m_value = static_cast<T>( value * static_cast<Type>( static_cast<T>(1) << F ) );
                                }

                                //! Converts to float value.
                                operator float( void ) const;

                                //! Converts to integer value.
                                operator int( void ) const;

		//! Returns an integer value of this fixed value.
		int                     toInt( void ) const;

		//! Returns a float value of this fixed value.
		float                   toFloat( void ) const;

		//! Returns a raw value.
		T						raw( void ) const { return m_value; }

		//! Sets a raw value.
		void					setRaw( const T& value ) { m_value = value; }

		//! Adds to fixed point values.
		FixedPointNumber        operator + ( const FixedPointNumber& other ) const;

		//! Subtracts to fixed point values
		FixedPointNumber        operator - ( const FixedPointNumber& other ) const;

		//! Multiplies two fixed point values.
		FixedPointNumber        operator * ( const FixedPointNumber& other ) const;

		//! Increments a fixed point value.
		const FixedPointNumber& operator += ( const FixedPointNumber& other );

		//! Compare two fixed point values.
		bool                    operator < ( const FixedPointNumber& other ) const;

		//! Compare two fixed point values.
		bool                    operator <= ( const FixedPointNumber& other ) const;

		//! Compare two fixed point values.
		bool                    operator > ( const FixedPointNumber& other ) const;

		//! Compare two fixed point values.
		bool                    operator >= ( const FixedPointNumber& other ) const;

		//! Returns a minimum step value we can represent.
		static double           step( void ) { return 1.0 / (static_cast<T>(1) << F); }

	private:

                                //! Constructs a FixedPointNumber value from a raw data.
                                FixedPointNumber( const T& value, const char* dummy ) : m_value( value ) {}

	private:

		//! Encoded fixed point value.
		T                       m_value;
	};

	// ** FixedPointNumber::operator float
	template<typename T, unsigned char F>
	inline FixedPointNumber<T, F>::operator float( void ) const
	{
		return toFloat();
	}

	// ** FixedPointNumber::operator int
	template<typename T, unsigned char F>
	inline FixedPointNumber<T, F>::operator int( void ) const
	{
		return toInt();
	}

	// ** FixedPointNumber::toInt
	template<typename T, unsigned char F>
	inline int FixedPointNumber<T, F>::toInt( void ) const
	{
		return m_value >> F;
	}

	// ** FixedPointNumber::toFloat
	template<typename T, unsigned char F>
	inline float FixedPointNumber<T, F>::toFloat( void ) const
	{
		return m_value * static_cast<float>( step() );
	}

	// ** FixedPointNumber::operator *
	template<typename T, unsigned char F>
	inline FixedPointNumber<T, F> FixedPointNumber<T, F>::operator * ( const FixedPointNumber& other ) const
	{
		return FixedPointNumber( static_cast<T>( (static_cast<long long>( m_value ) * static_cast<long long>( other.m_value )) >> F), NULL );
	}

	// ** FixedPointNumber::operator <
	template<typename T, unsigned char F>
	inline bool FixedPointNumber<T, F>::operator < ( const FixedPointNumber& other ) const
	{
		return m_value < other.m_value;
	}

	// ** FixedPointNumber::operator <=
	template<typename T, unsigned char F>
	inline bool FixedPointNumber<T, F>::operator <= ( const FixedPointNumber& other ) const
	{
		return m_value <= other.m_value;
	}

	// ** FixedPointNumber::operator >
	template<typename T, unsigned char F>
	inline bool FixedPointNumber<T, F>::operator > ( const FixedPointNumber& other ) const
	{
		return m_value > other.m_value;
	}

	// ** FixedPointNumber::operator >=
	template<typename T, unsigned char F>
	inline bool FixedPointNumber<T, F>::operator >= ( const FixedPointNumber& other ) const
	{
		return m_value >= other.m_value;
	}


	// ** FixedPointNumber::operator +=
	template<typename T, unsigned char F>
	inline const FixedPointNumber<T, F>& FixedPointNumber<T, F>::operator += ( const FixedPointNumber& other )
	{
		m_value = m_value + other.m_value;
		return *this;
	}

DC_END_DREEMCHEST

#endif	/*	!__Base_FixedPointNumber_H__	*/
