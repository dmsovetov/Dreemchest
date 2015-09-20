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

#ifndef __DC_Ecs_Aspect_H__
#define __DC_Ecs_Aspect_H__

#include "../Ecs.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

	//! Entity aspect to filter entities for systems & families. 
	/*!
	Aspects are used to check if a system is interested in an entity or a given entity belongs to a family.
	*/
	class Aspect {
	public:

		//! Aspect bitsets
		enum AspectType { All, Any, Exclude };

						//! Constructs an Apsect instance.
						Aspect( AspectType type, const Bitset& mask );

		//! Retutns true if entity has an intersection with this aspect.
		bool			hasIntersection( const EntityPtr& entity ) const;

		//! Compares two aspects.
		bool			operator < ( const Aspect& other ) const;

		//! Returns an aspect with all mask.
		template<typename TComponent0>
		static Aspect	all( void );

		//! Returns an aspect with all mask.
		template<typename TComponent0, typename TComponent1>
		static Aspect	all( void );

		//! Returns an aspect with all mask.
		template<typename TComponent0, typename TComponent1, typename TComponent2>
		static Aspect	all( void );

		//! Returns an aspect with any mask.
		template<typename TComponent0>
		static Aspect	any( void );

		//! Returns an aspect with any mask.
		template<typename TComponent0, typename TComponent1>
		static Aspect	any( void );

		//! Returns an aspect with any mask.
		template<typename TComponent0, typename TComponent1, typename TComponent2>
		static Aspect	any( void );

	private:

		Bitset			m_any;	//!< Entity should have at least one component from this set.
		Bitset			m_all;	//!< Entity should contain all components from this set.
		Bitset			m_exc;	//!< Entity should not contain a components from this set.
	};

	// ** Aspect::all
	template<typename TComponent0>
	inline Aspect Aspect::all( void ) {
		return Aspect( All, TComponent0::bit() );
	}

	// ** Aspect::all
	template<typename TComponent0, typename TComponent1>
	inline Aspect Aspect::all( void ) {
		return Aspect( All, TComponent0::bit() | TComponent1::bit() );
	}

	// ** Aspect::all
	template<typename TComponent0, typename TComponent1, typename TComponent2>
	inline Aspect Aspect::all( void ) {
		return Aspect( All, TComponent0::bit() | TComponent1::bit() | TComponent2::bit() );
	}

	// ** Aspect::any
	template<typename TComponent0>
	inline Aspect Aspect::any( void ) {
		return Aspect( Any, TComponent0::bit() );
	}

	// ** Aspect::any
	template<typename TComponent0, typename TComponent1>
	inline Aspect Aspect::any( void ) {
		return Aspect( Any, TComponent0::bit() | TComponent1::bit() );
	}

	// ** Aspect::any
	template<typename TComponent0, typename TComponent1, typename TComponent2>
	inline Aspect Aspect::any( void ) {
		return Aspect( Any, TComponent0::bit() | TComponent1::bit() | TComponent2::bit() );
	}

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_Aspect_H__	*/
