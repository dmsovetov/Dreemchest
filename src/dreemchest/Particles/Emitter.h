//
//  Filename:   Emitter.h
//	Created:	28:05:2011   18:32

#ifndef		__DC_Emitter_H__
#define		__DC_Emitter_H__

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include	"modifier/ModifierContainer.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

	// ** class EmitterModel
	class EmitterModel : public ModifierContainer {

		DC_DECLARE_IS( EmitterModel, Emitter, this );

	public:

                            EmitterModel( void );
                            ~EmitterModel( void );

		const vec2&			position( void ) const;
		void				setPosition( const vec2& value );
		float				duration( void ) const;
		void				setDuration( float value );
		const char*			name( void ) const;
		void				setName( const char *value );
		bool				isLooped( void ) const;
		void				setLooped( bool value );
		int					totalParticles( void ) const;
		ParticleModel*		particles( int index ) const;

		void				removeParticles( ParticleModel *particle );
		ParticleModel*		createParticles( void );

		Emitter*			createInstance( void ) const;

	private:

		ParticleModelArray	m_particles;
		vec2				m_position;
		float				m_duration;
		bool				m_isLooped;
		std::string			m_name;
	};

	// ** class Emitter
	class Emitter {
	public:

							Emitter( const EmitterModel *model );
							~Emitter( void );

		int					aliveCount( void ) const;
		bool				hasEnded( void ) const;

		int					update( float dt, const vec2& position );
        void                warmUp( float dt, const vec2& position );
		void				render( dcBatchRenderer renderer );

	private:

		bool				updateTime( float dt );

	private:

		const EmitterModel*	m_model;
		ParticlesArray		m_particles;
		float				m_time;
		int					m_aliveCount;
	};

} // namespace particles

} // namespace dreemchest

#endif		/*	!__DC_Emitter_H__	*/