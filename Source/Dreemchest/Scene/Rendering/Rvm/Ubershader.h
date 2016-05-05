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

#ifndef __DC_Scene_Rendering_Ubershader_H__
#define __DC_Scene_Rendering_Ubershader_H__

#include "../RenderScene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Ubershader stores a source code for shader and permutation cache.
    class Ubershader : public RefCounted {
    public:

        //! A shader feature mask type.
        typedef u64                 Bitmask;

		//! A single shader feature.
		struct Feature {
			Bitmask			        mask;   //!< Feature mask.
			String					name;   //!< Feature name.
            s32                     offset; //!< A feature mask offset.
		};

                                    //! Constructs an Ubershader instance.
                                    Ubershader( void );

        //! Adds a global include used by vertex and fragment shaders.
        void                        addInclude( const String& value );

        //! Returns vertex shader source code.
        const String&               vertex( void ) const;

        //! Sets vertex shader source code.
        void                        setVertex( const String& value );

        //! Returns fragment shader source code.
        const String&               fragment( void ) const;

        //! Sets fragment shader source code.
        void                        setFragment( const String& value );

		//! Adds a feature to shader.
		void						addFeature( Bitmask mask, const String& name );

        //! Returns the total number of features in this shader.
        s32                         featureCount( void ) const;

        //! Returns the shader feature by index.
        const Feature&              feature( s32 index ) const;

        //! Returns a supported features bitmask.
        Bitmask                     supportedFeatures( void ) const;

        //! Compiles a new permutation or returns a cached one.
        const Renderer::ShaderPtr&  permutation( Renderer::HalWPtr hal, Bitmask features ) const;

    private:

        //! Container type to map from a feature set to a compiled shader program.
        typedef HashMap<Bitmask, Renderer::ShaderPtr> Permutations;

        String                      m_vertex;               //!< Vertex shader source.
        String                      m_fragment;             //!< Fragment shader source.
        Array<String>               m_includes;             //!< Shared shader includes.
        Array<Feature>              m_features;             //!< Shader features.
        Bitmask                     m_supportedFeatures;    //!< A bitmask of all supported features.
        mutable Permutations        m_permutations;         //!< Compiled permutations are stored here.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_Ubershader_H__    */