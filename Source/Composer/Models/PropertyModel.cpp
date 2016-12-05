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

#include "PropertyModel.h"

DC_BEGIN_COMPOSER

// ** PropertyModel::PropertyModel
PropertyModel::PropertyModel( Instance instance, QObject* parent )
    : QAbstractItemModel( parent )
    , m_instance( instance )
{
    NIMBLE_ABORT_IF( !instance, "invalid meta-instance passed" );

    const Reflection::Class* metaClass = instance.type();

    for( s32 i = 0, n = metaClass->memberCount(); i < n; i++ ) {
        const Reflection::Member* member = metaClass->member( i );
        if( const Property* property = member->isProperty() ) {
            m_properties.push_back( property );
        }
    }
}

PropertyModel::~PropertyModel( void )
{

}

// ** PropertyModel::properties
const PropertyModel::Properties& PropertyModel::properties( void ) const
{
    return m_properties;
}

// ** PropertyModel::rowCount
int PropertyModel::rowCount( const QModelIndex& parent ) const
{
	return 1;
}

// ** PropertyModel::parent
QModelIndex PropertyModel::parent( const QModelIndex& child ) const
{
	return QModelIndex();
}

// ** PropertyModel::columnCount
int PropertyModel::columnCount( const QModelIndex& parent ) const
{
	return properties().size();
}

// ** PropertyModel::data
QVariant PropertyModel::data( const QModelIndex& index, int role ) const
{
	int idx = index.column();
	return QVariant::fromValue( m_properties[idx]->get( m_instance ) );
}

// ** PropertyModel::setData
bool PropertyModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
	int     idx = index.column();
    Variant v   = qvariant_cast<Variant>( value );

    if( !v.isValid() ) {
        LogError( "propertyModel", "%s", "value could not be set from a void variant\n" );
        return false;
    }

	if( m_properties[idx]->update( m_instance, v ) ) {
        Q_EMIT propertyChanged();
    }

	return true;
}

// ** PropertyModel::index
QModelIndex PropertyModel::index( int row, int column, const QModelIndex& parent ) const
{
	return createIndex( row, column, ( void* )NULL );
}

DC_END_COMPOSER
