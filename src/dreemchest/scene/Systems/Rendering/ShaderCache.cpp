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

#include "ShaderCache.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** ShaderCache::ShaderCache
ShaderCache::ShaderCache( const Renderer::HalPtr& hal ) : m_hal( hal )
{

}

// ** ShaderCache::findByName
const Renderer::ShaderPtr& ShaderCache::shaderById( ShaderId id )
{
	if( m_shaders[id].valid() ) {
		return m_shaders[id];
	}

	log::msg( "Loading shader '%s'...\n", s_shaderName[id] );

	const Code& code = s_shaderCode[id];
	m_shaders[id] = m_hal->createShader( code.m_vertex, code.m_fragment );
	DC_BREAK_IF( !m_shaders[id].valid() )

	return m_shaders[id];
}

// ** ShaderCache::s_shaderName
CString ShaderCache::s_shaderName[TotalShaders] = {
	  "Pink"
	, "ConstantColor"
	, "Diffuse"
};

// ** ShaderCache::s_shaderCode
ShaderCache::Code ShaderCache::s_shaderCode[TotalShaders] = {
	// ** Pink
	{
		CODE(
			uniform mat4 u_vp, u_transform;

			void main() {
				gl_Position = u_vp * u_transform * gl_Vertex;
			}
		),
		CODE(
			void main() {
				gl_FragColor = vec4( 1.0, 0.0, 1.0, 1.0 );
			}
		)
	},

	// ** Constant color
	{
		CODE(
			uniform mat4 u_vp, u_transform;

			void main() {
				gl_Position = u_vp * u_transform * gl_Vertex;
			}
		),
		CODE(
			uniform vec4 u_color;

			void main() {
				gl_FragColor = u_color;
			}
		)
	},

	// ** Diffuse
	{
		CODE(
			uniform mat4 u_vp, u_transform;

			varying vec2 v_uv0;

			void main() {
				v_uv0		= gl_MultiTexCoord0.xy;
				gl_Position = u_vp * u_transform * gl_Vertex;
			}
		),
		CODE(
			uniform sampler2D u_tex0;
			uniform vec4	  u_clr0;

			varying vec2	  v_uv0;

			void main() {
				gl_FragColor = texture2D( u_tex0, v_uv0 ) * u_clr0;
			}
		)
	}
};

} // namespace Scene

DC_END_DREEMCHEST