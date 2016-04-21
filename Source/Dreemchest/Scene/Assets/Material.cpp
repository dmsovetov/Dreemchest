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
Material::Material( void )
    : m_lightingModel( LightingModel::Unlit )
    , m_renderingMode( RenderingMode::Opaque )
    , m_isTwoSided( false )
    , m_features( 0 )
{
	setColor( Diffuse, Rgba( 0.8f, 0.8f, 0.8f ) );
	setColor( Specular, Rgba( 0.8f, 0.8f, 0.8f ) );
}

// ** Material::renderingMode
RenderingMode Material::renderingMode( void ) const
{
	return m_renderingMode;
}

// ** Material::setRenderingMode
void Material::setRenderingMode( RenderingMode value )
{
	m_renderingMode = value;
}

// ** Material::lightingModel
LightingModel Material::lightingModel( void ) const
{
	return m_lightingModel;
}

// ** Material::setLightingModel
void Material::setLightingModel( LightingModel value )
{
	m_lightingModel = value;
}

// ** Material::isTwoSided
bool Material::isTwoSided( void ) const
{
    return m_isTwoSided;
}

// ** Material::setTwoSided
void Material::setTwoSided( bool value )
{
    m_isTwoSided = value;
}

// ** Material::features
u32 Material::features( void ) const
{
	return m_features;
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
ImageHandle Material::texture( Layer layer ) const
{
	return m_texture[layer];
}

// ** Material::setTexture
void Material::setTexture( Layer layer, ImageHandle value )
{
	m_texture[layer] = value;
	updateMaterialFeatures();
}

// ** Material::diffuse
ImageHandle Material::diffuse( void ) const
{
	return texture( Diffuse );
}

// ** Material::setDiffuse
void Material::setDiffuse( ImageHandle value )
{
	setTexture( Diffuse, value );
}

// ** Material::updateMaterialFeatures
void Material::updateMaterialFeatures( void )
{
	m_features = 0;

    for( s32 i = 0; i < TotalMaterialLayers; i++ ) {
	    if( m_texture[i].isValid() ) {
		    m_features |= BIT( i );
	    }    
    }

    m_features |= PrimaryUvBit;
}

} // namespace Scene

DC_END_DREEMCHEST