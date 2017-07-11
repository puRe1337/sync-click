#pragma once
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <boost/asio.hpp>
#include "Connection.hpp"
#include "Connections.hpp"

using boost::asio::ip::tcp;

class Server
{
public:
	explicit Server( boost::asio::io_service& io, uint32_t port ) : m_acceptor( io, tcp::endpoint( tcp::v4( ), port ), true ) {
		start_accept( );
	}

private:
	void start_accept( ) {
		auto connection = std::make_shared< Connection >( m_acceptor.get_io_service( ), m_connections );
		m_acceptor.async_accept( connection->socket( ), std::bind( &Server::handle_accept, this, connection, std::placeholders::_1 ) );
	}

	void handle_accept( Connection::ptr con, const boost::system::error_code& ec ) {
		if ( !ec ) {
			fmt::print( "Client connected - {}\n", boost::lexical_cast< std::string >( con->socket( ).remote_endpoint( ) ) );
			con->start( );
		}
		start_accept( );
	}


	tcp::acceptor m_acceptor;
	Connections m_connections;
};
