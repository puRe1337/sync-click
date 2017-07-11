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
	if ( client.connected( ) ) {
		std::string compName, userName;
		compName.resize( 256 );
		userName.resize( 256 );
		DWORD len = 256;
		GetComputerName( &compName.at( 0 ), &len );
		compName.resize( len );
		len = 256;
		GetUserName( &userName.at( 0 ), &len );
		userName.resize( len );
		client.write( std::string( compName + " - " + userName + " connected." ) );
		while ( true ) {
			if ( GetAsyncKeyState( VK_INSERT ) ) {
				CMessage msg;
				WRITE(msg, eMessages::SendClick);
				client.write( msg );
			}
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
		}
	}
	return 0;
}
