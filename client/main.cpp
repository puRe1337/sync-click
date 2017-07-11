#include <iostream>
#include "Client.hpp"

/*
 *
 * Client
 *
 */

auto main( ) -> int32_t {
	SetConsoleTitle( "sync-click-client" );
	boost::asio::io_service io;
	Client client( io );
	client.connect( "botz.cc", 10000 );
	while ( true ) {
		if ( GetAsyncKeyState( VK_INSERT ) ) {
			CMessage msg;
			WRITE(msg, eMessages::SendClick);
			client.write( msg );
		}
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
	return 0;
}
