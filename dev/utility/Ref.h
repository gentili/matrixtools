#ifndef REF_H
#define REF_H

template<class T> class Ref; // Forward declaration

#include "AutoLock.h"

template<class T> class Ref
{
public:
	Ref();
	Ref(T* obj);
	Ref(const Ref<T>& rhs);
	Ref<T>& operator =(Ref<T>& rhs);
	virtual ~Ref();

	int*	getCounter() const		{ return counter; }
	T*	getObject() const;

			T&	operator *() const;
	virtual	T*	operator ->();

protected:

	int* counter;
	T* _object;

	static pthread_mutex_t reflock;
};

template<class T> pthread_mutex_t Ref<T>::reflock;

template<class T> inline Ref<T>::Ref()
{
	counter = new int(0);
	_object = NULL;
}

template<class T> inline Ref<T>::Ref(T* obj)
{
	counter = new int(1);
	_object = obj;
}

template<class T> inline Ref<T>::Ref(const Ref<T>& rhs)
{
	AutoLock lock(&reflock);

	counter = rhs.getCounter();
	_object = rhs.getObject();
	(*counter)++;
}

template<class T> inline Ref<T>::~Ref()
{
	AutoLock lock(&reflock);

	(*counter)--;
	if( *counter == 0 )
	{
		delete _object;
		delete counter;
		_object = NULL;
	}
}

template<class T> inline Ref<T>& Ref<T>::operator =(Ref<T>& rhs)
{
	AutoLock lock(&reflock);

	int* old_counter = counter;
	T* old__object = _object;

	counter = rhs.getCounter();
	_object = rhs.getObject();
	(*counter)++;

	(*old_counter)--;
	if(*old_counter == 0)
	{
		delete old_counter;
		delete old__object;
		old__object = NULL;
	}
    
	return (*this);
}

template<class T> inline T* Ref<T>::getObject(void) const
{
    return _object;
}

template<class T> inline T& Ref<T>::operator *() const
{
    return (*_object);
}

template<class T> inline T* Ref<T>::operator ->()
{
    return (_object);
}

#endif
