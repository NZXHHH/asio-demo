#include "AsioIOServicePool.h"

AsioIOServicePool::AsioIOServicePool(size_t pool_size):
    _io_services(pool_size),
    _works(pool_size)
{
    for (size_t i = 0; i < pool_size; ++i)
    {
        _works[i]=unique_ptr<Work>(new Work(_io_services[i].get_executor())); 
        _threads.emplace_back([=](){
            _io_services[i].run();
        });
    }
}

AsioIOServicePool::~AsioIOServicePool()
{
    std::cout << "AsioIOServicePool destruct" << endl;
    Stop();
}

boost::asio::io_context &AsioIOServicePool::get_io_service()
{
    static size_t index = 0;
    auto& io_service = _io_services[index++];
    if (index == _io_services.size())
        index = 0;
    return io_service;
}

void AsioIOServicePool::Stop()
{
    for (auto& work : _works)
    {
        work.reset();
    }
    for (auto& io_service : _io_services)
    {
        io_service.stop();
    }
    for (auto& thread : _threads)
    {
        if (thread.joinable())
            thread.join();
    }
}