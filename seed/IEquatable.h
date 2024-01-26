#ifndef IEQUATABLE_H_
#define IEQUATABLE_H_

template <typename T>
class IEquatable {
protected:
    IEquatable() {}
    virtual ~IEquatable() {}
public:
    virtual bool operator==(const T& t) const = 0;
    virtual bool operator!=(const T& t) const;
};

template <typename T>
bool IEquatable<T>::operator!=(const T& t) const {
    return !operator==(t);
}

#endif  // IEQUATABLE_H_
