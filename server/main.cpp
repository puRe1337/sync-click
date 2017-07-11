#include <iostream>
#include "Server.hpp"

/*
 *
 * Server
 *
 */

auto main( ) -> int32_t {
#ifdef WIN32
	SetConsoleTitle( "sync-click-server" );
#endif
	boost::asio::io_service io;
	Server tcpserver( io, 10000 );
	io.run( );
	std::cin.get( );
	return 0;
}
