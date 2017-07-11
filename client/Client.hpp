#pragma once
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <boost/asio.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "CMessage.hpp"
#include <iostream>
#include <thread>

using boost::asio::ip::tcp;

class Client
{
public:
	explicit Client( boost::asio::io_service& io ): m_socket( io ) { }

	void connect( const std::string& ip, uint32_t port ) {
		if ( m_socket.is_open( ) )
			return;
		tcp::resolver resolver( m_socket.get_io_service( ) );
		auto strPort = std::to_string( port );
		auto endpoint_it = resolver.resolve( { ip, strPort } );

		boost::system::error_code ec;

		boost::asio::connect( m_socket, endpoint_it, ec );

		if ( !ec ) {
			m_connected = true;
			startRead( );
		}
		else {
			std::cout << "Error while connecting" << std::endl;
		}
	}

	void disconnect( ) {
		boost::system::error_code ec;
		m_socket.shutdown( boost::asio::socket_base::shutdown_both, ec );
		m_socket.close( ec );
	}

	bool connected( void ) {
		return m_connected;
	}

	void write( const CMessage& msg ) {
		auto strData = msg.data( );
		m_socket.write_some( boost::asio::buffer( strData ) );
	}

private:
	void startRead( void ) {
		std::thread reader( &Client::readThread, this );
		reader.detach( );
	}

	void readThread( void ) {
		while ( 1 ) {
			boost::system::error_code error;

			boost::asio::streambuf buff;
			boost::asio::read_until( m_socket, buff, "\\dxme", error );

			if ( error ) {
				std::cout << "[ERROR] " << error.message( ) << std::endl;
				disconnect( );
				return;
			}
			std::stringstream ss;
			std::ostream readstream( &buff );
			ss << readstream.rdbuf( );
			auto strData = ss.str( );

			boost::replace_first( strData, "\\dxme", "" );


			CMessage msg( strData );
			READ(msg, eMessages, type);

			if ( type == eMessages::Message ) {
				READ(msg, std::string, strText);
				std::cout << strText << "\n";
			}
			else if ( type == eMessages::SendClick ) {
				std::cout << "CLICK" << std::endl;
				mouse_event( MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0 );
				std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
				std::thread( []() {
				Beep( 300, 300 );
			} ).detach( );
				mouse_event( MOUSEEVENTF_LEFTUP, 0, 0, 0, 0 );
			}
		}
	}

	tcp::socket m_socket;

	bool m_connected;
};
