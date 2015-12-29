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

#ifndef __DC_Composer_AssetModels_H__
#define __DC_Composer_AssetModels_H__

#include "PropertyModel.h"

DC_BEGIN_COMPOSER

	//! Material asset model.
	class MaterialModel : public PropertyModel {
	public:

                            //! Constructs MaterialModel instance.
		                    MaterialModel( Scene::MaterialWPtr material, QObject* parent = NULL );

		//! Called each time object property was changed.
		virtual void		objectChanged( void ) DC_DECL_OVERRIDE { m_material->bundle()->queueForLoading( m_material ); }

	private:

		Scene::MaterialWPtr m_material;
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Qt_AssetModels_H__	*/