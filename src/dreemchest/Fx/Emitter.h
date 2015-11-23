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

#ifndef __DC_Fx_Emitter_H__
#define __DC_Fx_Emitter_H__

#include "Fx.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

	// ** class EmitterModel
	class EmitterModel : public RefCounted {

		DC_DECLARE_IS( EmitterModel, Emitter, this );

	public:

                            EmitterModel( void );
                            ~EmitterModel( void );

		const Vec2&			position( void ) const;
		void				setPosition( const Vec2& value );
		float				duration( void ) const;
		void				setDuration( float value );
		const char*			name( void ) const;
		void				setName( const char *value );
		bool				isLooped( void ) const;
		void				setLooped( bool value );
		int					totalParticles( void ) const;
		ParticleModel*		particles( int index ) const;
		Zone*				zone( void ) const { return m_zone; }

		void				removeParticles( ParticleModel *particle );
		ParticleModel*		createParticles( void );

		Emitter*			createInstance( void ) const;

	private:

		ParticleModelArray	m_particles;
		Vec2				m_position;
		Zone*				m_zone;
		float				m_duration;
		bool				m_isLooped;
		std::string			m_name;
	};

	// ** class Emitter
	class Emitter : public RefCounted {
	public:

							Emitter( const EmitterModel *model );
							~Emitter( void );

		int					aliveCount( void ) const;
		bool				hasEnded( void ) const;

		int					update( float dt, const Vec2& position );
        void                warmUp( float dt, const Vec2& position );
		void				render( dcBatchRenderer renderer );

	private:

		bool				updateTime( float dt );

	private:

		const EmitterModel*	m_model;
		ParticlesArray		m_particles;
		float				m_time;
		int					m_aliveCount;
	};

} // namespace Fx

DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_Emitter_H__	*/