add_executable(client client.cc)
target_link_libraries(client Boost::system Boost::coroutine Boost::context Boost::thread Threads::Threads)
