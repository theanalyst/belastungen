#ifndef CLIENT_H
#define CLIENT_H
#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>

using tcp= boost::asio::ip::tcp;
namespace http = boost::beast::http;

void handle_error(boost::system::error_code& ec, std::string&& msg){
    std::cout << "error "<< msg << ec.message() << std::endl; 
}

class Session {
public:
    Session(boost::asio::io_context& ioc) : ioc(ioc), socket(ioc) {};

    template <typename Iter>
    void make_request(const std::string& endpoint,
	              const std::string& target,
		      Iter ep_iter,
		      boost::asio::yield_context yield){
	boost::system::error_code ec;
	boost::asio::async_connect(socket, ep_iter.begin(), ep_iter.end(),
				   yield[ec]);
	if(ec){
	    return handle_error(ec, "on connect ");
 	}

	http::request<http::string_body> req{http::verb::get, target, 11};
	req.set(http::field::host, endpoint);
	req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
	http::async_write(socket, req, yield[ec]);
	if(ec){
	    return handle_error(ec, "on socket write");
	}

	boost::beast::flat_buffer buffer;
	http::response<http::dynamic_body> res;
	http::async_read(socket, buffer, res, yield[ec]);

	if (ec) {
	    return handle_error(ec, "on socket read");
	}
	socket.shutdown(tcp::socket::shutdown_both);
    }

    ~Session(){
	boost::system::error_code ec;
	socket.shutdown(tcp::socket::shutdown_both, ec);
	if (ec && ec != boost::system::errc::not_connected)
	    handle_error(ec, "on shutdown");
    }
private:
    boost::asio::io_context& ioc;
    tcp::socket socket;
};

class BaseClient {
public:
    BaseClient(std::string&& _endpoint,
	       std::string&& _port,
	       int num_conn,
	       boost::asio::io_context& ioc):
	endpoint(std::move(_endpoint)),
	port(std::move(_port)),
	num_conn(num_conn),
	ioc(ioc),
	dns_resolver(ioc)
	{};

    template <typename CompletionToken>
    auto resolve_request(CompletionToken&& token){
	return dns_resolver.async_resolve(endpoint, port, token);
    }

   void make_request(const std::string& target,
		     boost::asio::yield_context yield)
    {
	boost::system::error_code ec;
	const auto results = resolve_request(yield[ec]);
	if(ec){
	    return handle_error(ec," resolving dns");
	}
	Session s{ioc};
	s.make_request(endpoint, target, results, yield);
    }
    
private:
    std::string endpoint;
    std::string port;
    int num_conn;
    boost::asio::io_context& ioc;
    tcp::resolver dns_resolver;
};

#endif /* CLIENT_H */
