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

#ifndef __DC_Fx_Parameter_H__
#define __DC_Fx_Parameter_H__

#include "Fx.h"

#define SampleParameter( parameter, default )  ((parameter) ? (parameter)->sample( scalar, default ) : (default))
#define SampleKoeficient( parameter, default ) SampleParameter( parameter, default * 100.0f ) * 0.01f

DC_BEGIN_DREEMCHEST

namespace Fx {

	//! Generic class used for declaring the parameter types.
	template<typename TValue>
	class Parameter {
	public:

		//! Parameter curve indices.
		enum CurveIndex {
			  Lower = 0			//!< Parameter lower bound.
			, Upper				//!< Parameter upper bounds.
			, TotalCurveIndices	//!< The total number of curve indices.
		};

		//! Alias the curve type.
		typedef Curve<TValue>	CurveType;

								//! Constructs the Parameter instance.
								Parameter( void );

								//! Returns true if the parameter is enabled.
								operator bool() const;

		//! Returns true if the parameter is enabled.
		bool					isEnabled( void ) const;

		//! Enables or disables the parameter.
		void					setEnabled( bool value );

		//! Returns the parameter sampling mode.
		SamplingMode			samplingMode( void ) const;

		//! Sets the parameter sampling mode.
		void					setSamplingMode( SamplingMode value );

		//! Sets the constant parameter value.
		void					setConstant( const TValue& value );

		//! Scales the parameter values.
		void					scale( f32 value );

		//! Returns the parameter curve by index.
		const CurveType&		curve( CurveIndex index ) const;
		CurveType&				curve( CurveIndex index );

		//! Samples the parameter at specified time.
		TValue					sample( f32 scalar, const TValue& defaultValue = TValue() ) const;

	private:

		bool					m_isEnabled;					//!< The flag indicating that parameter is enabled.
		SamplingMode			m_mode;							//!< The parameter sampling mode.
		CurveType				m_curves[TotalCurveIndices];	//!< Minimum and maximum curves.
	};

	// ** Parameter::Parameter
	template<typename TValue>
	Parameter<TValue>::Parameter( void ) : m_isEnabled( false ), m_mode( SampleConstant )
	{

	}

	// ** Parameter::operator bool
	template<typename TValue>
	Parameter<TValue>::operator bool() const
	{
		return m_isEnabled;
	}

	// ** Parameter::isEnabled
	template<typename TValue>
	bool Parameter<TValue>::isEnabled( void ) const
	{
		return m_isEnabled;
	}

	// ** Parameter::scale
	template<typename TValue>
	void Parameter<TValue>::scale( f32 value )
	{
		for( s32 i = 0; i < TotalCurveIndices; i++ ) {
			for( s32 j = 0, n = m_curves[i].keyframeCount(); j < n; j++ ) {
				m_curves[i].keyframe( j ).m_value *= value;
			}
		}
	}

	// ** Parameter::setConstant
	template<typename TValue>
	void Parameter<TValue>::setConstant( const TValue& value )
	{
		setSamplingMode( SampleConstant );
		curve( Lower ).clear();
		curve( Lower ).push( 0.0f, value );
	}

	// ** Parameter::curve
	template<typename TValue>
	const typename Parameter<TValue>::CurveType& Parameter<TValue>::curve( CurveIndex index ) const
	{
		return m_curves[index];
	}

	// ** Parameter::curve
	template<typename TValue>
	typename Parameter<TValue>::CurveType& Parameter<TValue>::curve( CurveIndex index )
	{
		return m_curves[index];
	}

	// ** Parameter::setEnabled
	template<typename TValue>
	void Parameter<TValue>::setEnabled( bool value )
	{
		m_isEnabled = value;
	}

	// ** Parameter::samplingMode
	template<typename TValue>
	SamplingMode Parameter<TValue>::samplingMode( void ) const
	{
		return m_mode;
	}

	// ** Parameter::setSamplingMode
	template<typename TValue>
	void Parameter<TValue>::setSamplingMode( SamplingMode value )
	{
		m_mode = value;
	}

	// ** Parameter::sample
	template<typename TValue>
	TValue Parameter<TValue>::sample( f32 scalar, const TValue& defaultValue ) const
	{
		DC_BREAK_IF( scalar < 0.0f || scalar > 1.0f );

		// Set result to a default value
		TValue result = defaultValue;

		switch( m_mode ) {
		case SampleConstant:				{
												m_curves[Lower].value( 0, result );
											}
											break;
		case SampleRandomBetweenConstants:	{
												TValue a, b;
												m_curves[Lower].value( 0, a );
												m_curves[Upper].value( 0, b );

												result = randomValue( a, b );
											}
											break;

		case SampleCurve:					{
												m_curves[Lower].sample( scalar, result );
											}
											break;

		case SampleRandomBetweenCurves:		{
												TValue a, b;
												m_curves[Lower].sample( scalar, a );
												m_curves[Upper].sample( scalar, b );

												result = randomValue( a, b );	
											}
											break;
		}

		return result;
	}

	//! Float parameter type.
	class FloatParameter : public Parameter<f32> {
	public:

		//! Sets the scalar parameter data.
		void								setData( const FloatArray& value, CurveIndex index );

		//! Sets the curve parameter value.
		void								setCurve( const FloatArray& data );

		//! Sets the random between to constants parameter.
		void								setRandomBetweenConstants( f32 min, f32 max );

		//! Sets the random between curves parameter value.
		void								setRandomBetweenCurves( const FloatArray& min, const FloatArray& max );
	};

	//! Vector parameter type.
	class Vec3Parameter : public Parameter<Vec3> {
	public:

		//! Sets the scalar parameter data.
		void								setData( const FloatArray& value, CurveIndex index );
	};

	//! Rgb color parameter type.
	class RgbParameter : public Parameter<Rgb> {
	public:

		//! Sets the scalar parameter data.
		void								setData( const FloatArray& value, CurveIndex index );

		//! Sets the curve parameter value.
		void								setCurve( const FloatArray& data );

		//! Sets the random between curves parameter value.
		void								setRandomBetweenCurves( const FloatArray& min, const FloatArray& max );
	};

} // namespace Fx

DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_Parameter_H__	*/