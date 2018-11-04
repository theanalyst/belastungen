#include "client.hpp"
#include <iostream>
#include <string>
#include <functional>
#include <boost/asio/spawn.hpp>

int main(int argc, char *argv[])
{
    
    if(argc != 4)
    {
	std::cerr << "Usage: client <host> <port> [<num_connections>(100)]";
	std::cerr << "argc : "<< argc << std::endl;
	exit(-1);
    }

    boost::asio::io_context ioc;
    BaseClient c{argv[1], argv[2], std::stoi(argv[3]), ioc};

    std::string target("/");
    boost::asio::spawn(ioc,[&c,&target](boost::asio::yield_context y){
	    c.make_request(target, y);});
    
    return 0;
}
