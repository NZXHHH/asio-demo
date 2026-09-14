#include "AsioIOThreadPool.h"

AsioIOThreadPool::AsioIOThreadPool(size_t pool_size):
    _io_services(),
    _works(unique_ptr<Work>(new Work(_io_services.get_executor())))
{
    for (size_t i = 0; i < pool_size; ++i)
    { 
        _threads.emplace_back([=](){
            _io_services.run();
        });
    }
}

AsioIOThreadPool::~AsioIOThreadPool()
{
    std::cout << "AsioIOThreadPool destruct" << endl;
    Stop();
}

boost::asio::io_context &AsioIOThreadPool::get_io_service()
{
    return _io_services;
}

void AsioIOThreadPool::Stop()
{
        _works.reset();
        _io_services.stop();
    for (auto& thread : _threads)
    {
        if (thread.joinable())
            thread.join();
    }
}