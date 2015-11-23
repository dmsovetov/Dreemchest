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

#include "../Fx.h"

#define SampleParameter( parameter, default )  ((parameter) ? (parameter)->sample( scalar, default ) : (default))
#define SampleKoeficient( parameter, default ) SampleParameter( parameter, default * 100.0f ) * 0.01f

DC_BEGIN_DREEMCHEST

namespace Fx {

    typedef Curve<float>    CurveScalar;
    typedef Curve<Rgb>      CurveRgb;

    // ** class Parameter
    class Parameter {
        
        DC_DECLARE_IS( Parameter, Parameter, this );

	public:

		enum { Min = 0, Max = 1 };

		// ** enum eType
        enum eType {
            Unknown,
            Scalar,
            Color
        };

		// ** eMode
		enum eMode {
			Constant,
			Interpolate,
			RandomConst,
			RandomInterpolate,
		};

    public:

                                Parameter( void );
                                ~Parameter( void );

        eType                   type( void ) const;
        void                    setType( eType value );
		eMode					mode( void ) const;
		void					setMode( eMode value );
        int                     totalKeyframes( int curve ) const;
        CurveScalar*            scalarCurve( int curve ) const;
        CurveRgb*               colorCurve( int curve ) const;
        bool                    isEnabled( void ) const;
        void                    setEnabled( bool value );
		void					setConstant( float value );
		void					setRange( float min, float max );
        FloatArray				data( int curve ) const;
        void                    setData( const FloatArray& data, int curve );

        void                    clear( void );
        float                   sample( float scalar, float defaultValue = 0.0f ) const;
        Rgb                     sample( float scalar, const Rgb& defaultValue = Rgb( 1.0f, 1.0f, 1.0f ) ) const;

        operator bool( void ) const { return m_isEnabled; }

    protected:

        bool                    m_isEnabled;
		eMode					m_mode;
        eType                   m_type;

        // ** struct sScalarCurves
        struct sScalarCurves {
            Curve<float> curves[2];
        };

        // ** struct sColorCurves
        struct sColorCurves {
            Curve<Rgb>	curves[2];
        };

        union {
            sScalarCurves*	m_scalar;
            sColorCurves*	m_color;
        };
    };

} // namespace Fx

DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_Parameter_H__	*/