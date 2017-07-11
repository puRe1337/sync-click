#include "Connection.hpp"
#include "Connections.hpp"

Connection::Connection( boost::asio::io_service& io, Connections& s ) : m_socket( io ), m_connections( s ) {}

void Connection::start( ) {
	read( );
	m_endpoint = m_socket.remote_endpoint( );
	m_connections.add( shared_from_this( ) );
}

void Connection::read( ) {
	m_socket.async_read_some( boost::asio::buffer( m_buf, m_buf.size( ) ), std::bind( &Connection::handle_read, shared_from_this( ), std::placeholders::_1, std::placeholders::_2 ) );
}

void Connection::write( const CMessage& msg ) {
	std::string strData = msg.data( ) + "\\dxme";
	bool write_in_progress = !m_message_queue.empty( );
	m_message_queue.push_back( strData );
	if ( !write_in_progress ) {
		do_write( );
	}
}

boost::asio::ip::tcp::socket& Connection::socket( ) {
	return m_socket;
}

void Connection::handle_read( const boost::system::error_code& ec, std::size_t length ) {
	try {
		if ( !ec ) {
			if ( length > 0 ) {
				std::string strData( m_buf.begin( ), m_buf.end( ) );
				strData.resize( length );

				CMessage msg( strData );
				READ(msg, eMessages, type);

				if ( type == eMessages::SendClick ) {

					for ( auto& c : m_connections.connections( ) ) {
						c->write( msg );
					}
				}
				else if ( type == eMessages::Message ) {
					READ(msg, std::string, text);
					fmt::print( "<{}>: {}\n", boost::lexical_cast< std::string >( m_socket.remote_endpoint( ) ), text );
				}

				read( );
			}
		}
		else {
			if ( m_socket.is_open( ) ) {
				m_connections.remove( shared_from_this( ) );
				auto rmtIp = m_endpoint;
				fmt::print( "Client disconnected ( {} ) - {}\n", ec.message( ), boost::lexical_cast< std::string >( rmtIp ) );
				m_socket.close( );
			}
		}
	}
	catch ( const std::exception& e ) {
		auto rmtIp = m_endpoint;
		fmt::print( "Client kicked ( {} ) - {}\n", e.what( ), boost::lexical_cast< std::string >( rmtIp ) );
		m_connections.remove( shared_from_this( ) );
		m_socket.shutdown( boost::asio::socket_base::shutdown_both );
		m_socket.close( );
	}
}

void Connection::do_write( ) {
	boost::asio::async_write( m_socket,
	                          boost::asio::buffer( m_message_queue.front( ) ),
	                          boost::asio::transfer_exactly( m_message_queue.front( ).size( ) ),
	                          std::bind( &Connection::handle_write, shared_from_this( ), std::placeholders::_1, std::placeholders::_2 ) );

	std::cout << "writing " << m_message_queue.front( ).size( ) << std::endl;
}

void Connection::handle_write( const boost::system::error_code& ec, std::size_t length ) {
	if ( ec ) {
		if ( m_socket.is_open( ) ) {
			m_connections.remove( shared_from_this( ) );
			auto rmtIp = m_endpoint;

			fmt::print( "Client disconnected ( {} ) - {}\n", ec.message( ), boost::lexical_cast< std::string >( rmtIp ) );
			m_socket.close( );
		}
	}
	else {
		m_message_queue.pop_front( );
		std::cout << "written " << length << std::endl;
		if ( !m_message_queue.empty( ) ) {
			do_write( );
		}
	}
}
