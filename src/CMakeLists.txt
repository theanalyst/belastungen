add_executable(eg eg.cpp)
target_link_libraries(eg Boost::system Threads::Threads)

add_executable(coro coro.cpp)
target_link_libraries(coro Boost::system Boost::coroutine Boost::context Boost::thread Threads::Threads)

add_executable(client client.cc)
target_link_libraries(client Boost::system Boost::coroutine Boost::context Boost::thread Threads::Threads)
