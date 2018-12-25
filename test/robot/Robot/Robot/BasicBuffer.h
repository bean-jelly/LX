#pragma once
#ifndef __BASICBUFFER_H__
#define __BASICBUFFER_H__
#include "utility/BasicType.h"

namespace utility
{
#define BUFFER_SIZE	(1 * 1024)
#define BUFFER_GROW	(16 * 1024)

//	typedef char            	int8;
//	typedef unsigned char   	uint8;
//	typedef int             	int32;
//	typedef unsigned int    	uint32;
//	typedef short           	int16;
//	typedef unsigned short		uint16;
//#ifdef _WIN32
//	typedef __int64  			int64;
//	typedef unsigned __int64	uint64;
//#else
//	typedef long long			int64;
//	typedef unsigned long long	uint64;
//#endif
//
//	typedef unsigned long   	ulong;
//	typedef long            	slong;
//	typedef unsigned char   	uchar;
//	typedef unsigned char   	byte;
	/*
	 *	@desc:		buffer封装
	 *  @author:	xxm
	 *  @note:		人个基本代码库
	 *  @mail:		237886849@qq.com
	 */
	class BasicBuffer  
	{
	public:
		BasicBuffer( uint32 grow );
		BasicBuffer( uint32 size, uint32 grow );
		BasicBuffer( const void* buf, uint32 len );
		virtual ~BasicBuffer();

		uchar *		buffer();
		uint32		length();
		uint32      size();

		void        reserve( uint32 bytes );
		void		descard( uint32 bytes );

		void		reset( uint32 size = BUFFER_SIZE, uint32 grow = BUFFER_GROW );

		void		write( const void* buf, uint32 len );

	protected:

		uchar *     m_buffer;
		uint32		m_length;
		uint32		m_size;
		uint32		m_grow;

		void		resize(uint32 size);
		void		clear();
		uchar*		memalloc(uint32 size);
	};

	//typedef std::vector<BasicBuffer*> BUFFER_LIST;
}
#endif

/*
	
	utility::BasicBuffer  m_mem_buffer(50*1024);
	m_mem_buffer.write( pBuf, nBufLen );

	MD5 md5( m_mem_buffer.buffer(), m_mem_buffer.length() );

	DecryptData( (char*)m_mem_buffer.buffer(), m_mem_buffer.length() );






*/