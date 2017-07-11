#pragma once
#include "Connection.hpp"
#include <set>

class Connections
{
public:
	void add( Connection::ptr c ) {
		m_connections.insert( c );
	}

	void remove( Connection::ptr c ) {
		m_connections.erase( c );
	}

	std::set< Connection::ptr > connections( ) { return m_connections; }
private:
	std::set< Connection::ptr > m_connections;
};
