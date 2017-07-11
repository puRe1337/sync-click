#pragma once
#include "Connection.hpp"
#include <set>

class Connections
{
public:
	void add( Connection::ptr c ) {
		m_connections.insert( c );
	}

	void write( const CMessage& msg ) {
		for ( auto& c : m_connections ) {
			c->write( msg );
		}
	}

	void write( const std::string& text ) {
		CMessage msg;
		WRITE(msg, eMessages::Message);
		WRITE(msg, text);
		write( msg );
	}

	void remove( Connection::ptr c ) {
		m_connections.erase( c );
	}

	auto connected_clients( ) {
		return m_connections.size( );
	}

private:
	std::set< Connection::ptr > m_connections;
};
