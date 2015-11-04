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

#ifndef __DC_Ecs_Archetype_H__
#define __DC_Ecs_Archetype_H__

#include "Entity.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

	//! Archetype is a predefined component composition.
	class ArchetypeBase : public Entity {
	public:

		//! Constructs archetype instance by adding all components.
		virtual void		construct( void ) = 0;

	#ifndef DC_BSON_DISABLED
		//! Reads archetype from a storage.
		virtual void		read( const io::Storage* storage );

		//! Writes archetype to a storage.
		virtual void		write( io::Storage* storage ) const;

		//! Returns the archetype BSON.
		io::Bson			bson( void ) const;

		//! Sets the archetype BSON.
		void				setBson( const io::Bson& value );
	#endif	/*	!DC_BSON_DISABLED	*/
	};

	//! Generic archetype class to declare archetypes.
	template<typename TArchetype>
	class Archetype : public ArchetypeBase {
	public:

		//! Weak pointer type.
		typedef WeakPtr<TArchetype>		WPtr;

		//! Strong pointer type.
		typedef StrongPtr<TArchetype>	Ptr;

		//! Returns the archetype type name.
		virtual CString					typeName( void ) const;
	};

	// ** Archetype::typeName
	template<typename TArchetype>
	CString Archetype<TArchetype>::typeName( void ) const
	{
		return TypeInfo<TArchetype>::name();
	}

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_Archetype_H__	*/