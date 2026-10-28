#pragma once
#include <cstddef>

template <class T>
class Sequence
{
public:
    virtual ~Sequence() = default;
    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    virtual T Get(std::size_t index) const = 0;
    virtual Sequence<T> *GetSubsequence(std::size_t startIndex, std::size_t endIndex) const = 0;
    virtual std::size_t GetLength() const = 0;
    virtual Sequence<T> *Append(T item) = 0;
    virtual Sequence<T> *Prepend(T item) = 0;
    virtual Sequence<T> *InsertAt(T item, std::size_t index) = 0;
    virtual Sequence<T> *Concat(Sequence<T> *list) const = 0;
};