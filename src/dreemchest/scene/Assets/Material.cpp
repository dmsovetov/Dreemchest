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

#include "Material.h"
#include "Image.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Material::Material
Material::Material( AssetBundle* bundle, const String& uuid, const String& name ) : Asset( bundle, Asset::Material, uuid, name ), m_shader( Solid )
{
}

// ** Material::create
MaterialPtr Material::create( const String& name )
{
	return MaterialPtr( DC_NEW Material( NULL, name, name ) );
}

// ** Material::shader
Material::Shader Material::shader( void ) const
{
	return m_shader;
}

// ** Material::setShader
void Material::setShader( Shader value )
{
	m_shader = value;
}

// ** Material::color
const Rgba& Material::color( Layer layer ) const
{
	return m_color[layer];
}

// ** Material::setColor
void Material::setColor( Layer layer, const Rgba& value )
{
	m_color[layer] = value;
}

// ** Material::texture
const ImageWPtr& Material::texture( Layer layer ) const
{
	return m_texture[layer];
}

// ** Material::setTexture
void Material::setTexture( Layer layer, const ImageWPtr& value )
{
	m_texture[layer] = value;
}

} // namespace Scene

DC_END_DREEMCHEST