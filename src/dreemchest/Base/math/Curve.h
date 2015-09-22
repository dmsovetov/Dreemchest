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

#ifndef __Base_Curve_H__
#define __Base_Curve_H__

DC_BEGIN_DREEMCHEST

    //! Parameter curve used for animation.
    template<typename T>
    class Curve {
    public:

        //! A single curve keyframe.
        struct Keyframe {
            f32				m_time;		//!< Keyframe time.
            T				m_value;	//!< Keyframe value.

							Keyframe( void ) {}
							Keyframe( f32 time, const T& value )
								: m_time( time ), m_value( value ) {}
        };

		//! Container type to store cureve keyframes.
        typedef Array<Keyframe>  Keyframes;

    public:

		//! Clears the curve keyframes.
        void                    clear( void );

		//! Samples the curve value at specified time.
        bool                    sample( f32 time, T& value ) const;

		//! Returns the keyframe value at specified index.
		bool					value( s32 index, T& value ) const;

		//! Removes the keyframe by index.
        void                    remove( s32 index );

		//! Inserts a new keyframe at specified index.
        void                    insert( s32 index, f32 time, const T& value );

		//! Sets the keyframe at specified index.
        void                    set( s32 index, f32 time, const T& value );

		//! Returns the total number of keyframes.
        s32                     keyframeCount( void ) const;

		//! Returns the keyframe by index.
        const Keyframe&			keyframe( s32 index ) const;

    private:

		//! Finds the two keyframes used for interpolation.
        bool                    findKeyframes( f32 time, const Keyframe** a, const Keyframe** b, f32* factor ) const;

		//! Returns the keyframe index for specified time,
        s32                     findIndexByTime( f32 time ) const;

		//! Returns the value on the edge of a curve.
		bool					sampleEdge( f32 time, T& value ) const;

    private:
        
        Keyframes				m_keyframes;	//!< Curve keyframes.
    };

    // ** Curve::keyframeCount
    template<typename T>
    s32 Curve<T>::keyframeCount( void ) const
    {
        return ( s32 )m_keyframes.size();
    }

    // ** Curve::keyframe
    template<typename T>
    const typename Curve<T>::Keyframe& Curve<T>::keyframe( s32 index ) const
    {
        DC_BREAK_IF( index < 0 || index >= keyframeCount() );
        return m_keyframes[index];
    }

	// ** Curve::sampleEdge
	template<typename T>
	bool Curve<T>::sampleEdge( f32 time, T& value ) const
	{
		if( time <= m_keyframes[0].m_time ) {
			value = m_keyframes[0].m_value;
			return true;
		}

		if( time >= m_keyframes.back().m_time ) {
			value = m_keyframes.back().m_value;
			return true;
		}

		return false;
	}

	// ** Curve::value
	template<typename T>
	bool Curve<T>::value( s32 index, T& value ) const
	{
		if( index < 0 || index >= keyframeCount() ) {
			return false;
		}

		value = m_keyframes[index].m_value;
		return true;
	}

    // ** Curve::sample
    template<typename T>
    bool Curve<T>::sample( f32 time, T& value ) const
    {
		if( m_keyframes.empty() ) {
			return false;
		}

        const Keyframe *a, *b;
        f32 factor;

		if( sampleEdge( time, value ) ) {
			return true;
		}

        if( !findKeyframes( time, &a, &b, &factor ) ) {
			return false;
        }

        value = a->m_value * (1.0f - factor) + b->m_value * factor;
        return true;
    }

    // ** Curve::clear
    template<typename T>
    void Curve<T>::clear( void )
    {
        m_keyframes.clear();
    }

    // ** Curve::insert
    template<typename T>
    void Curve<T>::insert( s32 index, f32 time, const T& value )
    {
        DC_BREAK_IF( index < 0 );
        m_keyframes.insert( m_keyframes.begin() + index, Keyframe( time, value ) );
    }

    // ** Curve::remove
    template<typename T>
    void Curve<T>::remove( s32 index )
    {
        DC_BREAK_IF( index < 0 || index >= keyframeCount() );
        m_keyframes.erase( m_keyframes.begin() + index );
    }

    // ** Curve::set
    template<typename T>
    void Curve<T>::set( s32 index, f32 time, const T& value )
    {
        DC_BREAK_IF( index < 0 || index >= keyframeCount() );
        m_keyframes[index].m_scalar = time;
        m_keyframes[index].m_value  = value;
    }

    // ** Curve::findIndexByTime
    template<typename T>
    s32 Curve<T>::findIndexByTime( f32 time ) const
    {
        s32 n = m_keyframes.size();
        
        for( s32 i = n - 1; i >= 0; i-- ) {
            if( time >= m_keyframes[i].m_time && i != n - 1 ) {
                return i;
            }
        }

        return -1;
    }

    // ** Curve::findKeyframes
    template<typename T>
    bool Curve<T>::findKeyframes( f32 time, const Keyframe** a, const Keyframe** b, f32* factor ) const
    {
        s32 index = findIndexByTime( time );
        if( index == -1 ) {
            return false;
        }

        const Keyframe& valueA = m_keyframes[index + 0];
        const Keyframe& valueB = m_keyframes[index + 1];

        *a      = &valueA;
        *b      = &valueB;
        *factor = (time - valueA.m_time) / (valueB.m_time - valueA.m_time);

        return true;
    }

DC_END_DREEMCHEST

#endif		/*	!__Base_Curve_H__	*/
