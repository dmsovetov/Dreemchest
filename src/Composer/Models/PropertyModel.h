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

#define BindSetter( method, object ) std::bind( &method, object, std::placeholders::_1 )
#define BindGetter( method, object ) std::bind( &method, object )

#define BindIndexedSetter( method, object ) std::bind( &method, object, std::placeholders::_1 )
#define BindIndexedGetter( method, object ) std::bind( &method, object )

DC_BEGIN_COMPOSER

	//! Base class for object property types.
	class AbstractProperty : public RefCounted {
	public:

		//! Returns true if this is a read only property.
		virtual bool		isReadOnly( void ) const = 0;

		//! Returns the QVariant value of a property.
		virtual QVariant	get( int index ) const = 0;

		//! Sets the property value from a QVariant.
		virtual void		set( int index , const QVariant& value ) = 0;

		//! Returns the property name.
		virtual QString		name( void ) const = 0;

		//! Returns the property type.
		virtual QString		type( void ) const = 0;
	};

	dcDeclareNamedPtrs( AbstractProperty, Property )

	//! Default TValue <-> QVariant converter
	template<typename TValue>
	class DefaultConverter {
	public:

		QVariant toVariant( TValue value ) const
		{
			return QVariant::fromValue<TValue>( value );
		}

		TValue toValue( const QVariant& value ) const
		{
			return value.value<TValue>();
		}
	};

	//! Converts asset types.
	template<typename TValue>
	class AssetConverter {
	public:

		QVariant toVariant( TValue value ) const
		{
			return QVariant::fromValue<Scene::AssetWPtr>( value );
		}

		TValue toValue( const QVariant& value ) const
		{
			return static_cast<typename TValue::Type*>( value.value<Scene::AssetWPtr>().get() );
		}	
	};

	//! Enumeration value converter.
	template<typename TValue, typename TModel>
	class EnumConverter {
	public:

		QVariant toVariant( TValue value ) const
		{
			TModel model;
			QString text = model.toString( value );
			return text;
		}

		TValue toValue( const QVariant& value ) const
		{
			TModel  model;
			QString text  = value.toString();
			TValue  result = static_cast<TValue>( model.fromString( text ) );

			return result;	
		}
	};

	//! Generic property type.
	template<typename TValue, typename TConverter = DefaultConverter<TValue>>
	class Property : public AbstractProperty {
	public:

		typedef std::function<void(TValue)> Setter;	//!< Property setter type.
		typedef std::function<TValue(void)> Getter;	//!< Property getter type.
			
							//! Constructs the Property instance.
							Property( const QString& name, Getter getter, Setter setter = Setter() );

		//! Returns true if this is a read only property.
		virtual bool		isReadOnly( void ) const DC_DECL_OVERRIDE;

		//! Returns the QVariant value of a property.
		virtual QVariant	get( int index ) const DC_DECL_OVERRIDE;

		//! Sets the property value from a QVariant.
		virtual void		set( int index, const QVariant& value ) DC_DECL_OVERRIDE;

		//! Returns the property name.
		virtual QString		name( void ) const DC_DECL_OVERRIDE;

		//! Returns the property type.
		virtual QString		type( void ) const DC_DECL_OVERRIDE;

	protected:

		QString				m_name;		//!< The property name.
		Setter				m_setter;	//!< The property setter.
		Getter				m_getter;	//!< The property getter.
	};

	//! Base class for all object properties models.
	class PropertyModel : public QAbstractItemModel {
	public:

							//! Constructs PropertyModel
							explicit PropertyModel( QObject* parent );

		//! Returns the total number of properties exposed.
		int					propertyCount( void ) const;

		//! Returns the property name.
		QString				propertyName( int index ) const;

		//! Returns the property type.
		QString				propertyType( int index ) const;

	protected:

		//! Adds new asset value property.
		template<typename TValue>
		void				addAsset( const QString& name, typename Property<TValue>::Getter getter, typename Property<TValue>::Setter setter );

		//! Adds new enumeration property.
		template<typename TValue, typename TModel>
		void				addEnum( const QString& name, typename Property<TValue>::Getter getter, typename Property<TValue>::Setter setter );

		//! Returns the constant row count equal to 1.
		virtual int			rowCount( const QModelIndex& parent ) const Q_DECL_OVERRIDE;

		//! Always returns an invalid index.
		virtual QModelIndex	parent( const QModelIndex& child ) const;

		//! Returns the total number of properties exposed by an object.
		virtual int			columnCount( const QModelIndex& parent = QModelIndex() ) const Q_DECL_OVERRIDE;

		//! Reads the data from object's property at index.column.
		virtual QVariant	data( const QModelIndex& index, int role = Qt::DisplayRole ) const Q_DECL_OVERRIDE;

		//! Writes the data to object's property at index.column.
		virtual bool		setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole ) Q_DECL_OVERRIDE;

		//! Returns the model index.
		virtual QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex() ) const Q_DECL_OVERRIDE;

		//! Called each time object property was changed.
		virtual void		objectChanged( void ) {}

	protected:

		Array<PropertyPtr>	m_properties;	//!< Exposed object properties.
	};

	// ** PropertyModel::addAsset
	template<typename TValue>
	void PropertyModel::addAsset( const QString& name, typename Property<TValue>::Getter getter, typename Property<TValue>::Setter setter )
	{
		m_properties.push_back( new Property<TValue, AssetConverter<TValue>>( name, getter, setter ) );
	}

	// ** PropertyModel::addEnum
	template<typename TValue, typename TModel>
	void PropertyModel::addEnum( const QString& name, typename Property<TValue>::Getter getter, typename Property<TValue>::Setter setter )
	{
		m_properties.push_back( new Property<TValue, EnumConverter<TValue, TModel>>( name, getter, setter ) );
	}

	// ** Property::Property
	template<typename TValue, typename TConverter>
	Property<TValue, TConverter>::Property( const QString& name, Getter getter, Setter setter )
		: m_name( name ), m_getter( getter ), m_setter( setter )
	{
		
	}

	// ** Property::isReadOnly
	template<typename TValue, typename TConverter>
	bool Property<TValue, TConverter>::isReadOnly( void ) const
	{
		return !m_setter;
	}

	// ** Property::get
	template<typename TValue, typename TConverter>
	QVariant Property<TValue, TConverter>::get( int index ) const
	{
		TConverter converter;
		TValue     value = m_getter();

		return converter.toVariant( value );
	}

	// ** Property::set
	template<typename TValue, typename TConverter>
	void Property<TValue, TConverter>::set( int index, const QVariant& variant )
	{
		TConverter converter;
		TValue     value = converter.toValue( variant );
		m_setter( value );
	}

	// ** Property::name
	template<typename TValue, typename TConverter>
	QString Property<TValue, TConverter>::name( void ) const
	{
		return m_name;
	}

	// ** Property::type
	template<typename TValue, typename TConverter>
	QString Property<TValue, TConverter>::type( void ) const
	{
		return QVariant::fromValue( TValue() ).typeName();
	}

DC_END_COMPOSER

#endif	/*	!__DC_Composer_PropertyModel_H__	*/