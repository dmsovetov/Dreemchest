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

#ifndef __DC_Renderer_PipelineFeatureLayout_H__
#define __DC_Renderer_PipelineFeatureLayout_H__

#include "Renderer.h"
#include "RenderState.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{    
    //! A single pipeline feature item is a string identifier and a corrensponding bit mask.
    struct PipelineFeature
    {
        FixedString             name;       //!< A feature name that is exposed to shader code.
        PipelineFeatures        bits;       //!< A corresponding pipeline feature mask.
        
        //! Generate a feature mask that corresponds to a specified vertex attribute.
        static PipelineFeatures vertexAttribute(VertexAttribute vertexAttribute);
        
        //! Generate a feature mask that corresponds to a constant buffer binding at specified slot.
        static PipelineFeatures constantBuffer(u8 index);
        
        //! Generate a feature mask that corresponds to a texture sampler binding at specified index.
        static PipelineFeatures sampler(u8 index);
        
        //! Generate a feature mask that corresponds to a user defined feature bits.
        static PipelineFeatures user(PipelineFeatures userDefined);
    };
    
    //! A pipeline state contains an active shader and a bitmask of activated pipeline features.
    class PipelineState
    {
    public:
        
        //! A bitfields that encode changes in a pipeline state.
        enum
        {
              ProgramChanged        = BIT(0)    //!< Indicates that a bound shader program was changed.
            , FeaturesChanged       = BIT(1)    //!< Indicates that a pipeline feature bitmask was changed.
            , FeatureLayoutChanged  = BIT(2)    //!< Indicates that a pipeline feature layout was changed.
        };
        
                                        //! Constructs a PipelineState instance.
                                        PipelineState( void );
        
        //! Sets an active program.
        void                            setProgram(Program value);
        
        //! Returns an active program.
        Program                         program( void ) const;
        
        //! Sets an active feature layout.
        void                            setFeatureLayout(const PipelineFeatureLayout* value);
        
        //! Returns an active feature layout.
        const PipelineFeatureLayout*    featureLayout( void ) const;
        
        //! Activates vertex attribute features.
        void                            activateVertexAttributes(PipelineFeatures features);
        
        //! Activates a corresponding sampler bit in a feature bitmask.
        void                            activateSampler(u8 index);
        
        //! Activates a corresponding constant buffer bit in a feature bitmask.
        void                            activateConstantBuffer(u8 index);
        
        //! Activates a corresponding user defined bits in a feature bitmask.
        void                            activateUserFeatures(PipelineFeatures features);
        
        //! This method should be called BEFORE applying state blocks to track pipeline changes.
        void                            beginStateBlock( void );
        
        //! This method should be called AFTER all state block are applied to track pipeline changes.
        void                            endStateBlock( void );
        
        //! Returns an active feature bitmask.
        PipelineFeatures                features( void ) const;
        
        //! Returns a supported features bitmask.
        PipelineFeatures                mask( void ) const;
        
        //! Returns a pipeline state changes bitmask.
        u8                              changes( void ) const;
        
        //! This method should be called after activating a program permutation.
        void                            acceptChanges( void );
        
    private:
        
        Program                         m_program;              //!< An active program.
        PipelineFeatures                m_stateBlockFeatures;   //!< A bitmask of pipeline features activated by a list of state blocks.
        PipelineFeatures                m_features;             //!< A bitmask of activated pipeline features.
        const PipelineFeatureLayout*    m_featureLayout;        //!< An active feature layout.
        u8                              m_changes;              //!< A bitmask of recorded pipeline state changed.
    };
    
    //! Pipeline feature layout defines mappings from a pipeline feature mask to actual preprocessor definitions.
    class PipelineFeatureLayout
    {
    public:
        
        //! A single pipeline feature.
        struct Element
        {
            PipelineFeatures    mask;       //!< Pipeline feature mask that matches this element.
            String              name;       //!< Feature name.
            s32                 offset;     //!< A feature mask offset to extract actual element value.
        };
        
                                //! Construct PipelineFeatureLayout instance.
                                PipelineFeatureLayout( void );
        
        //! Returns a bitmask of supported pipeline features.
        PipelineFeatures        mask( void ) const;
        
        //! Returns a total number of shader feature elements.
        s32                     elementCount( void ) const;
        
        //! Returns an element at specified index.
        const Element&          elementAt(s32 index) const;
        
        //! Adds a new element to a layout.
        void                    addFeature(const String& name, PipelineFeatures mask);
        
    private:
        
        Array<Element>          m_elements; //!< An array of available pipeline features.
        PipelineFeatures        m_mask;     //!< A bitmask with supported pipeline features.
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_PipelineFeatureLayout_H__   */
