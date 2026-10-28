#pragma once
#include "LinkedList.hpp"
#include "Sequence.hpp"
#include "exceptions.hpp"

template <typename T>
class MutableListSequence : public Sequence<T>
{
private:
    LinkedList<T> list;

public:
    MutableListSequence(T *items, std::size_t count) : list(items, count) {}
    MutableListSequence() : list() {}
    MutableListSequence(const MutableListSequence &other) = default;
    explicit MutableListSequence(const LinkedList<T> &lst) : list(lst) {}

    T GetFirst() const override
    {
        if (GetLength() == 0)
            throw EmptyStructureException("MutableListSequence::GetFirst");
        return list.GetFirst();
    }

    T GetLast() const override
    {
        if (GetLength() == 0)
            throw EmptyStructureException("MutableListSequence::GetLast");
        return list.GetLast();
    }

    T Get(std::size_t index) const override
    {
        return list.Get(index);
    }

    std::size_t GetLength() const override
    {
        return list.GetLength();
    }

    Sequence<T> *GetSubsequence(std::size_t startIndex, std::size_t endIndex) const override
    {
        LinkedList<T> *subListPtr = list.GetSubList(startIndex, endIndex);
        MutableListSequence<T> *result = new MutableListSequence<T>(*subListPtr);
        delete subListPtr;
        return result;
    }

    Sequence<T> *Append(T item) override
    {
        list.Append(item);
        return this;
    }

    Sequence<T> *Prepend(T item) override
    {
        list.Prepend(item);
        return this;
    }

    Sequence<T> *InsertAt(T item, std::size_t index) override
    {
        list.InsertAt(item, index);
        return this;
    }

    Sequence<T> *Concat(Sequence<T> *seq) const override
    {
        const MutableListSequence<T> *other = dynamic_cast<const MutableListSequence<T> *>(seq);
        if (!other)
            throw InvalidArgument("MutableListSequence::Concat: неверный тип последовательности");
        MutableListSequence<T> *result = new MutableListSequence<T>(*this);
        for (auto it = other->list.begin(); it != other->list.end(); ++it)
            result->list.Append(*it);
        return result;
    }

    template <typename U, typename Func>
    MutableListSequence<U> *map(Func f) const
    {
        std::size_t len = this->GetLength();
        MutableListSequence<U> *result = new MutableListSequence<U>();
        for (std::size_t i = 0; i < len; ++i)
            result->Append(f(this->Get(i)));
        return result;
    }

    template <typename U, typename Func>
    U reduce(Func f, U init) const
    {
        U acc = init;
        for (std::size_t i = 0; i < this->GetLength(); ++i)
            acc = f(acc, this->Get(i));
        return acc;
    }
};