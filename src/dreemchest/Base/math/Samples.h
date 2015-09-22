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

#ifndef __Base_Samples_H__
#define __Base_Samples_H__

DC_BEGIN_DREEMCHEST

	//! The sample set.
	template<typename T>
	class Samples {
	public:

		//! Alias the sample type.
		typedef T		Sample;

						//! Constructs Samples instance.
						Samples( void );

						//! Constructs Samples instance with an array of samples.
						Samples( const Array<T>& samples );

		//! Appends a new sample to the set.
		Samples&		operator << ( const T& sample );

		//! Returns the sample with a specified index.
		const T&		operator[]( s32 index ) const;

		//! Returns the sample at specified index.
		const T&		at( s32 index ) const;

		//! Returns the mean value of a sample set.
		T				mean( void ) const;

		//! Returns the variance of a sample set.
		T				variance( void ) const;

		//! Creates a new sample set by a subtracting the mean value from each sample.
		Samples<T>		centered( void ) const;

		//! Returns the standard deviation of a sample set.
		T				sdev( void ) const;

		//! Returns the total number of samples.
		u32				size( void ) const;

	private:

		//! The cached sample set state.
		struct State {
			//! A bitset flags that store a sample set state.
			enum {
				  RecomputeMean		= BIT( 0 )
				, RecomputeVariance	= BIT( 1 )
				, RecomputeStdDev	= BIT( 2 )
				, RecomputeAll		= RecomputeMean | RecomputeVariance | RecomputeStdDev
			};

						//! Constructs State instance.
						State( void )
							: flags( 0 ), mean( 0.0f ), variance( 0.0f ), sdev( 0.0f ) {}

			//! Sets the state flag.
			void		set( u8 mask ) { flags |= mask; }

			//! Returns true if a state flag is set.
			bool		is( u8 mask ) const { return (flags & mask) != 0; }

			//! Resets the state flag.
			void		reset( u8 mask ) { flags &= ~mask; }

			u8			flags;		//!< The sample set state flags.
			T			mean;		//!< The cached mean value of a sample set.
			T			variance;	//!< The cached variance value of a sample set.
			T			sdev;		//!< The cached standard deviation value of a sample set.
		};

		Array<T>		m_samples;	//!< The array of samples.
		mutable State	m_state;	//!< The sample set state.
	};

	// ** Samples<T>::Samples
	template<typename T>
	Samples<T>::Samples( void )
	{
	}

	// ** Samples<T>::Samples
	template<typename T>
	Samples<T>::Samples( const Array<T>& samples ) : m_samples( samples )
	{
		m_state.set( State::RecomputeAll );
	}

	// ** Samples::operator[]
	template<typename T>
	const T& Samples<T>::operator[]( s32 index ) const
	{
		return at( index );
	}

	// ** Samples::operator[]
	template<typename T>
	Samples<T>& Samples<T>::operator << ( const T& sample )
	{
		m_state.set( State::RecomputeAll );
		m_samples.push_back( sample );
		return *this;
	}

	// ** Samples::at
	template<typename T>
	const T& Samples<T>::at( s32 index ) const
	{
		DC_BREAK_IF( index < 0 || index >= ( s32 )size() );
		return m_samples[index];
	}

	// ** Samples::size
	template<typename T>
	u32 Samples<T>::size( void ) const
	{
		return ( u32 )m_samples.size();
	}

	// ** Samples::centered
	template<typename T>
	Samples<T> Samples<T>::centered( void ) const
	{
		Samples<T> result;

		T m = mean();

		for( u32 i = 0, n = size(); i < n; i++ ) {
			result << m_samples[i] - m;
		}

		return result;
	}

	// ** Samples::mean
	template<typename T>
	T Samples<T>::mean( void ) const
	{
		if( !m_state.is( State::RecomputeMean ) ) {
			return m_state.mean;
		}

		// ** Recompute the mean
		T sum = 0.0f;

		for( u32 i = 0, n = ( u32 )m_samples.size(); i < n; i++ ) {
			sum += at( i );
		}

		m_state.mean = sum / size();
		m_state.reset( State::RecomputeMean );

		return m_state.mean;
	}

	// ** Samples::variance
	template<typename T>
	T Samples<T>::variance( void ) const
	{
		if( !m_state.is( State::RecomputeVariance ) ) {
			return m_state.variance;
		}

		// ** Recompute the variance.
		T sum = 0.0f;
		T u   = mean();

		for( u32 i = 0, n = ( u32 )m_samples.size(); i < n; i++ ) {
			sum += pow( at( i ) - u, 2.0f );
		}

		m_state.variance = sum / size();
		m_state.reset( State::RecomputeVariance );

		return m_state.variance;
	}

	// ** Samples::sdev
	template<typename T>
	T Samples<T>::sdev( void ) const
	{
		if( !m_state.is( State::RecomputeStdDev ) ) {
			return m_state.sdev;
		}

		m_state.sdev = sqrtf( variance() );
		m_state.reset( State::RecomputeStdDev );

		return m_state.sdev;
	}

	//! A covariance matrix class.
	template<typename T>
	class CovMatrix : public Matrix<T> {
	public:

						//! Constructs the CovMatrix instance.
						CovMatrix( s32 size );

		//! Rotates the vector until it converges.
		Vector<T>		converge( const Vector<T>& v, s32 iterations = 10 ) const;
	};

	// ** CovMatrix::CovMatrix
	template<typename T>
	CovMatrix<T>::CovMatrix( s32 size ) : Matrix<T>( size, size )
	{
	}

	// ** CovMatrix::converge
	template<typename T>
	Vector<T> CovMatrix<T>::converge( const Vector<T>& v, s32 iterations ) const
	{
		Vector<T> r = *this * v;

		for( s32 i = 0; i < iterations - 1; i++ ) {
			r = *this * r;
		}

		return r;
	}

	namespace detail {

		//! Computes the numerator of a sample set covariance.
		template<typename T>
		T covariance( const Samples<T>& a, const Samples<T>& b )
		{
			DC_BREAK_IF( a.size() != b.size() );

			T am  = a.mean();
			T bm  = b.mean();
			T cov = 0.0f;

			for( u32 i = 0, n = a.size(); i < n; i++ ) {
				const T& sa = a[i];
				const T& sb = b[i];

				cov += (sa - am) * (sb - bm);
			}

			return cov;
		}

	} // namespace detail

	//! Computes the sample covariance of a two sample sets.
	template<typename T>
	T sampleCovariance( const Samples<T>& a, const Samples<T>& b )
	{
		return detail::covariance( a, b ) / (a.size() - 1);
	}

	//! Computes the population covariance of a two sample sets.
	template<typename T>
	T populationCovariance( const Samples<T>& a, const Samples<T>& b )
	{
		return detail::covariance( a, b ) / a.size();
	}

	//! Computes the covariance matrix of a two sample sets.
	template<typename T, typename TSamples>
	CovMatrix<T> covarianceMatrix( const TSamples& samples, u32 dimensions )
	{
		// ** The size of a sample set.
		u32 n = samples.size();

		// ** Get the sample set mean.
		typename TSamples::Sample mean = samples.mean();

		// ** The resulting covariance matrix.
		CovMatrix<T> result( dimensions );

		// ** Compute the covariance matrix.
		for( u32 row = 0; row < dimensions; row++ ) {
			for( u32 col = 0; col < dimensions; col++ ) {
				T cov = 0.0f;

				for( u32 i = 0; i < n; i++ ) {
					cov += (samples[i][row] - mean[row]) * (samples[i][col] - mean[col]);
				}

				result( col, row ) = cov / n;
			}
		}

		return result;
	}

DC_END_DREEMCHEST

#endif  /*  !defined( __Base_Samples_H__ )  */
