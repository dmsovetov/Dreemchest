//
//  Filename:   Parameter.h
//	Created:	28:05:2011   18:32

#ifndef		__DC_Parameter_H__
#define		__DC_Parameter_H__

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include	"../ParticleModelNode.h"

#define SampleParameter( parameter, default )  ((parameter) ? (parameter)->sample( scalar, default ) : (default))
#define SampleKoeficient( parameter, default ) SampleParameter( parameter, default * 100.0f ) * 0.01f

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

    typedef Curve<float>    CurveScalar;
    typedef Curve<Rgb>      CurveRgb;

    // ** class Parameter
    class Parameter : public IParticleModelNode {
        
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
        Rgb                     sample( float scalar, const Rgb& defaultValue = White ) const;

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

        union {;
            sScalarCurves*	m_scalar;
            sColorCurves*	m_color;
        };
    };

} // namespace particles

} // namespace dreemchest

#endif		/*	!__DC_Parameter_H__	*/