#ifndef ICOMPARABLE_H_
#define ICOMPARABLE_H_

#include "IEquatable.h"

template <typename T>
class IComparable : public IEquatable<T> {
protected:
    IComparable() {}
    virtual ~IComparable() {}
public:
    virtual bool operator<(const T& t) const = 0;
    virtual bool operator>(const T& t) const = 0;

    virtual bool operator<=(const T& t) const;
    virtual bool operator>=(const T& t) const;
};

template <typename T>
bool IComparable<T>::operator<=(const T& t) const {
    return operator < (t) || this->operator == (t);
}

template <typename T>
bool IComparable<T>::operator>=(const T& t) const {
    return operator > (t) || this->operator == (t);
}

#endif  // ICOMPARABLE_H_
