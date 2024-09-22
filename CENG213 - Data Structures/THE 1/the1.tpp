#pragma once

#include "Iterable.tpp"
#include <iostream>

// len = -1 => infinite length

// Your implementations go here.

// Repeat: Creates an iterable representing infinte sequence v v v v
template <typename T>
class RepeatIterator : public IIterator<T>
{
private:
    T value;

public:
    RepeatIterator(T parameter)
        :value(parameter)
    { }

    virtual T next() override final
    {
        return value;
    }



};

template <typename T>
class RepeatIterable : public IIterable<T>
{
public:
    T value;


    RepeatIterable(T parameter)
        :value(parameter)
    { }


    virtual IIterator<T> * iter() const override final
    {
       return new RepeatIterator<T>(value);
    }

};

template <typename T>
IIterable<T>* repeat(T v)
{
    return new RepeatIterable<T>(v);
}


// Count: Creates an iterable representing the infinite sequence  start , start+delta , start+delta+delta ,  . . .
template<typename T>
class CountIterator : public IIterator<T>
{
private:
    T start;
    T delta;
    T current;
    bool hasIterationStarted;
public:
    CountIterator(T s, T d)
        : start(s), delta(d),  hasIterationStarted(false)
    {}

    virtual T next() override final
    {
       if (!hasIterationStarted)
       {
           hasIterationStarted = true;
           current = start + delta;
           return start;
       }
       else
       {
           T temp = current;
           current += delta;
           return temp;
       }
    }

};

template<typename T>
class CountIterable : public IIterable<T>
{
private:
    T start;
    T delta;

public:
    CountIterable(T s, T d)
        :start(s), delta(d)
    {}

    virtual IIterator<T> * iter() const override final
    {
       return new CountIterator<T>(start, delta);
    }

};

template<typename T>
IIterable<T> * count(T start, T delta)
{
    return new CountIterable<T>(start, delta);
}


//Skip: Skips the firstkelements ofi. This would correspond to the sequence
template<typename T>
class SkipIterator : public IIterator<T>
{
private:
    IIterator<T>* SourceIt;
    unsigned skipCount;

public:
    virtual T next() override
    {
       return SourceIt->next();
    }

    SkipIterator(unsigned k, IIterator<T>* sourceit)
        :SourceIt(sourceit), skipCount(k)
    {

       for(unsigned i = 0; i < skipCount; i++)
       {
           try
           {
               SourceIt->next();

           } catch (StopIteration &)
           {

           }
       }
    }

    virtual ~SkipIterator() override
    {
       delete SourceIt;
    }

};

template<typename T>
class SkipIterable: public IIterable<T>
{
private:
    IIterable<T> *SourceIterable;
    unsigned skipCount;
public:

    SkipIterable(unsigned k, IIterable<T> *sourcei)
        : SourceIterable(sourcei), skipCount(k)
    {}

    virtual IIterator<T> * iter() const override
    {
       return new SkipIterator<T>(skipCount, SourceIterable->iter());
    }

    virtual ~SkipIterable() override
    {
       delete SourceIterable;
    }
};

template <typename T>
IIterable<T> * skip(unsigned k, IIterable<T> *i)
{
    return new SkipIterable<T>(k, i);
}


// Take: Creates an iterable representing the first k elements of i.
template<typename T>
class TakeIterator : public IIterator<T>
{
private:
    IIterator<T>* SourceIt;
    unsigned takeCount;
    unsigned index;
public:
    TakeIterator(unsigned k, IIterator<T>* sourceit)
        :SourceIt(sourceit), takeCount(k), index(0)
    {}


    virtual T next() override
    {
       if(index == takeCount)
       {
           throw StopIteration();
           return -1;
       }
       else
       {
           index++;
           return SourceIt->next();
       }
    }

    virtual ~TakeIterator() override
    {
       delete SourceIt;
    }

};

template<typename T>
class TakeIterable: public IIterable<T>
{
private:
    IIterable<T> *SourceIterable;
    unsigned takeCount;
public:

    TakeIterable(unsigned k, IIterable<T> *sourcei)
        : SourceIterable(sourcei), takeCount(k)
    {
    }

    virtual IIterator<T> * iter() const override
    {
       return new TakeIterator<T>(takeCount, SourceIterable->iter());
    }

    virtual ~TakeIterable() override
    {
       delete SourceIterable;
    }
};

template<typename T>
IIterable<T> * take(unsigned k, IIterable<T> *i)
{
    return new TakeIterable<T>(k, i);
}


// Cycle: Creates an iterable that cycles throughiinfinitely many times. That is, it represents the sequence
template<typename T>
class CycleIterator : public IIterator<T>
{
private:
    IIterator<T>* SourceIt;
    IIterable<T>* SourceIterable;

public:
    CycleIterator(IIterator<T> *sourcei, IIterable<T>* sourceiterable)
        : SourceIt(sourcei), SourceIterable(sourceiterable)
    {

    }

    virtual T next() override
    {
       T value;

       try
       {
           value = SourceIt->next();
       }
       catch (StopIteration &)
       {
           delete SourceIt;
           SourceIt = SourceIterable->iter();
           return SourceIt ->next();
       }

       return value;
    }

    virtual ~CycleIterator() override
    {
       delete SourceIt;
    }

};

template<typename T>
class CycleIterable : public IIterable<T>
{
private:
    IIterable<T> *SourceIterable;

public:
    CycleIterable(IIterable<T> *sourcei)
        : SourceIterable(sourcei)
    {
    }

    virtual IIterator<T> * iter() const override
    {
       return new CycleIterator<T>(SourceIterable->iter(), SourceIterable);
    }

    virtual ~CycleIterable() override
    {
       delete SourceIterable;
    }
};

template<typename T>
IIterable<T> * cycle(IIterable<T> *i)
{
    return new CycleIterable<T>(i);
}


// Concat: Concatenates two iterables i and j. That is,i[0],  . . . , i[len(i)−1], j[0], j[1]
template<typename T>
class ConcatIterator : public IIterator<T>
{
private:
    IIterable<T>* SourceIterable2;
    IIterator<T>* SourceIt;
    bool isitFirst;

public:
    ConcatIterator(IIterator<T>* sourceit1, IIterable<T>* sourceiterable2)
        : SourceIterable2(sourceiterable2), SourceIt(sourceit1), isitFirst(true)
    {}

    virtual T next() override
    {
       T value;

       try
       {
           value = SourceIt->next();
       }
       catch (StopIteration &)
       {
           if (isitFirst)
           {
               isitFirst = false;
               delete SourceIt;
               SourceIt = SourceIterable2->iter();
               return SourceIt->next();
           }
           else
           {
               throw StopIteration();
           }
       }

       return value;
    }

    virtual ~ConcatIterator() override
    {
       delete SourceIt;
    }

};

template <typename T>
class ConcatIterable : public IIterable<T>
{
private:
    IIterable<T>* SourceIterable1;
    IIterable<T>* SourceIterable2;

public:
    ConcatIterable(IIterable<T>* sourceiterable1, IIterable<T>* sourceiterable2)
        : SourceIterable1(sourceiterable1), SourceIterable2(sourceiterable2)
    {}

    virtual IIterator<T> * iter() const override
    {
       return new ConcatIterator<T>(SourceIterable1->iter(), SourceIterable2);
    }

    virtual ~ConcatIterable() override
    {
       delete SourceIterable1;
       delete SourceIterable2;
    }
};

template <typename T>
IIterable<T> * concat(IIterable<T> *i, IIterable<T> *j)
{
    return new ConcatIterable<T>(i, j);
}


// Pair: Groups the elements of an iterable i in pairs
template<typename T>
class PairIterator: public IIterator<std::pair<T, T>>
{
private:
    IIterator<T>* SourceIterator;
    std::pair<T, T> Pair;
public:
    PairIterator(IIterator<T>* sourceiterator)
        : SourceIterator(sourceiterator)
    { }

    virtual std::pair<T, T> next() override
    {
       T first, second;
       first = SourceIterator->next();
       second = SourceIterator->next();
       Pair.first = first;
       Pair.second = second;
       return Pair;
    }

    virtual ~PairIterator() override
    {
       delete SourceIterator;
    }
};

template <typename T>
class PairIterable: public IIterable<std::pair<T, T>>
{
private:
    IIterable<T>* SourceIterable;

public:
    PairIterable(IIterable<T>* sourceiterable)
        :SourceIterable(sourceiterable)
    {}

    virtual IIterator<std::pair<T, T>>* iter() const override
    {
       return new PairIterator<T>(SourceIterable->iter());
    }

    virtual ~PairIterable() override
    {
       delete SourceIterable;
    }
};

template<typename T>
IIterable<std::pair<T, T>> * pair(IIterable<T> *i)
{
    return new PairIterable<T>(i);
}


// Creates an iterable that applies an element-wise min operation on two iterables i and j
template <typename T>
class MinIterator : public IIterator<T>
{
private:
    IIterator<T> * SourceIterator1;
    IIterator<T> * SourceIterator2;

public:
    MinIterator(IIterator<T> * sourceiterator1, IIterator<T> * sourceiterator2)
        :SourceIterator1(sourceiterator1), SourceIterator2(sourceiterator2)
    {}

    virtual T next() override
    {
       T first, second;
       first = SourceIterator1->next();
       second = SourceIterator2->next();

       if(first <= second)
       {
           return first;
       }
       else
       {
           return second;
       }
    }

    virtual ~MinIterator() override
    {
       delete SourceIterator1;
       delete SourceIterator2;
    }
};

template <typename T>
class MinIterable : public IIterable<T>
{
private:
    IIterable<T>* SourceIterable1;
    IIterable<T>* SourceIterable2;

public:
    MinIterable(IIterable<T>* sourceiterable1, IIterable<T>* sourceiterable2)
        : SourceIterable1(sourceiterable1), SourceIterable2(sourceiterable2)
    {}

    virtual IIterator<T>* iter() const override
    {
       return new MinIterator<T>(SourceIterable1->iter(), SourceIterable2->iter());
    }

    virtual ~MinIterable() override
    {
       delete SourceIterable1;
       delete SourceIterable2;
    }
};

template<typename T>
IIterable<T> * min(IIterable<T> *i, IIterable<T> *j)
{
    return new MinIterable<T>(i, j);
}


// Select: i[s[0]], i[s[1]], i[s[2]],  . .
template<typename T>
class SelectIterator : public IIterator<T>
{
private:
    IIterable<T>* BaseIterable;
    IIterator<unsigned>* IndexIterator;

public:
    SelectIterator(IIterable<T>* base, IIterator<unsigned>* index)
        :BaseIterable(base), IndexIterator(index)
    {}

    virtual T next() override
    {
       unsigned index;
       index = IndexIterator->next();

       IIterator<T>* NewIterator = BaseIterable->iter();
       T value;

       for(int i = 0; i <= index; i++)
       {
           try {
               value = NewIterator->next();
           } catch (StopIteration) {
               continue;
           }
       }
       delete NewIterator;
       return value;
    }

    virtual ~SelectIterator() override
    {
       delete IndexIterator;
    }
};

template<typename T>
class SelectIterable : public IIterable<T>
{
private:
    IIterable<T>* BaseIterable;
    IIterable<unsigned>* IndexIterable;

public:
    SelectIterable(IIterable<T>* base, IIterable<unsigned>* index)
        :BaseIterable(base), IndexIterable(index)
    {}

    virtual IIterator<T>* iter() const override
    {
       return new SelectIterator(BaseIterable, IndexIterable->iter());
    }

    virtual ~SelectIterable() override
    {
       delete BaseIterable;
       delete IndexIterable;
    }
};

template<typename T>
IIterable<T> * select(IIterable<T> *i, IIterable<unsigned> *s)
{
    return new SelectIterable(i, s);
}
