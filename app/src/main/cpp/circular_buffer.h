#pragma once

/*
 * This is based on https://github.com/embeddedartistry/embedded-resources/blob/master/examples/cpp/circular_buffer.cpp
 * which is licensed under CC0, as such this file will remain under the same license CC0
 */
#include <cstdio>

#include <memory>
#include <mutex>

template<class T>
class circular_buffer
{
public:
    explicit circular_buffer(size_t size) :
            _buffer(std::unique_ptr<T[]>(new T[size])),
            _max_size(size)
    {
    }

    T* getBuffer()
    {
        return _buffer.get();
    }

    size_t getHeadLocation()
    {
        return _head;
    }

    void incrementHead(int inc) {

        for(int i = 0; i < inc; ++i)
        {
            if (_full) {
                _tail = (_tail + 1) % _max_size;
            }

            _head = (_head + 1) % _max_size;

            _full = _head == _tail;
        }

    }

    T get()
    {
        //std::lock_guard<std::mutex> lock(mutex_);

        if (empty())
        {
            return T();
        }

        //Read data and advance the tail (we now have a free space)
        auto val = _buffer[_tail];
        _full = false;
        _tail = (_tail + 1) % _max_size;

        return val;
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        _head = _tail;
        _full = false;
    }

    bool empty() const {
        //if head and tail are equal, we are empty
        return (!_full && (_head == _tail));
    }

    bool full() const
    {
        //If tail is ahead the head by 1, we are full
        return _full;
    }

    size_t capacity() const
    {
        return _max_size;
    }

    size_t size() const {
        size_t size = _max_size;

        if (!_full) {
            if (_head >= _tail) {
                size = _head - _tail;
            } else {
                size = _max_size + _head - _tail;
            }
        }

        return size;
    }

private:
    std::atomic<int> _buffer_size;
    std::mutex mutex_;
    std::unique_ptr<T[]> _buffer;
    size_t _head = 0;
    size_t _tail = 0;
    const size_t _max_size;
    bool _full = 0;
};