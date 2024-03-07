/***********************************************************
 *
 * @file    smartPointer.h
 *
 * @brief   smartPointer头文件
 *
 * @author  
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/


#ifndef _SMART_POINTER_H_
#define _SMART_POINTER_H_



#define COMPARE(_op_)                                    \
inline bool operator _op_ ( const sp<T>& o ) const {	 \
	return m_ptr _op_ o.m_ptr;							 \
}														 \
inline bool operator _op_ ( const T* o ) const {		 \
	return m_ptr _op_ o;								 \
}														 \
template<typename U>									 \
inline bool operator _op_ ( const sp<U>& o ) const {	 \
	return m_ptr _op_ o.m_ptr;							 \
}														 \
template<typename U>									 \
inline bool operator _op_ ( const U* o ) const {		 \
	return m_ptr _op_ o;								 \
}


class RefBase
{
protected:
	RefBase();
	virtual ~RefBase();

public:
	void incRef();
	void decRef();

private:
	int mRefNum;
};

template< typename T>
class sp
{
public:
	inline sp():m_ptr(0) {}
	~sp();

	sp( T* other);
	sp( const sp<T>& other);

	template<typename U> sp( U* other);
	template<typename U> sp( const sp<U>& other);

	sp& operator = ( const sp<T>& other);

	sp& operator = ( T* other);

	template<typename U> sp& operator = ( const sp<U>& other);
	template<typename U> sp& operator = ( U* other);

	inline T& operator* () const
	{
		return *m_ptr;
	}
	inline T* operator-> () const
	{
		return m_ptr;
	}
	inline T* get() const
	{
		return m_ptr;
	}

	COMPARE(==)
	COMPARE(!=)
	COMPARE(>)
	COMPARE(<)
	COMPARE(<=)
	COMPARE(>=)

	T* m_ptr;

};

template< typename T>
sp< T >::sp(T* other):m_ptr(other)
{
	if( other )
	{
		other->incRef();
	}
}

template< typename T>
sp< T >::sp(const sp<T>& other):m_ptr(other.m_ptr)
{
	if( m_ptr )
	{
		m_ptr->incRef();
	}
}

template< typename T> template<typename U>
sp<T>::sp(U* other):m_ptr(other)
{
	if ( other )
	{
		other->inRef();
	}
}

template< typename T> template<typename U>
sp<T>::sp(const sp<U>& other):m_ptr(other.m_ptr)
{
	if( m_ptr )
	{
		m_ptr->incRef();
	}
}

template<typename T>
sp<T>::~sp()
{
	if( m_ptr )
	{
		m_ptr->decRef();
	}
}

template<typename T>
sp<T>& sp<T>::operator = (const sp<T>& other)
{
	if( other.m_ptr )
	{
		other.m_ptr->incRef();
	}
	if ( m_ptr )
	{
		m_ptr->decRef();
	}
	m_ptr = other.m_ptr;

	return *this;
}


template<typename T>
sp<T>& sp<T>::operator = (T* other)
{
	if( other )
	{
		other->incRef();
	}
	if( m_ptr )
	{
		m_ptr->decRef();
	}
	m_ptr = other;

	return *this;
}

template<typename T> template<typename U>
sp<T>& sp<T>::operator = ( const sp<U>& other )
{
	if( other.m_ptr )
	{
		other.m_ptr->incRef();
	}
	if( m_ptr )
	{
		m_ptr->decRef();
	}
	m_ptr = other.m_ptr;
	return *this;
}

template<typename T> template<typename U>
sp<T>& sp<T>::operator = (U* other )
{
	if( other )
	{
		other->incRef();
	}
	if( m_ptr )
	{
		m_ptr->decRef();
	}
	m_ptr = other;

	return *this;
}

#endif//_SMART_POINTER_H_
