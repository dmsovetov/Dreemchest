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

#ifndef __Base_LinearRegression_H__
#define __Base_LinearRegression_H__

DC_BEGIN_DREEMCHEST

	//! Linear regression model.
	template<typename T>
	class LinearRegression {
	public:

		//! Linear function sample.
		struct Sample {
			T					m_argument;	//!< Function argument.
			T					m_value;	//!< Function value.

								Sample( T argument = 0.0f, T value = 0.0f )
									: m_argument( argument ), m_value( value ) {}
		};

		//! The linear function samples wrapper.
		class Samples {
		public:

			//! Adds a new sample to the set.
			const Samples&		operator += ( const Sample& sample );

			//! Returns the sample by index.
			const Sample&		operator [] ( s32 index ) const;

			//! Returns the total number of samples in set.
			s32					count( void ) const;

			//! Returns the mean value of a function argument.
			T					argumentMean( void ) const;

			//! Returns the mean value of a function value.
			T					valueMean( void ) const;

		private:

			Array<Sample>		m_samples;	//!< Gathered function samples.
		};

		//! Evaluates the function with a specified argument.
		T						operator()( const T& argument ) const;

		//! Returns the linear regression SST (total sum of squares).
		T						sst( void ) const;

		//! Returns the linear regression SSE (sum of squares due to error).
		T						sse( void ) const;

		//! Returns the linear regression SSR (sum of squares due to regression).
		T						ssr( void ) const;

		//! Returns the linear slope.
		T						slope( void ) const;

		//! Returns the linear intercept.
		T						intercept( void ) const;

		//! Creates a linear regression model from a set of samples.
		static LinearRegression createFromSamples( const Samples& samples );

		//! Preforms a self test.
		static void             selfTest( void );

	private:

								//! Constructs the LinearRegression instance
								LinearRegression( T slope, T intercept, T error );

		//! Computes the errors for a built regression model.
		void                    computeErrors( const Samples& samples );

	private:

		T						m_slope;				//!< Linear slope.
		T						m_intercept;			//!< Linear intercept.
		T						m_error;				//!< Regression error.
		T						m_sst, m_sse, m_ssr;	//!< SST, SSE and SSR values.
	};

	// ** LinearRegression<T>::Samples::operator +=
	template<typename T>
	const typename LinearRegression<T>::Samples& LinearRegression<T>::Samples::operator += ( const Sample& sample )
	{
		m_samples.push_back( sample );
		return *this;
	}

	// ** LinearRegression<T>::Samples::operator []
	template<typename T>
	const typename LinearRegression<T>::Sample& LinearRegression<T>::Samples::operator [] ( s32 index ) const
	{
		assert( index >= 0 && index < count() );
		return m_samples[index];
	}

	// ** LinearRegression<T>::Samples::count
	template<typename T>
	s32 LinearRegression<T>::Samples::count( void ) const
	{
		return ( s32 )m_samples.size();
	}

	// ** LinearRegression<T>::Samples::argumentMean
	template<typename T>
	T LinearRegression<T>::Samples::argumentMean( void ) const
	{
		T sum = 0.0f;

		for( int i = 0, n = count(); i < n; i++ ) {
			sum += m_samples[i].m_argument;
		}

		return sum / count();
	}

	// ** LinearRegression<T>::Samples::valueMean
	template<typename T>
	T LinearRegression<T>::Samples::valueMean( void ) const
	{
		T sum = 0.0f;

		for( int i = 0, n = count(); i < n; i++ ) {
			sum += m_samples[i].m_value;
		}

		return sum / count();
	}

	// ** LinearRegression<T>::LinearRegression
	template<typename T>
	LinearRegression<T>::LinearRegression( T slope, T intercept, T error ) : m_slope( slope ), m_intercept( intercept ), m_error( error ), m_sst( 0.0f ), m_sse( 0.0f ), m_ssr( 0.0f )
	{

	}

	// ** LinearRegression<T>::sst
	template<typename T>
	T LinearRegression<T>::sst( void ) const
	{
		return m_sst;
	}

	// ** LinearRegression<T>::sse
	template<typename T>
	T LinearRegression<T>::sse( void ) const
	{
		return m_sse;
	}

	// ** LinearRegression<T>::ssr
	template<typename T>
	T LinearRegression<T>::ssr( void ) const
	{
		return m_ssr;
	}

	// ** LinearRegression<T>::intercept
	template<typename T>
	T LinearRegression<T>::intercept( void ) const
	{
		return m_intercept;
	}

	// ** LinearRegression<T>::slope
	template<typename T>
	T LinearRegression<T>::slope( void ) const
	{
		return m_slope;
	}

	// ** LinearRegression<T>::createFromSamples
	template<typename T>
	LinearRegression<T> LinearRegression<T>::createFromSamples( const Samples& samples )
	{
		T argumentMean  = samples.argumentMean();
		T valueMean     = samples.valueMean();

		T num = 0.0f;
		T den = 0.0f;

		// ** Evaluate slope
		for( s32 i = 0, n = samples.count(); i < n; i++ ) {
			const LinearRegression<T>::Sample& sample = samples[i];

			T da = sample.m_argument - argumentMean;
			T dv = sample.m_value    - valueMean;

			num += da * dv;
			den += da * da;
		}

		T slope = num / den;

		// ** Evaluate intercept
		T intersept = valueMean - slope * argumentMean;

		// ** Create a regression & compute errors
		LinearRegression result = LinearRegression( slope, intersept, 0.0f );
		result.computeErrors( samples );

		return result;
	}

	// ** LinearRegression<T>::computeErrors
	template<typename T>
	void LinearRegression<T>::computeErrors( const Samples& samples )
	{
		T mean = samples.valueMean();

		m_sst = m_sse = m_ssr = 0.0f;

		for( int i = 0, n = samples.count(); i < n; i++ ) {
			const Sample& sample = samples[i];

			T predicted = ( *this )( sample.m_argument );

			m_sst += powf( sample.m_value - mean,       2.0f );
			m_ssr += powf( predicted      - mean,       2.0f );
			m_sse += powf( sample.m_value - predicted,  2.0f );
		}
	}

	// ** LinearRegression<T>::operator()
	template<typename T>
	T LinearRegression<T>::operator()( const T& argument ) const
	{
		return argument * m_slope + m_intercept + m_error;
	}

	// ** LinearRegression<T>::selfTest
	template<typename T>
	void LinearRegression<T>::selfTest( void )
	{
		Samples samples;

		samples += Sample( 34,  5  );
		samples += Sample( 108, 17 );
		samples += Sample( 64,  11 );
		samples += Sample( 88,  8  );
		samples += Sample( 99,  14 );
		samples += Sample( 51,  5  );

		assert( samples.argumentMean() == 74.0f );
		assert( samples.valueMean() == 10.0f );

		LinearRegression lr = LinearRegression<T>::createFromSamples( samples );

		assert( compareFloats( lr.slope(),      0.146219686f ) );
		assert( compareFloats( lr.intercept(), -0.820257187f ) );
		assert( compareFloats( lr.sst(),        120          ) );
		assert( compareFloats( lr.sse(),        30.074894f   ) );
		assert( compareFloats( lr.ssr(),        89.9251022   ) );

		assert( compareFloats( lr( 212 ), 30.18, 0.01f ) );
	}

DC_END_DREEMCHEST

#endif  /*  !defined( __Base_LinearRegression_H__ )  */
