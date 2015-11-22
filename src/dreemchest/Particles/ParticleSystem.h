//
//  Filename:   ParticleSystem.h
//	Created:	28:05:2011   18:32

#ifndef		__DC_ParticleSystem_H__
#define		__DC_ParticleSystem_H__

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include    "modifier/ModifierContainer.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

	// ** class ParticleSystemModel
	class ParticleSystemModel : public ModifierContainer {

		DC_DECLARE_IS( ParticleSystemModel, ParticleSystem, this );

	public:

							ParticleSystemModel( void );
							~ParticleSystemModel( void );

		int					totalEmitters( void ) const;
		EmitterModel*		emitter( int index ) const;

		void				removeEmitter( EmitterModel *emitter );
		EmitterModel*		createEmitter( void );

		ParticleSystem*		createInstance( void ) const;

	private:

		EmitterModelArray	m_emitters;
	};

	// ** class ParticleSystem
	class ParticleSystem {
	public:

									ParticleSystem( const ParticleSystemModel *model );
									~ParticleSystem( void );

		const vec2&					position( void ) const;
		void						setPosition( const vec2& value );
		float						timeScale( void ) const;
		void						setTimeScale( float value );
		int							aliveCount( void ) const;
		bool						hasEnded( void ) const;

		int							update( float dt );
		void						render( dcBatchRenderer renderer );
        void                        warmUp( float dt = 0.1f );

	private:

		const ParticleSystemModel*	m_model;
		EmitterArray				m_emitters;
		vec2						m_position;
		float						m_timeScale;
		int							m_aliveCount;
	};

} // namespace particles

} // namespace dreemchest

#endif		/*	!__DC_ParticleSystem_H__	*/