#pragma once

FORCEINLINE size_t ROUND_UP_ALIGN(size_t src, size_t base)
{
    return ((src + (base - 1)) / base) * base;
}

// value of "base" should be 2^n. All other value will get incorrect value.
FORCEINLINE size_t ROUND_UP_ALIGN_2N(size_t src, size_t base)
{
    return ((src + (base - 1)) & (~(base-1)));
}

template<typename T>
inline void SwapEndian(T* src, T* dest)
{
    UCHAR* s = (UCHAR*)src;
    UCHAR* d = (UCHAR*)dest;
    size_t size = sizeof(T);
    for (size_t i = 0; i < size; i++)
    {
        d[i] = s[size - 1 - i];
    }
}

template<typename T>
inline void SwapEndian(T* data)
{
    UCHAR* src = (UCHAR*)data;
    UCHAR* dest = (UCHAR*)data;
    size_t size = sizeof(T) / 2;
    for (size_t i = 0; i < size; i++)
    {
        dest[i] = src[size - 1 - i];
    }
}

template<typename T>
inline T SwapEndian(T data)
{
    T temp = 0;
    UCHAR* src = (UCHAR*)&data;
    UCHAR* dest = (UCHAR*)&temp;
    size_t size = sizeof(T);
    for (size_t i = 0; i < size; i++)
    {
        dest[i] = src[size - 1 - i];
    }

    return temp;
}
