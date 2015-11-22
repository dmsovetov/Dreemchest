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

#include    "ParticleSystem.h"

#include	"Emitter.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

DC_BEGIN_DREEMCHEST

namespace Particles {

// ------------------------------------------ ParticleSystemModel ------------------------------------------ //

// ** ParticleSystemModel::ParticleSystemModel
ParticleSystemModel::ParticleSystemModel( void )
{
}

ParticleSystemModel::~ParticleSystemModel( void )
{
	for( int i = 0, n = totalEmitters(); i < n; i++ ) {
		delete m_emitters[i];
	}
}

// ** ParticleSystemModel::totalEmitters
int ParticleSystemModel::totalEmitters( void ) const
{
	return ( int )m_emitters.size();
}

// ** ParticleSystemModel::emitter
EmitterModel* ParticleSystemModel::emitter( int index ) const
{
	DC_BREAK_IF( index < 0 || index >= totalEmitters() );
	return m_emitters[index];
}

// ** ParticleSystemModel::removeEmitter
void ParticleSystemModel::removeEmitter( EmitterModel *emitter )
{
	EmitterModelArray::iterator i = std::find( m_emitters.begin(), m_emitters.end(), emitter );
	if( i == m_emitters.end() ) {
		DC_BREAK;
		return;
	}

	delete *i;
	m_emitters.erase( i );
}

// ** ParticleSystemModel::createEmitter
EmitterModel* ParticleSystemModel::createEmitter( void )
{
	EmitterModel *emitter = DC_NEW EmitterModel;
	m_emitters.push_back( emitter );

	return emitter;
}

// ** ParticleSystemModel::createInstance
ParticleSystem* ParticleSystemModel::createInstance( void ) const
{
	return DC_NEW ParticleSystem( this );
}

// ** ParticleSystem::ParticleSystem
ParticleSystem::ParticleSystem( const ParticleSystemModel *model ) : m_model( model ), m_position( 0, 0 ), m_aliveCount( 0 )
{
	for( int i = 0, n = m_model->totalEmitters(); i < n; i++ ) {
		m_emitters.push_back( m_model->emitter( i )->createInstance() );
	}
}

ParticleSystem::~ParticleSystem( void )
{
	for( int i = 0, n = ( int )m_emitters.size(); i < n; i++ ) {
		delete m_emitters[i];
	}
}

// ** ParticleSystem::position
const Vec2& ParticleSystem::position( void ) const
{
	return m_position;
}

// ** ParticleSystem::setPosition
void ParticleSystem::setPosition( const Vec2& value )
{
	m_position = value;
}

// ** ParticleSystem::timeScale
float ParticleSystem::timeScale( void ) const
{
	return m_timeScale;
}

// ** ParticleSystem::setTimeScale
void ParticleSystem::setTimeScale( float value )
{
	m_timeScale = value;
}

// ** ParticleSystem::aliveCount
int ParticleSystem::aliveCount( void ) const
{
	return m_aliveCount;
}

// ** ParticleSystem::hasEnded
bool ParticleSystem::hasEnded( void ) const
{
	for( int i = 0, n = ( int )m_emitters.size(); i < n; i++ ) {
		if( !m_emitters[i]->hasEnded() ) {
			return false;
		}
	}

	return true;
}

// ** ParticleSystem::update
int ParticleSystem::update( float dt )
{
	m_aliveCount = 0;

	for( int i = 0, n = ( int )m_emitters.size(); i < n; i++ ) {
		m_aliveCount += m_emitters[i]->update( dt, m_position );
	}

	return m_aliveCount;
}

// ** ParticleSystem::warmUp
void ParticleSystem::warmUp( float dt )
{
	for( int i = 0, n = ( int )m_emitters.size(); i < n; i++ ) {
		m_emitters[i]->warmUp( dt, m_position );
	}
}

// ** ParticleSystem::render
void ParticleSystem::render( dcBatchRenderer renderer )
{
	for( int i = 0, n = ( int )m_emitters.size(); i < n; i++ ) {
		m_emitters[i]->render( renderer );
	}
}

} // namespace Particles

DC_END_DREEMCHEST
