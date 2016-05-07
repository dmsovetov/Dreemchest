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

#ifndef __DC_Scene_AssetGenerators_H__
#define __DC_Scene_AssetGenerators_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Generates a mesh of a plane defined by two othogonal vectors.
    class MeshPlaneGenerator : public Assets::GeneratorSource<Mesh> {
    public:

                        //! Constructs a MeshPlaneGenerator instance.
                        MeshPlaneGenerator( const Vec3& u, const Vec3& v, f32 size );

    protected:

        //! Generates a plane mesh.
        virtual bool    generate( Assets::Assets& assets, Mesh& mesh ) DC_DECL_OVERRIDE;

    private:

        f32             m_size; //!< A generated quad size.
        Vec3            m_u;    //!< A first basis vector.
        Vec3            m_v;    //!< A second basis vector.
    };

    //! Generates a box mesh with specified dimensions
    class MeshBoxGenerator : public Assets::GeneratorSource<Mesh> {
    public:

                        //! Constructs a MeshBoxGenerator instance.
                        MeshBoxGenerator( f32 width, f32 height, f32 depth );

    protected:

        //! Generates a box mesh.
        virtual bool    generate( Assets::Assets& assets, Mesh& mesh ) DC_DECL_OVERRIDE;

    private:

        f32             m_width;    //!< A box width.
        f32             m_height;   //!< A box height.
        f32             m_depth;    //!< A box depth.
    };

	//! Generates a sphere mesh with a specified radius.
	class MeshSphereGenerator : public Assets::GeneratorSource<Mesh> {
    public:

                        //! Constructs a MeshSphereGenerator instance.
                        MeshSphereGenerator( f32 radius );

    protected:

        //! Generates a sphere mesh.
        virtual bool    generate( Assets::Assets& assets, Mesh& mesh ) DC_DECL_OVERRIDE;

    private:

        f32             m_radius;   //!< A sphere radius.	
	};

	//! Generates a torus mesh with a specified radius.
	class MeshTorusGenerator : public Assets::GeneratorSource<Mesh> {
    public:

                        //! Constructs a MeshTorusGenerator instance.
                        MeshTorusGenerator( f32 radius );

    protected:

        //! Generates a torus mesh.
        virtual bool    generate( Assets::Assets& assets, Mesh& mesh ) DC_DECL_OVERRIDE;

    private:

        f32             m_radius;   //!< A torus radius.	
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_AssetGenerators_H__    */