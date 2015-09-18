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

#include "RenderPass.h"
#include "Emitters/RopEmitter.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderPassBase::addEmitter
void RenderPassBase::addEmitter( RopEmitterBase* emitter )
{
	m_emitters.push_back( emitter );
}

// ** RenderPassBase::begin
void RenderPassBase::begin( RenderingContextPtr context, const Camera& camera, const Transform& transform, const Matrix4& viewProjection )
{
	Rvm& rvm = *context->rvm().get();
	ShaderCache& shaders = *context->shaders().get();

	// Save camera & transform references.
	m_camera = &camera;
	m_transform = &transform;

	// Reset the RVM before rendering
	rvm.reset();

	// Peform basic pass setup
	rvm.setViewProjection( viewProjection );
	rvm.setDefaultShader( shaders.shaderById( ShaderCache::Null ) );

	// Setup rendering states
	setup( rvm, shaders, viewProjection );
}

// ** RenderPassBase::perform
void RenderPassBase::render( RenderingContextPtr context )
{
	Rvm& rvm = *context->rvm().get();
	ShaderCache& shaders = *context->shaders().get();

	// Emit render operations
	emit( rvm, shaders );
}

// ** RenderPassBase::end
void RenderPassBase::end( RenderingContextPtr context )
{
	// Flush emitted render operations.
	context->rvm()->flush();
}

// ** RenderPassBase::setup
void RenderPassBase::setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection )
{
	
}

// ** RenderPassBase::emit
void RenderPassBase::emit( Rvm& rvm, ShaderCache& shaders )
{
	for( u32 i = 0, n = ( u32 )m_emitters.size(); i < n; i++ ) {
		m_emitters[i]->emit( rvm, shaders, *m_camera, *m_transform );
	}
}

} // namespace Scene

DC_END_DREEMCHEST