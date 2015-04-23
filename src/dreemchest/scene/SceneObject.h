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

#ifndef __DC_Scene_SceneObject_H__
#define __DC_Scene_SceneObject_H__

#include "Scene.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! Scene object class is a component holder.
	class SceneObject : public RefCounted {
	EmbedUserData
	public:

		virtual								~SceneObject( void );

		//! Attaches a new component to a scene object.
		template<typename T>							StrongPtr<T> attach( void );
		template<typename T, TemplateFunctionTypes1>	StrongPtr<T> attach( TemplateFunctionArgs1 );
		template<typename T, TemplateFunctionTypes2>	StrongPtr<T> attach( TemplateFunctionArgs2 );
		template<typename T, TemplateFunctionTypes3>	StrongPtr<T> attach( TemplateFunctionArgs3 );
		template<typename T, TemplateFunctionTypes4>	StrongPtr<T> attach( TemplateFunctionArgs4 );

		//! Returns a component of specified type that is attached to this scene object.
		template<typename T> StrongPtr<T>	get( void ) const;

		//! Removes a component of specified type from a scene object.
		template<typename T> void			detach( void );

		//! Returns true if a scene object has a component of specified type.
		template<typename T> bool			has( void ) const;

		//! Creates a new scene object instance.
		static SceneObjectPtr				create( void );

	private:

											//! Constructs a SceneObject instance.
											SceneObject( void );

		//! Sets a scene object component.
		template<typename T> StrongPtr<T>	set( T* value );

	private:

		//! Container type to store scene object components.
		typedef Map<TypeIdx, ComponentPtr> Components;

		//! Scene object components.
		Components			m_components;
	};

	// ** SceneObject::SceneObject
	inline SceneObject::SceneObject( void )
	{
	}

	inline SceneObject::~SceneObject( void )
	{
	}

	// ** SceneObject::create
	inline SceneObjectPtr SceneObject::create( void )
	{
		return SceneObjectPtr( DC_NEW SceneObject );
	}

	// ** SceneObject::set
	template<typename T>
	StrongPtr<T> SceneObject::set( T* value )
	{
		StrongPtr<T> component = StrongPtr<T>( value );
		m_components[TypeIndex<T>::idx()] = component;
		return component;
	}

	// ** SceneObject::get
	template<typename T>
	StrongPtr<T> SceneObject::get( void ) const
	{
		Components::const_iterator i = m_components.find( TypeIndex<T>::idx() );
		return i != m_components.end() ? i->second : StrongPtr<T>();
	}

	// ** SceneObject::has
	template<typename T>
	bool SceneObject::has( void ) const
	{
		return m_components.find( TypeIndex<T>::idx() ) != m_components.end();
	}

	// ** SceneObject::detach
	template<typename T>
	void SceneObject::detach( void )
	{
		Components::iterator i = m_components.find( TypeIndex<T>::idx() );
		if( i != m_components.end() ) {
			m_components.erase( i );
		}
	}

	// ** SceneObject::attach
	template<typename T>
	StrongPtr<T> SceneObject::attach( void )
	{
		return set( DC_NEW T( this ) );
	}

	// ** SceneObject::attach
	template<typename T, TemplateFunctionTypes1>
	StrongPtr<T> SceneObject::attach( TemplateFunctionArgs1 )
	{
		return set( DC_NEW T( this, arg0 ) );
	}

	// ** SceneObject::attach
	template<typename T, TemplateFunctionTypes2>
	StrongPtr<T> SceneObject::attach( TemplateFunctionArgs2 )
	{
		return set( DC_NEW T( this, arg0, arg1 ) );
	}

	// ** SceneObject::attach
	template<typename T, TemplateFunctionTypes3>
	StrongPtr<T> SceneObject::attach( TemplateFunctionArgs3 )
	{
		return set( DC_NEW T( this, arg0, arg1, arg2 ) );
	}

	// ** SceneObject::attach
	template<typename T, TemplateFunctionTypes4>
	StrongPtr<T> SceneObject::attach( TemplateFunctionArgs4 )
	{
		return set( DC_NEW T( this, arg0, arg1, arg2, arg3 ) );
	}

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_SceneObject_H__    */