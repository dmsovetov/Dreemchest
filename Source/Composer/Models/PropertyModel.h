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

#ifndef __DC_Composer_PropertyModel_H__
#define __DC_Composer_PropertyModel_H__

#include "../Composer.h"

DC_BEGIN_COMPOSER

    //! Contains an array of object properties.
    class PropertyModel : public QAbstractItemModel {

        Q_OBJECT

    Q_SIGNALS:

        //! Emitted when a property was changed.
        void                        propertyChanged( void );

    public:

                                    //! Constructs PropertyModel instance.
                                    PropertyModel( Introspection::MetaObject* metaObject, QObject* parent = NULL );
                                    ~PropertyModel( void );

    protected:

	    //! Returns the constant row count equal to 1.
	    virtual int			        rowCount( const QModelIndex& parent ) const Q_DECL_OVERRIDE;

	    //! Always returns an invalid index.
	    virtual QModelIndex	        parent( const QModelIndex& child ) const Q_DECL_OVERRIDE;

	    //! Returns the total number of properties exposed by an object.
	    virtual int			        columnCount( const QModelIndex& parent = QModelIndex() ) const Q_DECL_OVERRIDE;

	    //! Reads the data from object's property at index.column.
	    virtual QVariant	        data( const QModelIndex& index, int role = Qt::DisplayRole ) const Q_DECL_OVERRIDE;

	    //! Writes the data to object's property at index.column.
	    virtual bool		        setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole ) Q_DECL_OVERRIDE;

	    //! Returns the model index.
	    virtual QModelIndex         index( int row, int column, const QModelIndex& parent = QModelIndex() ) const Q_DECL_OVERRIDE;
    };

DC_END_COMPOSER

#endif	/*	!__DC_Composer_PropertyModel_H__	*/