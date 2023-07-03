#include "Single.hpp"

template<typename T>
T* Single<T>::_pInstance = nullptr;

template<typename T>
typename Single<T>::AutoRelease Single<T>::_autoRelease;

template<typename T>
std::mutex Single<T>::_mutex;