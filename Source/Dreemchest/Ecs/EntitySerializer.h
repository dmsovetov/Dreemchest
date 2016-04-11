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

#ifndef __DC_Ecs_EntitySerializer_H__
#define __DC_Ecs_EntitySerializer_H__

#include "Ecs.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

    //! Performs writing/reading entities to a key-value storage.
    class Serializer : public Reflection::Serializer {
    public:

                                            //! Constructs Serializer instance.
                                            Serializer( EcsWPtr ecs, const Bitset& excluded = Bitset() );

        //! Writes entity instance to a key-value storage.
        virtual bool                        serialize( EntityWPtr entity, KeyValue& ar ) const;

        //! Creates entity and reads it from a key-value storage.
        virtual bool                        deserialize( Reflection::AssemblyWPtr assembly, EntityWPtr entity, const KeyValue& ar );

	private:

		//! Callback to access the default value of Entity flags property.
		Variant								defaultEntityFlags( const KeyValue& ar ) const;
    private:

        EcsWPtr                             m_ecs;      //!< Parent Ecs instance.
        Bitset                              m_excluded; //!< List of excluded components.
    };

} // namespace Ecs

DC_END_DREEMCHEST

#endif    /*    !__DC_Ecs_EntitySerializer_H__    */