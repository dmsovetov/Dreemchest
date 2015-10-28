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

#ifndef	__Base_FlagSet_H__
#define	__Base_FlagSet_H__

DC_BEGIN_DREEMCHEST

	//! Holds the bitmask.
	template<typename T>
	class FlagSet {
	public:

					//! Constructs the FlagSet instance.
					FlagSet( void )
						: m_mask( 0 ) {}

					//! Constructs the FlagSet instance.
					FlagSet( T value )
						: m_mask( value ) {}

		//! Compares two bit masks.
		bool		operator == ( const FlagSet& other ) const;

		//! Compares the bit mask with a number.
		bool		operator == ( T mask ) const;

		//! Converts the bit mask to a number,
					operator T( void ) const;

		//! Sets the specified bit mask to a value.
		void		set( T mask, bool set );

		//! Turns on the specified bit mask.
		void		on( T mask );

		//! Turns off the specified bit mask.
		void		off( T mask );

		//! Check the specified bit mask.
		bool		is( T mask ) const;

		//! Returns true if the specified bit mask is not set.
        bool		disabled( T mask ) const;

	private:

		T			m_mask;	//!< Bit mask.
	};
	
	// ** FlagSet<T>::operator ==
	template<typename T>
	bool FlagSet<T>::operator == ( const FlagSet& other ) const
	{
		return m_mask == other.m_mask;
	}

	// ** FlagSet<T>::operator ==
	template<typename T>
	bool FlagSet<T>::operator == ( T mask ) const
	{
		return m_mask == mask;
	}

	// ** FlagSet<T>::operator T
	template<typename T>
	FlagSet<T>::operator T ( void ) const
	{
		return m_mask;
	}

	// ** FlagSet::set
	template<typename T>
	inline void FlagSet<T>::set( T mask, bool set ) {
		m_mask = set ? (m_mask | mask) : (m_mask & ~mask);
	}

	// ** FlagSet::on
	template<typename T>
	inline void FlagSet<T>::on( T mask ) {
		m_mask = m_mask | mask;
	}

	// ** FlagSet::off
	template<typename T>
	inline void FlagSet<T>::off( T mask ) {
		m_mask = m_mask & ~mask;
	}

	// ** FlagSet::is
	template<typename T>
	inline bool FlagSet<T>::is( T mask ) const {
		return ( m_mask & mask ) ? true : false;
	}
    
	// ** FlagSet::disabled
	template<typename T>
	inline bool FlagSet<T>::disabled( T mask ) const {
		return ( m_mask & mask ) ? false : true;
	}

	typedef FlagSet<u8> FlagSet8;
	typedef FlagSet<u16> FlagSet16;
	typedef FlagSet<u32> FlagSet32;

DC_END_DREEMCHEST

#endif	/*	!__Base_FlagSet_H__	*/
