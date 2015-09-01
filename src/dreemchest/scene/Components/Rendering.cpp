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

#include "Rendering.h"

#include "../Assets/Mesh.h"
#include "../Assets/Material.h"

DC_BEGIN_DREEMCHEST

namespace scene {

// ---------------------------------------------- StaticMesh ---------------------------------------------- //

// ** StaticMesh::mesh
const MeshPtr& StaticMesh::mesh( void ) const
{
	return m_mesh;
}

// ** StaticMesh::setMesh
void StaticMesh::setMesh( const MeshPtr& value )
{
	m_mesh = value;
}

// ** StaticMesh::materialCount
u32 StaticMesh::materialCount( void ) const
{
	return ( u32 )m_materials.size();
}

// ** StaticMesh::material
MaterialPtr StaticMesh::material( u32 index ) const
{
	return index < materialCount() ? m_materials[index] : MaterialPtr();
}

// ** StaticMesh::setMaterial
void StaticMesh::setMaterial( u32 index, const MaterialPtr& value )
{
	DC_BREAK_IF( index > 8 );

	if( index >= materialCount() ) {
		m_materials.resize( index + 1 );
	}

	m_materials[index] = value;
}

// ** StaticMesh::lightmap
const renderer::TexturePtr& StaticMesh::lightmap( void ) const
{
	return m_lightmap;
}

// ** StaticMesh::setLightmap
void StaticMesh::setLightmap( const renderer::TexturePtr& value )
{
	m_lightmap = value;
}

// ---------------------------------------------- Sprite ---------------------------------------------- //

// ** Sprite::image
const ImagePtr& Sprite::image( void ) const
{
	return m_image;
}

// ** Sprite::color
const Rgba& Sprite::color( void ) const
{
	return m_color;
}

} // namespace scene

DC_END_DREEMCHEST