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

#include "ParticleSystem.h"

#include "Emitter.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

// ------------------------------------------ ParticleSystem ------------------------------------------ //

// ** ParticleSystem::emitterCount
s32 ParticleSystem::emitterCount( void ) const
{
	return static_cast<s32>( m_emitters.size() );
}

// ** ParticleSystem::emitter
EmitterWPtr ParticleSystem::emitter( s32 index ) const
{
	DC_BREAK_IF( index < 0 || index >= emitterCount() );
	return m_emitters[index];
}

// ** ParticleSystem::removeEmitter
void ParticleSystem::removeEmitter( const EmitterWPtr& emitter )
{
	EmittersArray::iterator i = std::find( m_emitters.begin(), m_emitters.end(), emitter );

	if( i == m_emitters.end() ) {
		DC_BREAK;
		return;
	}

	m_emitters.erase( i );
}

// ** ParticleSystem::addEmitter
EmitterWPtr ParticleSystem::addEmitter( void )
{
	Emitter* emitter = DC_NEW Emitter;
	m_emitters.push_back( emitter );

	return emitter;
}

// ** ParticleSystem::createInstance
ParticleSystemInstancePtr ParticleSystem::createInstance( void ) const
{
	return ParticleSystemInstancePtr( DC_NEW ParticleSystemInstance( const_cast<ParticleSystem*>( this ) ) );
}

// ---------------------------------------- ParticleSystemInstance ---------------------------------------- //

// ** ParticleSystemInstance::ParticleSystemInstance
ParticleSystemInstance::ParticleSystemInstance( ParticleSystemWPtr particleSystem ) : m_particleSystem( particleSystem ), m_position( 0.0f, 0.0f, 0.0f ), m_aliveCount( 0 ), m_timeScale( 1.0f )
{
	for( int i = 0, n = m_particleSystem->emitterCount(); i < n; i++ ) {
		m_emitters.push_back( m_particleSystem->emitter( i )->createInstance() );
	}
}

// ** ParticleSystemInstance::position
const Vec3& ParticleSystemInstance::position( void ) const
{
	return m_position;
}

// ** ParticleSystemInstance::setPosition
void ParticleSystemInstance::setPosition( const Vec3& value )
{
	m_position = value;
}

// ** ParticleSystemInstance::timeScale
f32 ParticleSystemInstance::timeScale( void ) const
{
	return m_timeScale;
}

// ** ParticleSystemInstance::setTimeScale
void ParticleSystemInstance::setTimeScale( f32 value )
{
	m_timeScale = value;
}

// ** ParticleSystemInstance::aliveCount
s32 ParticleSystemInstance::aliveCount( void ) const
{
	return m_aliveCount;
}

// ** ParticleSystemInstance::emitterCount
s32 ParticleSystemInstance::emitterCount( void ) const
{
	return static_cast<s32>( m_emitters.size() );
}

// ** ParticleSystemInstance::emitter
EmitterInstanceWPtr ParticleSystemInstance::emitter( s32 index ) const
{
	DC_BREAK_IF( index < 0 || index >= emitterCount() );
	return m_emitters[index];
}

// ** ParticleSystemInstance::hasEnded
bool ParticleSystemInstance::hasEnded( void ) const
{
	for( u32 i = 0, n = ( u32 )m_emitters.size(); i < n; i++ ) {
		if( !m_emitters[i]->hasEnded() ) {
			return false;
		}
	}

	return true;
}

// ** ParticleSystemInstance::update
s32 ParticleSystemInstance::update( f32 dt )
{
	m_aliveCount = 0;

	for( u32 i = 0, n = ( u32 )m_emitters.size(); i < n; i++ ) {
		m_aliveCount += m_emitters[i]->update( dt * m_timeScale, m_position );
	}

	return m_aliveCount;
}

// ** ParticleSystemInstance::warmUp
void ParticleSystemInstance::warmUp( f32 dt )
{
	for( u32 i = 0, n = ( u32 )m_emitters.size(); i < n; i++ ) {
		m_emitters[i]->warmUp( dt, m_position );
	}
}

} // namespace Fx

DC_END_DREEMCHEST
