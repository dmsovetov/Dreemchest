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
ShaderCache::ShaderCache( const String& folder, const Renderer::HalPtr& hal ) : m_hal( hal )
{
	String vertex, fragment;

	if( loadFromFile( folder + "Unlit.shader", vertex, fragment ) ) {
		m_material[Material::Unlit] = DC_NEW Shader( vertex, fragment );
		m_material[Material::Unlit]->addFeature( Material::FeatureDiffuse, "USE_DIFFUSE_MAP" );
	}

	if( loadFromFile( folder + "Ambient.shader", vertex, fragment ) ) {
		m_material[Material::Ambient] = DC_NEW Shader( vertex, fragment );
		m_material[Material::Ambient]->addFeature( Material::FeatureDiffuse, "USE_DIFFUSE_MAP" );		
	}

	if( loadFromFile( folder + "Phong.shader", vertex, fragment ) ) {
		m_material[Material::Phong] = DC_NEW Shader( vertex, fragment );
		m_material[Material::Phong]->addFeature( Material::FeatureDiffuse, "USE_DIFFUSE_MAP" );
	}
}

// ** ShaderCache::shaderById
const Renderer::ShaderPtr& ShaderCache::shaderById( ShaderId id )
{
	if( m_shaders[id].valid() ) {
		return m_shaders[id];
	}

	LogVerbose( "Loading shader '%s'...\n", s_shaderName[id] );

	const Code& code = s_shaderCode[id];
	m_shaders[id] = m_hal->createShader( code.m_vertex, code.m_fragment );
	DC_BREAK_IF( !m_shaders[id].valid() )

	return m_shaders[id];
}

// ** ShaderCache::shaderById
const Renderer::ShaderPtr& ShaderCache::materialShader( Material::Model model, u32 features )
{
	if( !m_material[model].valid() ) {
		return m_shaders[Null];
	}

	if( !m_material[model]->isLoaded( features ) ) {
		LogVerbose( "Loading '%s' material shader permutation %d...\n", s_modelName[model], features );
	}
	
	return m_material[model]->permutation( m_hal, features );
}

// ** ShaderCache::loadFromFile
bool ShaderCache::loadFromFile( const String& fileName, String& vertex, String& fragment )
{
	static CString vertexShaderMarker   = "[VertexShader]";
	static CString fragmentShaderMarker = "[FragmentShader]";

	// Read the code from a text file
	String code = Io::DiskFileSystem::readTextFile( fileName );

	// Extract vertex/fragment shader code blocks
	u32 vertexBegin = code.find( vertexShaderMarker );
	u32 fragmentBegin = code.find( fragmentShaderMarker );

	if( vertexBegin == String::npos && fragmentBegin == String::npos ) {
		return false;
	}

	if( vertexBegin != String::npos ) {
		u32 vertexCodeStart = vertexBegin + strlen( vertexShaderMarker );
		vertex = code.substr( vertexCodeStart, fragmentBegin > vertexBegin ? fragmentBegin - vertexCodeStart : String::npos );
	}

	if( fragmentBegin != String::npos ) {
		u32 fragmentCodeStart = fragmentBegin + strlen( fragmentShaderMarker );
		fragment = code.substr( fragmentCodeStart, vertexBegin > fragmentBegin ? vertexBegin - fragmentCodeStart : String::npos );
	}

	return true;
}

// ** ShaderCache::s_shaderName
CString ShaderCache::s_shaderName[TotalShaders] = {
	  "Pink"
	, "ConstantColor"
	, "Normals"
};

// ** ShaderCache::s_shaderName
CString ShaderCache::s_modelName[Material::TotalModels] = {
	  "Unlit"
	, "Ambient"
	, "Phong"
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

	// ** Output normals as color.
	{
		CODE(
			uniform mat4 u_vp, u_transform;
			varying vec3 v_normal;

			void main() {
				v_normal	= gl_Normal;
				gl_Position = u_vp * u_transform * gl_Vertex;
			}
		),
		CODE(
			varying vec3 v_normal;

			void main() {
				gl_FragColor = vec4( v_normal * 0.5 + 0.5, 1.0 );
			}
		)
	}
};

// ------------------------------------------ Shader ------------------------------------------ //

// ** Shader::Shader
Shader::Shader( const String& vertex, const String& fragment ) : m_vertex( vertex ), m_fragment( fragment )
{
}

// ** Shader::isLoaded
bool Shader::isLoaded( u32 features ) const
{
	return features < static_cast<u32>( m_shaders.size() ) && m_shaders[features].valid();
}

// ** Shader::addFeature
void Shader::addFeature( u32 mask, const String& name )
{
	Feature feature;
	feature.mask = mask;
	feature.name = name;
	m_features.push_back( feature );
}

// ** Shader::permutation
const Renderer::ShaderPtr& Shader::permutation( Renderer::HalPtr hal, u32 features )
{
	// First try cached shader
	if( isLoaded( features ) ) {
		return m_shaders[features];
	}

	// Resize the shaders array
	if( features >= m_shaders.size() ) {
		m_shaders.resize( features + 1 );
	}

	// Generate macro definitions from features
	String macro = "";

	for( u32 i = 0, n = ( u32 )m_features.size(); i < n; i++ ) {
		if( m_features[i].mask & features ) {
			macro += "#define " + m_features[i].name + "\n";
		}
	}

	// Compile the shader
	m_shaders[features] = hal->createShader( (macro + m_vertex).c_str(), (macro + m_fragment).c_str() );

	return m_shaders[features];
}

} // namespace Scene

DC_END_DREEMCHEST