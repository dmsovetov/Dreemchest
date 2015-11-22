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

#ifndef __DC_Particles_ParticleBundle_H__
#define __DC_Particles_ParticleBundle_H__

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

	// ** struct sParticle
	struct sParticle {
		enum { MaxSnapshots = 64 };

		vec2    m_position;
		float   m_rotation;
		float   m_direction;

		struct {
			vec2    velocity;
		} m_force;

		struct {
			float   velocity;
		} m_linear;

		struct {
			float   velocity;
			float   torque;
		} m_angular;

		struct {
			float   age;
			float   fade;
		} m_life;

		struct {
            struct {
                Rgb     rgb;
                float   alpha;
            } initial;
            
            struct {
                Rgb     rgb;
                float   alpha;
            } current;
		} m_color;

		struct {
			float   initial;
			float   current;
		} m_size;

		struct {
			vec2    pos;
			Rgb     color;
			float	alpha;
			float   size;
		} m_snapshots[MaxSnapshots];
	};

	// ** class ParticleModel
	class ParticleModel : public ModifierContainer {

		DC_DECLARE_IS( ParticleModel, Particles, this );

		// ** enum eScalarParameter
		enum eScalarParameter {
			Emission,
			Life,
			Direction,
			Size,
			SizeOverLife,
			Transparency,
			TransparencyOverLife,
			Velocity,
			VelocityOverLife,
			AngularVelocity,
			AngularVelocityOverLife,
			Torque,
			TorqueOverLife,
			Rotation,

			TotalScalarParameters
		};

        // ** enum eColorParameter
        enum eColorParameter {
            Color,
            ColorOverLife,

            TotalColorParameters
        };

	public:

								ParticleModel( void );
								~ParticleModel( void );

		int						count( void ) const;
		void					setCount( int value );
		dcTextureAsset			texture( void ) const;
		void					setTexture( dcTextureAsset value );
		RendererType			rendererType( void ) const;
		void					setRendererType( RendererType value );
		Renderer*				renderer( void ) const;
		BlendMode				blendMode( void ) const;
		void					setBlendMode( BlendMode value );
		const char*				name( void ) const;
		void					setName( const char *value );
        float                   emission( float scalar ) const;
		int						maxSnapshots( void ) const;
		void					setMaxSnapshots( int value );

		int                     update( sParticle *items, int itemCount, float dt, vec2 *min = NULL, vec2 *max = NULL ) const;
		int                     init( Zone *zone, sParticle *items, const vec2& pos, int itemCount, float scalar ) const;
		void					savePaths( sParticle* items, int itemCount ) const;
		void					render( dcBatchRenderer renderer, const sParticle *items, int itemCount ) const;

		Particles*				createInstance( void ) const;

	private:

		void					initSnapshots( sParticle& particle, int count ) const;
		int						snapshotsToSave( void ) const;

	private:

		BlendMode				m_blendMode;
		Renderer*				m_particleRenderer;
		dcTextureAsset			m_texture;
		int						m_count;
		int						m_maxSnapshots;
		std::string				m_name;
        
        Parameter               m_scalar[TotalScalarParameters];
        Parameter               m_color[TotalColorParameters];
	};

	// ** class Particles
	class Particles {
	public:

								Particles( const ParticleModel *model );
								~Particles( void );

		int						aliveCount( void ) const;

		int						update( Zone *zone, float dt, const vec2& position, float scalar, bool noEmission );
		void					render( dcBatchRenderer renderer );

	private:

		const ParticleModel*	m_model;
		sParticle*				m_particles;
		int						m_count;
		int						m_aliveCount;
		vec2					m_min, m_max;
		float					m_time, m_emissionTime, m_snapshotTime;
	};

} // namespace particles
    
} // namespace dreemchest

#endif	/*	!__DC_Particles_ParticleBundle_H__	*/