#pragma once
#include "utility/BasicType.h"
#include <tchar.h>
#include <wininet.h>

namespace utility
{
	typedef std::wstring string_t;
#define BASIC_HTTP_VERB_GET			_T("GET")
#define BASIC_HTTP_VERB_POST		_T("POST")
	class BasicBuffer;
	/*
	 *	desc:获取http数据
	 *  author:xxm
	 */
	class BasicHttpContext
	{
	public:
		BasicHttpContext();
		virtual ~BasicHttpContext();
	public:
		/*
		 *	desc:get 方式
		 *  参数1:url
		 *	参数2:接收buffer(BYTE单位)
		 *	参数3:buffer size(BYTE单位)
		 */
		bool HttpSendConnect(const TCHAR* lpUrl, void* lpGetBuffer, int& nSize, int nOutTime = 3000, int TryCount = 1);

		/*
		 *	desc:http请求数据
		 *  参数1:url
		 *	参数2:HTTP verb
		 *	参数3:optional data,after the request headers
		 *  参数4:Read buffer
		 */
		bool HttpSendConnectEx(const TCHAR* lpUrl,const TCHAR* lpHttpVerb,BasicBuffer*const pBufOptional,BasicBuffer& getBuffer,int nOutTime = 3000, int TryCount = 1);
	private:
		/*
		 *	desc:解析url
		 */
		bool HttpParseUrl(const TCHAR* lpUrl, DWORD& dwServiceType, string_t& strSrv, string_t& strObj, INTERNET_PORT& dwPort);
		/*
		 *	desc:解析helper
		 */
		bool HttpParseURLWorker(const TCHAR* pstrURL,LPURL_COMPONENTS lpComponents, DWORD& dwServiceType,INTERNET_PORT& nPort, DWORD dwFlags);
	};
}

/*
 	demo:
	char buf[MAX_PATH] = {0};
	int nSize = MAX_PATH;
	utility::BasicHttpContext http;
	http.HttpSendConnect(_T("http://int.updrv.com/DTL/GetBindXml.aspx?aid=11&cver=2&id=999"),buf,nSize);

	std::wstring str;
	utility::BasicTranscode::UTF8_to_Unicode(buf,strlen(buf),str);

	demo2:
	utility::BasicBuffer szBuffer("tp=r0",5);
	utility::BasicBuffer szGetBuffer(BUFFER_SIZE,BUFFER_GROW);
	utility::BasicHttpContext http;
	http.HttpSendConnectEx(_T("http://sync.wallpaper.updrv.com/service/SyncHandler.ashx"),BASIC_HTTP_VERB_POST,&szBuffer,szGetBuffer);

 */