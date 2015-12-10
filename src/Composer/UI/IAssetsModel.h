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

#ifndef __DC_Composer_IAssetsModel_H__
#define __DC_Composer_IAssetsModel_H__

#include "IUserInterface.h"

namespace Ui {

	//! Assets model interface.
	class IAssetsModel : public IUserInterface {
	public:

		//! Sets the assets model delegate.
		virtual void						setDelegate( IAssetsModelDelegateWPtr value )	= 0;

		//! Returns the assets model delegate.
		virtual IAssetsModelDelegateWPtr	delegate( void ) const							= 0;
	};

	//! Assets model delegate to process events.
	class IAssetsModelDelegate : public RefCounted {
	public:

		//! Handles the asset added event.
		virtual void				handleAssetAdded( const Asset& asset ) {}

		//! Handles the asset removed event.
		virtual void				handleAssetRemoved( const Asset& asset ) {}

		//! Handles the asset changed event.
		virtual void				handleAssetChanged( const Asset& asset ) {}
	};

} // namespace Ui

#endif	/*	!__DC_Composer_IAssetsModel_H__	*/