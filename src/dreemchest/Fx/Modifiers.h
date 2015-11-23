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

#ifndef	__DC_Fx_ParticleModifiers_H__
#define	__DC_Fx_ParticleModifiers_H__

#include "Parameter.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

    // ** class Modifier
    class Modifier : public RefCounted {

		DC_DECLARE_IS( Modifier, Modifier, this );

    public:

							Modifier( void );
        virtual				~Modifier( void ) {}

		static Modifier*	create( ModifierType type );

		float				duration( void ) const;
		void				setDuration( float value );
		bool				isEnabled( void ) const;
		void				setEnabled( bool value );

        virtual void		apply( float dt, float scalar, Particles *particles ) = 0;

	protected:

		float				m_duration;
		bool				m_isEnabled;
    };

    // ** class Friction
    class Friction : public Modifier {
    public:

                            Friction( void );

        // ** ParticleModifier
        virtual void        apply( float dt, float scalar, Particles *particles );

    private:

		Parameter			m_value;
    };

    // ** class Force
    class Force : public Modifier {
    public:

                            Force( void );

        // ** ParticleModifier
        virtual void        apply( float dt, float scalar, Particles *particles );

    private:

        Parameter			m_x, m_y, m_z;
    };

    // ** class ForceField
    class ForceField : public Modifier {
    public:

                            ForceField( void );

        // ** ParticleModifier
        virtual void        apply( float dt, float scalar, Particles *particles );

        // ** ForceField
        bool                isQuadratic( void ) const;
        void                setQuadratic( bool value );

    private:

		Parameter			m_x, m_y, m_z;
		Parameter			m_force;
		Parameter			m_radius;
        bool                m_isQuadratic;
    };

} // namespace Fx
    
DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_ParticleModifiers_H__	*/