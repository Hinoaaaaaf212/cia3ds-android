	/**
	 * @file Optional.h
	 * @brief Declaration of tc::Optional
	 * @author Jack (jakcron)
	 * @version 0.1
	 * @date 2019/01/06
	 **/
#pragma once
#include <tc/types.h>

namespace tc {

	/**
	 * @class Optional
	 * @brief A wrapper class, where the existence of the wrapped value is optional.
	 **/
template <class T>
class Optional
{
public:
		/**
		 * @brief Default constructor
		 *
		 * This Optional shall be null initially.
		 **/
	Optional();

		/**
		 * @brief Initialising constructor with value to wrap
		 * @param[in] value const T& Reference to value to wrap
		 * 
		 * This Optional shall be not null initially.
		 **/
	Optional(const T& value);

		/**
		 * @brief Copy constructor
		 * @param[in] other const Optional<T>& Reference to Optional object to copy
		 * 
		 * This Optional shall be not null initially.
		 **/
	Optional(const Optional<T>& other);

		/// Operator to wrap a value
	void operator=(const T& value);

		/// Operator to duplicate another Optional
	void operator=(const Optional<T>& other);

		/**
		 * @brief Access the wrapped value
		 * @return T& reference to value
		 **/
	T& get() const;

		/**
		 * @brief Determine if the Optional value doesn't exist
		 * @return bool true if the value does not exist.
		 **/
	bool isNull() const;

		/**
		 * @brief Determine if the Optional value exists
		 * @return bool true if the value exists.
		 **/
	bool isSet() const;

		/**
		 * @brief Release the wrapped value
		 * 
		 * This will destroy the wrapped value and make this Optional null.
		 * If this Optional is already null, this does nothing.
		 **/
	void makeNull();
private:
	std::shared_ptr<T> mValue;
};

template <class T>
inline Optional<T>::Optional() :
	mValue()
{
}

template <class T>
inline Optional<T>::Optional(const T& value) :
	Optional()
{
	*this = value;
}

template <class T>
inline Optional<T>::Optional(const Optional<T>& other) :
	Optional()
{
	*this = other;
}

template <class T>
inline void Optional<T>::operator=(const T& value)
{
	// if mValue is null we need to allocate memory for it
	if (mValue == nullptr)
	{
		mValue = std::shared_ptr<T>(new T);
	}

	// assign the value
	*mValue = value;
}

template <class T>
inline void Optional<T>::operator=(const Optional<T>& other)
{
	// if the other is null, then we make this null
	if (other.isNull())
	{
		this->makeNull();
	}
	// otherwise we have to assign this with the unwrapped mValue of other
	else
	{
		*this = other.get();
	}
}

template <class T>
inline T& Optional<T>::get() const
{
	return *mValue;
}

template <class T>
inline bool Optional<T>::isNull() const
{
	return mValue == nullptr;
}

template <class T>
inline bool Optional<T>::isSet() const
{
	return mValue != nullptr;
}

template <class T>
inline void Optional<T>::makeNull()
{
	mValue.reset();
}

} // namespace tc