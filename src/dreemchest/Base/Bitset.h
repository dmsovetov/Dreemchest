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
 
#ifndef	__Base_Bitset_H__
#define	__Base_Bitset_H__

DC_BEGIN_DREEMCHEST

	// ** class Bitset
	class Bitset {
	public:

									//! Constructs an empty bitset.
									Bitset( void ) : m_bits( 1 ) { clear(); }

		//! Returns true if a bitset has at least one bit set.
									operator bool( void ) const;

		//! Compares two bitsets.
		bool						operator < ( const Bitset& other ) const;

		//! Returns true if two bitsets are not equal.
		bool						operator != ( const Bitset& other ) const;

		//! Returns an intersection of bitsets.
		Bitset						operator & ( const Bitset& other ) const;

		//! Returns a union of bitsets.
		Bitset						operator | ( const Bitset& other ) const;

		//! Returns true if two bitsets have an intersection.
		bool						operator * ( const Bitset& other ) const;

		//! Returns a bitset size.
		unsigned int				size( void ) const;

		//! Turns on a given bit.
		void						set( unsigned int bit );

		//! Checks a given bit
		bool						is( unsigned int bit ) const;

		//! Clears a given bit
		void						clear( unsigned int bit );

		//! Clears a bitset.
		void						clear( void );

		//! Creates a bitset with just a single bit set.
		static Bitset				withSingleBit( unsigned int bit );

	private:

		//! Bitset buffer.
		std::vector<unsigned char>	m_bits;
	};

	// ** Bitset::withSingleBit
	inline Bitset Bitset::withSingleBit( unsigned int bit ) {
		Bitset bitset;
		bitset.set( bit );
		return bitset;
	}

	// ** Bitset::operator bool
	inline Bitset::operator bool( void ) const
	{
		for( int i = 0, n = m_bits.size(); i < n; i++ ) {
			if( m_bits[i] ) return true;
		}

		return false;
	}

	// ** Bitset::operator <
	inline bool Bitset::operator < ( const Bitset& other ) const
	{
		u32 bit = max2( size(), other.size() );

		while( bit-- ) {
			bool a = is( bit );
			bool b = other.is( bit );

			if( a != b ) {
				return a ? false : true;
			}
		}

		return false;
	}

	// ** Bitset::operator !=
	inline bool Bitset::operator != ( const Bitset& other ) const
	{
		u32 bit = max2( size(), other.size() );

		while( bit-- ) {
			if( is( bit ) != other.is( bit ) ) {
				return true;
			}
		}

		return false;
	}

	// ** Bitset::operator *
	inline bool Bitset::operator * ( const Bitset& other ) const {
		int size = std::min( m_bits.size(), other.m_bits.size() );

		for( int i = 0; i < size; i++ ) {
			if( m_bits[i] & other.m_bits[i] ) {
				return true;
			}
		}

		return false;
	}

	// ** Bitset::size
	inline unsigned int Bitset::size( void ) const
	{
		return m_bits.size() * 8;
	}

	// ** Bitset::operator |
	inline Bitset Bitset::operator | ( const Bitset& other ) const {
		Bitset result;

		int nBits = std::max( size(), other.size() );

		for( int i = 0; i < nBits; i++ ) {
			if( is( i ) || other.is( i ) ) result.set( i );
		}

		return result;
	}

	// ** Bitset::set
	inline void Bitset::set( unsigned int bit ) {
		unsigned int idx = bit >> 3;

		if( idx >= m_bits.size() ) {
			m_bits.resize( idx + 1 );
		}

		m_bits[idx] |= (1 << (bit & 7));
	}

	// ** Bitset::is
	inline bool Bitset::is( unsigned int bit ) const {
		unsigned int idx = bit >> 3;
		return ( idx < m_bits.size() && m_bits[idx] & (1 << (bit & 7 )) ) ? true : false;
	}

	// ** Bitset::clear
	inline void Bitset::clear( unsigned int bit ) {
		unsigned int idx = bit >> 3;

		if( idx < m_bits.size() ) {
			m_bits[idx] &= ~(1 << (bit & 7));
		}
	}

	// ** Bitset::clear
	inline void Bitset::clear( void ) {
		memset( &m_bits[0], 0, m_bits.size() );
	}

DC_END_DREEMCHEST

#endif		/*	!__DC_Bitset_H__	*/