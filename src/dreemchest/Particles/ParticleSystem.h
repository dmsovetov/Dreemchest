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

#ifndef __DC_Particles_ParticleSystem_H__
#define __DC_Particles_ParticleSystem_H__

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

DC_BEGIN_DREEMCHEST

namespace Particles {

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

		const Vec2&					position( void ) const;
		void						setPosition( const Vec2& value );
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
		Vec2						m_position;
		float						m_timeScale;
		int							m_aliveCount;
	};

} // namespace Particles

DC_END_DREEMCHEST

#endif		/*	!__DC_Particles_ParticleSystem_H__	*/