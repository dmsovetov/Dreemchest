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

#ifndef __DC_Fx_ParticleBundle_H__
#define __DC_Fx_ParticleBundle_H__

#include "Parameter.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

	//! A single particle item.
	struct Particle {
		enum { MaxSnapshots = 64 };

		Vec3    m_position;			//!< Particle position.
		f32		m_rotation;			//!< Particle rotation.
		f32		m_direction;		//!< Particle movement direction.

		struct {
			Vec3    velocity;		//!< The force applied to a particle.
			Vec3	acceleration;	//!< Particle acceleration due to applied force.
		} m_force;

		struct {
			f32		velocity;		//!< Particle angular velocity.
			f32		torque;			//!< Particle torque.
		} m_angular;

		struct {
			f32		current;		//!< Particle life left in seconds.
			f32		initial;		//!< Particle life fade.
		} m_life;

		struct {
            struct {
                Rgb     rgb;
                f32		alpha;
            } initial;
            
            struct {
                Rgb     rgb;
                f32		alpha;
            } current;
		} m_color;

		struct {
			f32		initial;
			f32		current;
		} m_size;

		struct {
			Vec3    pos;
			Rgb     color;
			f32		alpha;
			f32		size;
		} m_snapshots[MaxSnapshots];
	};

	//! Particle bursts structure.
	struct ParticleBurst {
		f32					time;			//!< The burst time.
		s32					count;			//!< The number of particles to burst.
		s32					lastIteration;	//!< Last iteration number the burst was emitted.
	};

	//! Particles contains an array of particles and a set of simulation parameters.
	class Particles : public RefCounted {
	friend class Emitter;
	public:

		//! Available particle scalar parameters.
		enum ScalarParameter {
			Emission,
			Life,
			Direction,
			Size,
			SizeOverLife,
			Transparency,
			TransparencyOverLife,
			Speed,
			Gravity,
			AccelerationX,
			AccelerationY,
			AccelerationZ,
			VelocityXOverLife,
			VelocityYOverLife,
			VelocityZOverLife,
			AccelerationXOverLife,
			AccelerationYOverLife,
			AccelerationZOverLife,
			AngularVelocity,
			AngularVelocityOverLife,
			Torque,
			TorqueOverLife,
			Rotation,

			TotalScalarParameters
		};

        //! Available particle color parameters.
        enum ColorParameter {
            Color,
            ColorOverLife,

            TotalColorParameters
        };

	public:

		//! Returns the maximum number of particles.
		s32						count( void ) const;

		//! Sets the maximum number of particles.
		void					setCount( s32 value );

		//! Returns the particle rendering mode.
		RenderingMode			renderingMode( void ) const;

		//! Sets the particle rendering mode.
		void					setRenderingMode( RenderingMode value );

		//! Returns the particle blending mode.
		BlendingMode			blendingMode( void ) const;

		//! Sets the particle blending mode.
		void					setBlendingMode( BlendingMode value );

		//! Returns the particle material name.
		const String&			material( void ) const;

		//! Sets the particle material name.
		void					setMaterial( const String& value );

		//! Returns parent emitter.
		EmitterWPtr				emitter( void ) const;

		//! Adds burst item.
		void					addBurst( f32 time, s32 count );

		//! Returns the total number of bursts.
		s32						burstCount( void ) const;

		//! Returns the burst by index.
		const ParticleBurst&	burst( s32 index ) const;

		//! Returns the particles name.
		const String&			name( void ) const;

		//! Sets the particles name.
		void					setName( const String& value );

		//! Returns the emission rate at specified time
        f32						emission( f32 scalar ) const;

		//! Returns the maximum number of snapshots to save.
		s32						maxSnapshots( void ) const;

		//! Sets the maximum number of snapshots to save.
		void					setMaxSnapshots( s32 value );

		//! Returns the scalar parameter.
		const FloatParameter&	scalarParameter( ScalarParameter parameter ) const;
		FloatParameter&			scalarParameter( ScalarParameter parameter );

		//! Returns the color parameter.
		const RgbParameter&		colorParameter( ColorParameter parameter ) const;
		RgbParameter&			colorParameter( ColorParameter parameter );

		//! Updates the group of particles.
		s32                     update( Particle* items, s32 itemCount, f32 dt, Bounds* bounds = NULL ) const;

		//! Initializes a new group of particles.
		s32                     init( const ZoneWPtr& zone, Particle *items, const Vec3& pos, s32 itemCount, f32 scalar ) const;

		//! Saves the particles snapshots.
		void					savePaths( Particle* items, s32 itemCount ) const;

		ParticlesInstancePtr	createInstance( IMaterialFactoryWPtr materialFactory ) const;

	private:

								Particles( EmitterWPtr emitter );

		//! Initializes the particle snapshots.
		void					initSnapshots( Particle& particle, s32 count ) const;

		//! Returns the total number of snapshots to save.
		s32						snapshotsToSave( void ) const;

	private:

		EmitterWPtr				m_emitter;							//!< Parent emitter instance.
		BlendingMode			m_blendingMode;						//!< Particle blending mode.
		RenderingMode			m_renderingMode;					//!< Particle rendering mode.
		s32						m_count;							//!< The maximum number of particles.
		s32						m_maxSnapshots;						//!< The maximum number of particle snapshots to save.
		String					m_name;								//!< Particles name.
		String					m_material;							//!< Particles material name.
		Array<ParticleBurst>	m_bursts;							//!< Particle bursts.
        
        FloatParameter          m_scalar[TotalScalarParameters];	//!< Particle scalar parameters.
        RgbParameter            m_color[TotalColorParameters];		//!< Particle color parameters.
	};

	//! The particles instance.
	class ParticlesInstance : public RefCounted {
	friend class Particles;
	public:

		//! Returns the total number of alive particles.
		s32						aliveCount( void ) const;

		//! Returns the particle items.
		const Particle*			items( void ) const;

		//! Returns the particle blending mode.
		BlendingMode			blendingMode( void ) const;

		//! Returns the particles bounding box.
		const Bounds&			bounds( void ) const;

		//! Returns the particle rendering mode.
		RenderingMode			renderingMode( void ) const;

		//! Returns the particles material.
		IMaterialWPtr			material( void ) const;

		//! Sets the particles material.
		void					setMaterial( const IMaterialWPtr& value );

		//! Updates the particles.
		s32						update( s32 iteration, const ZoneWPtr& zone, f32 dt, const Vec3& position, f32 scalar, bool noEmission );

	private:

								//! Constructs the ParticlesInstance instance.
								ParticlesInstance( IMaterialFactoryWPtr materialFactory, ParticlesWPtr particles );

	private:

		ParticlesWPtr			m_particles;	//!< Parent particles.
		Array<Particle>			m_items;		//!< An array of particle items.
		s32						m_aliveCount;	//!< The total number of alive particles.
		Bounds					m_bounds;		//!< Particles bounding box.
		f32						m_time;			//!< Current particles time.
		Array<ParticleBurst>	m_bursts;		//!< Available particle bursts.
		IMaterialPtr			m_material;		//!< Particle material reference.
		f32						m_emissionTime;
		f32						m_snapshotTime;
	};

} // namespace Fx
    
DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_ParticleBundle_H__	*/