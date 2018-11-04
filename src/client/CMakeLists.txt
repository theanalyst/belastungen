add_executable(client client.cpp)
target_link_libraries(client Boost::system Boost::coroutine Boost::context Boost::thread Threads::Threads)
