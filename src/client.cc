#include <iostream>
#include <string>
#include <boost/asio/io_context.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>

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
	port(_port), num_conn(_num_conn.value_or(100)) {};
    int get_request(const std::string& target); 
};
template <typename str_type>
int BaseClient<str_type>::get_request(const std::string& target){
    http::request<http::string_body> req{http::verb::get, target, 11};
    tcp::resolver resolver{ioc};
    tcp::socket socket{ioc};

    // Look up the domain name
    auto const results = resolver.resolve(endpoint, port);

    // Make the connection on the IP address we get from a lookup
    boost::asio::connect(socket, results.begin(), results.end());

    req.set(http::field::host, endpoint);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    
    http::write(socket, req);

    boost::beast::flat_buffer buffer;
    http::response<http::dynamic_body> res;

    http::read(socket, buffer, res);

    std::cout << res << std::endl;
    boost::system::error_code ec;
    socket.shutdown(tcp::socket::shutdown_both, ec);

    // not_connected happens sometimes
    // so don't bother reporting it.
    //
    if(ec && ec != boost::system::errc::not_connected)
	return -1;

    return 0;	
} 

using Client = BaseClient<std::string>;

int main(int argc, char *argv[])
{
    
    if(argc != 2 && argc != 3)
    {
	std::cerr << "Usage: client <host> <port> [<num_connections>(100)]";
	std::cerr << "argc : "<< argc << std::endl;
	exit(-1);
    }
    std::optional<int> nc = argc == 3 ? std::optional<int>(std::stoi(argv[2]))
	: std::nullopt;
    Client client {argv[1],argv[2], nc};
    client.get_request("/");
    return 0;
}
