//
//  Filename:   Modifiers.h
//	Created:	28:05:2011   18:32

#ifndef		__DC_ParticleModifiers_H__
#define		__DC_ParticleModifiers_H__

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include    "../zone/ZoneContainer.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

    // ** class Modifier
    class Modifier : public ZoneContainer {

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

} // namespace particles
    
} // namespace dreemchest

#endif		/*	!__DC_ParticleModifiers_H__	*/