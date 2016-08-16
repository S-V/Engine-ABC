/*
=============================================================================
	File:	KeyValue.h
	Desc:	Class representing a single Key/Value pair,
			useful for building dictionaries.
			Swiped from Nebula 3 (C) 2006 Radon Labs GmbH.
=============================================================================
*/

#ifndef __LIB_KEY_VALUE_H__
#define __LIB_KEY_VALUE_H__

namespace abc {

template< typename TKey, typename TValue >
class TKeyValue {
public:
	TKeyValue();
	/// constructor with key and value
	TKeyValue( const TKey& k, const TValue& v );
	/// constructor with key and undefined value
	TKeyValue( const TKey& k );
	/// copy constructor
	TKeyValue( const TKeyValue< TKey, TValue >& rhs );
	
	void	 operator =  ( const TKeyValue< TKey, TValue >& rhs );
	bool	 operator == ( const TKeyValue< TKey, TValue >& rhs ) const;
	bool	 operator != ( const TKeyValue< TKey, TValue >& rhs ) const;
	bool	 operator >  ( const TKeyValue< TKey, TValue >& rhs ) const;
	bool	 operator >= ( const TKeyValue< TKey, TValue >& rhs ) const;
	bool	 operator <  ( const TKeyValue< TKey, TValue >& rhs ) const;
	bool	 operator <= ( const TKeyValue< TKey, TValue >& rhs ) const;
	
	/// read/write access to value
	TValue &		Value();

	/// read access to key
	const TKey &	Key() const;
	
	/// read access to key
	const TValue &	Value() const;

protected:
	TKey	keyData;
	TValue	valueData;
};

//------------------------------------------------------------------------------
/**
*/
template< typename TKey, typename TValue >
TKeyValue<TKey, TValue>::TKeyValue()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
template< typename TKey, typename TValue >
TKeyValue<TKey, TValue>::TKeyValue(const TKey& k, const TValue& v) :
keyData(k),
valueData(v)
{
	// empty
}

//------------------------------------------------------------------------------
/**
This strange constructor is useful for search-by-key if
the key-value-pairs are stored in a Util::Array.
*/
template< typename TKey, typename TValue >
TKeyValue<TKey, TValue>::TKeyValue(const TKey& k) :
keyData(k)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
template< typename TKey, typename TValue >
TKeyValue<TKey, TValue>::TKeyValue(const TKeyValue<TKey, TValue>& rhs) :
keyData(rhs.keyData),
valueData(rhs.valueData)
{
	// empty
}

template< typename TKey, typename TValue >
void
TKeyValue<TKey, TValue>::operator=(const TKeyValue<TKey, TValue>& rhs)
{
	this->keyData = rhs.keyData;
	this->valueData = rhs.valueData;
}

template< typename TKey, typename TValue >
bool
TKeyValue<TKey, TValue>::operator==(const TKeyValue<TKey, TValue>& rhs) const
{
	return (this->keyData == rhs.keyData);
}

template< typename TKey, typename TValue >
bool
TKeyValue<TKey, TValue>::operator!=(const TKeyValue<TKey, TValue>& rhs) const
{
	return (this->keyData != rhs.keyData);
}

template< typename TKey, typename TValue >
bool
TKeyValue<TKey, TValue>::operator>(const TKeyValue<TKey, TValue>& rhs) const
{
	return (this->keyData > rhs.keyData);
}

template< typename TKey, typename TValue >
bool
TKeyValue<TKey, TValue>::operator>=(const TKeyValue<TKey, TValue>& rhs) const
{
	return (this->keyData >= rhs.keyData);
}

template< typename TKey, typename TValue >
bool
TKeyValue<TKey, TValue>::operator<(const TKeyValue<TKey, TValue>& rhs) const
{
	return (this->keyData < rhs.keyData);
}

template< typename TKey, typename TValue >
bool
TKeyValue<TKey, TValue>::operator<=(const TKeyValue<TKey, TValue>& rhs) const
{
	return (this->keyData <= rhs.keyData);
}

template< typename TKey, typename TValue >
TValue&
TKeyValue<TKey, TValue>::Value()
{
	return this->valueData;
}

template< typename TKey, typename TValue >
const TKey&
TKeyValue<TKey, TValue>::Key() const
{
	return this->keyData;
}

template< typename TKey, typename TValue >
const TValue&
TKeyValue<TKey, TValue>::Value() const
{
	return this->valueData;
}

} //End of namespace abc

#endif // !__LIB_KEY_VALUE_H__

