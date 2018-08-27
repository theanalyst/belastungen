#include <iostream>
#include <string>
#include <boost/asio/io_context.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/thread/thread.hpp>

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;
//static constexpr auto http_1_1_ver = 11;
template <typename str_type>
class BaseClient {
    str_type endpoint;
    str_type port;
    int num_conn;
    boost::asio::io_context ioc;
public:
    BaseClient(str_type&& ep, str_type _port, std::optional<int> _num_conn) :
	endpoint(std::forward<str_type>(ep)),
	port(_port), num_conn(_num_conn.value_or(100)) {
	std::cout << "num_conn" << num_conn << std::endl;
    };
    int get_request(const std::string& target); 
};

template <typename str_type>
int BaseClient<str_type>::get_request(const std::string& target){
    
    tcp::resolver resolver {ioc};

    auto const results = resolver.resolve(endpoint, port);
//    boost::asio::io_context::strand strand_;

//    boost::asio::thread_pool pool;
    try {
	for (int i=0; i< num_conn; i++){
	    std::cout << "spawning conn " << i << std::endl;
	    boost::asio::spawn(ioc, [this, results,target](boost::asio::yield_context yield)
			       {
				   tcp::socket socket {ioc};
				   boost::asio::async_connect(socket, results.begin(), results.end(), yield);			       
				   http::request<http::string_body> req{http::verb::get, target, 11};
				   req.set(http::field::host, endpoint);
				   req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
				   http::async_write(socket, req, yield);
				   boost::beast::flat_buffer buffer;
				   http::response<http::dynamic_body> res;
				   http::async_read(socket, buffer, res, yield);
				   socket.shutdown(tcp::socket::shutdown_both);
				   std::cout<< "done" << std::endl;
//std::cout << res << std::endl;
			       });    
	} 
    } catch (boost::system::error_code& ec) {
	std::cout << "connect error  " << ec.message() << std::endl;
    }
    std::cout << "running " << std::endl;
    // boost::thread_group worker_threads;
    // for (int i=0; i<1;i++){
    // 	worker_threads.create_thread(boost::bind(&boost::asio::io_context::run, &ioc));
    // }
    // worker_threads.join_all();
    try {
	ioc.run();
//	pool.join();
    } catch (const std::exception& ec) {
	std::cout << "run error" << ec.what() << std::endl;
    }
    
    return 0;	
} 

using Client = BaseClient<std::string>;

int main(int argc, char *argv[])
{
    
    if(argc != 3 && argc != 4)
    {
	std::cerr << "Usage: client <host> <port> [<num_connections>(100)]";
	std::cerr << "argc : "<< argc << std::endl;
	exit(-1);
    }
    std::optional<int> nc = argc == 4 ? std::optional<int>(std::stoi(argv[3]))
	: std::nullopt;
    Client client {argv[1],argv[2], nc};
    client.get_request("/");
    return 0;
}
