#pragma once
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>
#include <deque>
#include <iostream>
#include <algorithm>

#include "../Shared/CMessage.hpp"
#include "fmt/format.h"

class Server;
class Connections;

class Connection : public std::enable_shared_from_this< Connection >
{
public:
	typedef std::shared_ptr< Connection > ptr;

	explicit Connection( boost::asio::io_service& io, Connections& s );

	void start( );

	void read( );

	void write( const CMessage& msg );

	boost::asio::ip::tcp::socket& socket( );

private:
	void handle_read( const boost::system::error_code& ec, std::size_t length );

	void do_write( );

	void handle_write( const boost::system::error_code& ec, std::size_t length );

	bool accepted( ) const;

	bool m_accepted;
	boost::asio::ip::tcp::socket m_socket;
	boost::asio::ip::tcp::endpoint m_endpoint;
	std::deque< std::string > m_message_queue;
	std::array< char, 4096 > m_buf;

	Connections& m_connections;
};
