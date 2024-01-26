#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <mutex>
#include <vector>
#include <cassert>

template <typename T>
class Singleton {
private:
    static std::mutex _mutex;
    static T* volatile _instance;
    static bool _allowInstance;
protected:
    Singleton() {
        assert(_allowInstance);
        _allowInstance = false;
    }
    virtual ~Singleton() {}
public:
    static T* volatile instance() {
        if (Singleton::_instance == nullptr) {
            std::lock_guard<std::mutex> lock(_mutex);
            if (Singleton::_instance == nullptr) {
                _instance = new T();
            }
        }
        return _instance;
    }
};

template <typename T>
T* volatile Singleton<T>::_instance = nullptr;

template <typename T>
std::mutex Singleton<T>::_mutex;

template <typename T>
bool Singleton<T>::_allowInstance = true;

#endif  // SINGLETON_H_
