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
		auto now = std::chrono::high_resolution_clock::now( );
		while ( true ) {
			if ( GetAsyncKeyState( VK_INSERT ) ) {
				CMessage msg;
				WRITE(msg, eMessages::SendClick);
				client.write( msg );
			}
			if ( std::chrono::duration_cast< std::chrono::seconds >( std::chrono::high_resolution_clock::now( ) - now ).count( ) >= 30 ) {
				CMessage msg;
				WRITE(msg, eMessages::Ping);
				client.write( msg );
				now = std::chrono::high_resolution_clock::now( );
			}
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
		}
	}
	return 0;
}
