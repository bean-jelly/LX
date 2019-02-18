#include "BasicBuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string>

//////////////////////////////////////////////////////
// class BasicBuffer
//

namespace utility
{
	/*
	 *	desc:		动态buffer封装
	 *	author:		xxm
	 *  @note:		人个基本代码库
	 *  @mail:		237886849@qq.com
	 */
	BasicBuffer::BasicBuffer( uint32 grow ) : m_length(0),m_size(0), m_grow(grow)
	{
		m_buffer = NULL;
	}
	BasicBuffer::BasicBuffer( uint32 size, uint32 grow ) : m_length(0), m_size(0), m_grow(grow)
	{
		m_buffer = memalloc(size+1);//new uchar[size+1];
		if( m_buffer )
		{
			m_size = size;
		}
	}

	BasicBuffer::BasicBuffer( const void* buf, uint32 len ) : m_length(0), m_size(0)
	{
		m_buffer = memalloc(len+1);//new uchar[len+1];
		if( m_buffer )
		{
			m_size = len;
			write( buf, len );
		}
	}

	BasicBuffer::~BasicBuffer()
	{
		clear();
	}

	void BasicBuffer::reset( uint32 size, uint32 grow )
	{
		clear();
		m_buffer = memalloc(size);//new uchar[size];
		if( !m_buffer )
		{
			return;
		}
		m_length	= 0;
		m_size		= size;
		m_grow      = grow;
	}

	void BasicBuffer::clear()
	{
		if( m_buffer != NULL )
		{
			delete []m_buffer;
			m_buffer = NULL;
		}
		m_length	= 0;
		m_size		= 0;
	}

	void BasicBuffer::resize( uint32 size )
	{
		//	m_buffer.resize(size);
		uchar * temp = memalloc(size+1);//new uchar[size+1];
		if( !temp )
		{
			return;
		}
		if( m_buffer == NULL )
		{
			m_buffer = temp;
			m_size   = size;
			m_length = 0;
			return;
		}
		if ( size < m_length ) 
		{
			memcpy( temp, m_buffer, size );
			m_length = size;
		}
		else
		{
			memcpy( temp, m_buffer, m_length );
		}
		delete []m_buffer;
		m_buffer = temp;
		m_size = size;
	}

	void BasicBuffer::write( const void* buf, uint32 len )
	{
		if( (m_size - m_length) < len )
		{
			resize( m_size + len + m_grow );
		}

		if( !m_buffer || ( m_size - m_length ) < len )
		{
			return;
		}

		memcpy( m_buffer + m_length, buf, len );
		m_length += len;
	}

	void  BasicBuffer::reserve( uint32 bytes )
	{
		if( m_size < bytes )
		{
			resize( bytes );
		}
		m_length = bytes;
	}
	void BasicBuffer::descard( uint32 bytes )
	{
		if( bytes <= 0 ) 
		{
			return;
		}

		if( m_length >= bytes )
		{
			m_length -= bytes;
			memmove( m_buffer, m_buffer + bytes, m_length );
		}
		else
		{
			m_length = 0;
		}
	}

	uchar * BasicBuffer::buffer()
	{
		return (uchar*)m_buffer; 
	}

	utility::uint32 BasicBuffer::length()
	{
		return m_length;
	}

	utility::uint32 BasicBuffer::size()
	{
		return m_size;
	}

	uchar* BasicBuffer::memalloc( uint32 size )
	{
		uchar * temp = new uchar[size];
		if(temp)
			memset(temp,0,size);
		return temp;
	}
}
