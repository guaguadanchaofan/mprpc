#pragma once
#include <string>
#include <queue>
#include <mutex> //pthread_mutex_t
#include <thread>
#include <condition_variable> //pthread_cond_t


// 异步写入日志队列
template <class T>
class LockQueue
{
public:
    void Push(const T &data)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(data);
        _condvariable.notify_one();
    }

    T Pop()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (_queue.empty())
        {
            // 日志队列为空 ，线程进等待
            _condvariable.wait(lock);
        }
        T data = _queue.front();
        _queue.pop();
        return data;
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _condvariable;
};
