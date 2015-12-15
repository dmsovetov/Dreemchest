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

#ifndef __DC_Fx_ParticleSystem_H__
#define __DC_Fx_ParticleSystem_H__

#include "Fx.h"
#include "Parameter.h"

#include <Io/JsonLoader.h>

DC_BEGIN_DREEMCHEST

namespace Fx {

	//! Root particle system type. Contains an array of particle emitters.
	class ParticleSystem : public RefCounted {
	public:


		//! Returns the total number of emitters inside this particle system.
		s32							emitterCount( void ) const;

		//! Returns an emitter by index.
		EmitterWPtr					emitter( s32 index ) const;

		//! Removes an emitter.
		void						removeEmitter( const EmitterWPtr& emitter );

		//! Adds a new emitter.
		EmitterWPtr					addEmitter( void );

		//! Creates a new instance if a particle system.
		ParticleSystemInstancePtr	createInstance( IMaterialFactoryWPtr materialFactory ) const;

		//! Creates the ParticleSystem instance and loads it from file.
		static ParticleSystemPtr	createFromFile( const String& fileName, f32 scalingFactor = 1.0f );

		//! Creates the ParticleSystem instance and loads it from JSON string.
		static ParticleSystemPtr	createFromJson( const String& json, f32 scalingFactor = 1.0f );

	private:

		EmittersArray				m_emitters;	//!< All particle emitters.
	};

	//! A single particle system instance.
	class ParticleSystemInstance : public RefCounted {
	friend class ParticleSystem;
	public:

		//! Returns the particle system instance position.
		const Vec3&					position( void ) const;

		//! Sets the particle system instance position.
		void						setPosition( const Vec3& value );

		//! Returns the time scaling factor.
		f32							timeScale( void ) const;

		//! Sets the time scaling factor.
		void						setTimeScale( f32 value );

		//! Returns the total number of alive particles.
		s32							aliveCount( void ) const;

		//! Returns the total number of emitters.
		s32							emitterCount( void ) const;

		//! Returns the emitter instance.
		EmitterInstanceWPtr			emitter( s32 index ) const;

		//! Returns the particle system bounding box.
		Bounds						bounds( void ) const;

		//! Returns true if the particle system has ended the playback.
		bool						hasEnded( void ) const;

		//! Performs the particle system update.
		s32							update( f32 dt );

		//! Performs the particle system warmup with a specified time delta.
        void                        warmUp( f32 dt = 0.1f );

	private:

									//! Constructs ParticleSystemInstance instance.
									ParticleSystemInstance( IMaterialFactoryWPtr materialFactory, ParticleSystemWPtr particleSystem );

	private:

		ParticleSystemWPtr			m_particleSystem;	//!< Parent particle system.
		EmitterInstancesArray		m_emitters;			//!< Array of emitter instances.
		Vec3						m_position;			//!< Current instance position.
		f32							m_timeScale;		//!< The time scaling factor.
		s32							m_aliveCount;		//!< The total number of alive particles.
	};

#ifdef HAVE_JSON

	//! Loads the particle system from JSON file.
	class JsonParticleSystemLoader : public io::JsonLoaderBase {
	public:

		//! Loads the particle system from string.
		bool						load( ParticleSystemWPtr particleSystem, const String& json, f32 scalingFactor );

	private:

		//! Reads particles from JSON object.
		bool						readParticles( const Json::Value& object );

		//! Reads emitter from JSON object.
		bool						readEmitter( const Json::Value& object );

		//! Reads renderer from JSON object.
		bool						readRenderer( const Json::Value& object );

		//! Reads the color module from JSON object.
		bool						readColor( ParticlesWPtr particles, const Json::Value& object );

		//! Reads the emission module from JSON object.
		bool						readEmission( ParticlesWPtr particles, const Json::Value& object );

		//! Reads the size module from JSON object.
		bool						readSize( ParticlesWPtr particles, const Json::Value& object );

		//! Reads the rotation module from JSON object.
		bool						readAngularVelocity( ParticlesWPtr particles, const Json::Value& object );

		//! Reads the acceleration module from JSON object.
		bool						readAcceleration( ParticlesWPtr particles, const Json::Value& object );

		//! Reads the velocity module from JSON object.
		bool						readVelocity( ParticlesWPtr particles, const Json::Value& object );

		//! Reads the initial module from JSON object.
		bool						readInitial( ParticlesWPtr particles, const Json::Value& object );

		//! Reads the color parameter from JSON object.
		void						readColorParameter( RgbParameter& parameter, const Json::Value& object );

		//! Reads the scalar parameter from JSON object.
		void						readScalarParameter( FloatParameter& parameter, const Json::Value& object, f32 scalingFactor = 1.0f );

	private:

		//! Particle module loader
		typedef cClosure<bool(ParticlesWPtr, const Json::Value&)>	ModuleLoader;

		//! Container type to store particle module loaders.
		typedef Map<String, ModuleLoader>		ModuleLoaders;

		//! Container type to store emitters.
		typedef Map<String, EmitterWPtr>		Emitters;

		Emitters					m_emitters;			//!< Loaded emitters.
		ModuleLoaders				m_moduleLoaders;	//!< Available module loaders.
		ParticleSystemWPtr			m_particleSystem;	//!< Particle system to be parsed.
		f32							m_scalingFactor;	//!< The particle system scaling factor.
	};

#endif	/*	HAVE_JSON	*/

} // namespace Fx

DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_ParticleSystem_H__	*/