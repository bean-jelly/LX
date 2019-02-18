#include "stdafx.h"
#include <wininet.h>
#include <Shlwapi.h>
#include "BasicHttpContext.h"
#include "BasicBuffer.h"

#pragma comment(lib, "Wininet.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma warning(disable:4996)
namespace utility
{
	/*
	 *	desc:获取http数据
	 *	author:xxm
	 */
	BasicHttpContext::BasicHttpContext()
	{
	}

	BasicHttpContext::~BasicHttpContext()
	{
	}

	bool BasicHttpContext::HttpSendConnect( const TCHAR* lpUrl, void* lpGetBuffer, int& nSize, int nOutTime /*= 3000*/, int TryCount /*= 1*/ )
	{
		bool bRes = false;
		if( NULL == lpUrl || NULL == lpGetBuffer )
			return false;

		HINTERNET internet=InternetOpen(_T("HTTP Connect"), INTERNET_OPEN_TYPE_PRECONFIG,NULL, NULL, NULL);
		if(!internet)
			return false;

		HINTERNET file_handle=InternetOpenUrl(internet,lpUrl, NULL, 0,INTERNET_FLAG_RELOAD, 0);
		if(!file_handle)
		{
			InternetCloseHandle(internet);
			return false;
		}

		InternetSetOption(file_handle,INTERNET_OPTION_CONNECT_TIMEOUT ,&nOutTime,sizeof(nOutTime));
		InternetSetOption(file_handle,INTERNET_OPTION_CONNECT_RETRIES ,&TryCount,sizeof(TryCount));

		try{
			do{
				TCHAR lpBuffer[32] = {_T('\0')};
				DWORD dwSize = 32;
				DWORD dwIndex = 0;

				//参数3,是以BYTE为单位，参数4是lpdwBufferLength 单位也是BYTE
				if( !HttpQueryInfo(file_handle,HTTP_QUERY_STATUS_CODE, lpBuffer, &dwSize, &dwIndex) )
					break;

				DWORD dwStateCode = _ttol(lpBuffer);
				if( dwStateCode != HTTP_STATUS_OK && dwStateCode != HTTP_STATUS_REDIRECT && dwStateCode != HTTP_STATUS_PARTIAL_CONTENT )
					break;

				dwSize = 32;
				dwIndex = 0;
				memset( lpBuffer, 0x00, sizeof(lpBuffer));//sizeof(lpBuffer) 等于 32*sizeof(TCHAR)
				//参数3,是以BYTE为单位，参数4是lpdwBufferLength 单位也是BYTE
				if( !HttpQueryInfo(file_handle,HTTP_QUERY_CONTENT_LENGTH, lpBuffer, &dwSize, &dwIndex) )
					break;

				DWORD ulFileSize = _ttol(lpBuffer);
				if( ulFileSize < 1 )
					break;
				if( ulFileSize > nSize )
					ulFileSize = nSize;

				bRes = true;

				int nReadByteCount = 0;
				DWORD nCurReadByte;
				//参数3,是以BYTE为单位，参数4是lpdwBufferLength 单位也是BYTE
				while( nReadByteCount < ulFileSize && InternetReadFile(file_handle, (unsigned char*)lpGetBuffer+nReadByteCount, ulFileSize-nReadByteCount, &nCurReadByte) )
				{
					nReadByteCount += nCurReadByte;
				}
				nSize = nReadByteCount;

			}while(false);

		}
		catch(...)
		{
		}

		InternetCloseHandle(file_handle);
		InternetCloseHandle(internet);
		return bRes;
	}

	bool BasicHttpContext::HttpSendConnectEx( const TCHAR* lpUrl,const TCHAR* lpHttpVerb,BasicBuffer*const pBufOptional,BasicBuffer& getBuffer,int nOutTime /*= 3000*/, int TryCount /*= 1*/ )
	{
		if(NULL == lpUrl || NULL == pBufOptional)
			return false;

		string_t strHttpVerb = BASIC_HTTP_VERB_GET;
		if(NULL != lpHttpVerb)
		{
			strHttpVerb = lpHttpVerb;
		}

		HINTERNET hInternet = NULL;
		HINTERNET hConnect = NULL;
		HINTERNET hRequest = NULL;
		string_t strAgent = _T("Mozilla/5.0 (Windows NT 5.1; rv:19.0) Gecko/20100101 Firefox/19.0");

		//使用IE的默认代理设置(从注册表读取代理)
		hInternet = ::InternetOpen(strAgent.c_str(),INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
		if(NULL==hInternet)
			return false;

		::InternetSetOption(hInternet,INTERNET_OPTION_CONNECT_TIMEOUT ,&nOutTime,sizeof(nOutTime));
		::InternetSetOption(hInternet,INTERNET_OPTION_CONNECT_RETRIES ,&TryCount,sizeof(TryCount));
		::InternetSetOption(hInternet,INTERNET_OPTION_SEND_TIMEOUT, &nOutTime, sizeof(nOutTime));
		::InternetSetOption(hInternet,INTERNET_OPTION_RECEIVE_TIMEOUT, &nOutTime, sizeof(nOutTime));
		
		bool bRes = false;
		try
		{
			do{
				INTERNET_PORT wPort;
				DWORD dwType;
				string_t strServer;
				string_t strObject;
				this->HttpParseUrl(lpUrl,dwType,strServer,strObject,wPort);

				hConnect = ::InternetConnect(hInternet,strServer.c_str(),wPort,NULL,NULL,INTERNET_SERVICE_HTTP,0,0);
				if(NULL == hConnect)
					break;
				//lpszAcceptTypes NULL 默认接收是text/*
				hRequest = ::HttpOpenRequest(hConnect,strHttpVerb.c_str(),strObject.c_str(),HTTP_VERSION,NULL,NULL,INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_EXISTING_CONNECT,NULL);
				if(NULL == hRequest)
					break;

				//pBufOptional->write("\r\n", strlen("\r\n"));
				string_t strHeader = _T("Content-Type:   application/x-www-form-urlencoded\r\n\r\n");
				BOOL bRet = ::HttpSendRequest(hRequest,strHeader.c_str(),strHeader.length(),(LPVOID*)pBufOptional->buffer(),pBufOptional->length());
				if(FALSE == bRet)
					break;
				OutputDebugString(L"================================xxm ");
				DWORD dwStateCode = 0;
				DWORD dwBufferLen = sizeof(DWORD);
				bRet = ::HttpQueryInfo(hRequest,HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER,&dwStateCode,&dwBufferLen,0);
				if(bRet == TRUE && dwStateCode == HTTP_STATUS_OK)
				{
					const int BUFFER_LENGTH = 1024*9;
					char* szBuffer = new char[BUFFER_LENGTH];  //读取文件的缓冲
					if(!szBuffer)
						break;

					DWORD nRead = 0;
					while(::InternetReadFile(hRequest,szBuffer,BUFFER_LENGTH,&nRead) && nRead > 0)
					{
						getBuffer.write(szBuffer,nRead);
					}
					bRes = true;
					delete[] szBuffer;
				}

			}while(false);

		}catch(...)
		{
		}

		::InternetCloseHandle(hRequest);
		::InternetCloseHandle(hConnect);
		::InternetCloseHandle(hInternet);
		char buf[9000] = { 0 };
		sprintf(buf, "xxm = %s ", (char*)getBuffer.buffer());
		OutputDebugStringA(buf);
		return bRes;
	}

	bool BasicHttpContext::HttpParseUrl(const TCHAR* lpUrl,DWORD& dwServiceType,string_t& strSrv,string_t& strObj,INTERNET_PORT& dwPort)
	{
		dwServiceType = 0;
		if (lpUrl == NULL)
			return false;

		bool bOk = false;
		URL_COMPONENTS urlComponents;
		memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
		urlComponents.dwStructSize = sizeof(URL_COMPONENTS);

		urlComponents.dwHostNameLength = INTERNET_MAX_URL_LENGTH;
		urlComponents.lpszHostName = new TCHAR[INTERNET_MAX_URL_LENGTH+1];
		urlComponents.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
		urlComponents.lpszUrlPath = new TCHAR[INTERNET_MAX_URL_LENGTH+1];

		INTERNET_PORT nPort;
		if (HttpParseURLWorker(lpUrl,&urlComponents,dwServiceType,nPort,ICU_BROWSER_MODE))
		{
			dwPort = nPort;
			strSrv = urlComponents.lpszHostName;
			strObj = urlComponents.lpszUrlPath;
			bOk = true;
		}
		delete []urlComponents.lpszHostName;
		delete []urlComponents.lpszUrlPath;

		return bOk;
	}

	bool BasicHttpContext::HttpParseURLWorker(const TCHAR* pstrURL,LPURL_COMPONENTS lpComponents, DWORD& dwServiceType,INTERNET_PORT& nPort, DWORD dwFlags)
	{
		TCHAR* pstrCanonicalizedURL = NULL;
		TCHAR szCanonicalizedURL[INTERNET_MAX_URL_LENGTH];
		DWORD dwNeededLength = INTERNET_MAX_URL_LENGTH;
		BOOL bRetVal;
		bool bMustFree = false;

		// Decoding is done in InternetCrackUrl/UrlUnescape 
		// so we don't need the ICU_DECODE flag here.

		DWORD dwCanonicalizeFlags = dwFlags & (ICU_NO_ENCODE | ICU_NO_META | ICU_ENCODE_SPACES_ONLY | ICU_BROWSER_MODE);

		DWORD dwCrackFlags = 0;

		BOOL bUnescape = FALSE;

		if((dwFlags & (ICU_ESCAPE | ICU_DECODE)) && (lpComponents->dwUrlPathLength != 0) )
		{	
			if (dwFlags & ICU_BROWSER_MODE)
				bUnescape = TRUE;
			else
				dwCrackFlags |= ICU_ESCAPE;
		}

		bRetVal = InternetCanonicalizeUrl(pstrURL, szCanonicalizedURL,&dwNeededLength, dwCanonicalizeFlags);

		if (!bRetVal)
		{
			if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
				return FALSE;

			pstrCanonicalizedURL = new TCHAR[dwNeededLength];

			if (pstrCanonicalizedURL == NULL)
				return FALSE;

			bMustFree = TRUE;
			bRetVal = InternetCanonicalizeUrl(pstrURL, pstrCanonicalizedURL,&dwNeededLength, dwCanonicalizeFlags);
			if (!bRetVal)
			{
				delete [] pstrCanonicalizedURL;
				return FALSE;
			}
		}
		else
		{
			pstrCanonicalizedURL = szCanonicalizedURL;
		}
		bRetVal = InternetCrackUrl(pstrCanonicalizedURL, 0,dwCrackFlags, lpComponents);

		if(bUnescape)
		{
			if (_tcslen(lpComponents->lpszUrlPath) >= INTERNET_MAX_URL_LENGTH || 
				FAILED(UrlUnescape(lpComponents->lpszUrlPath,NULL,NULL,URL_UNESCAPE_INPLACE | URL_DONT_UNESCAPE_EXTRA_INFO)))
			{
				if (bMustFree)
					delete [] pstrCanonicalizedURL;

				return false;
			}
			lpComponents->dwUrlPathLength = lstrlen(lpComponents->lpszUrlPath);
		}

		if (bMustFree)
		{
			delete [] pstrCanonicalizedURL;
		}

		if (!bRetVal)
		{
			dwServiceType = 0;
		}
		else
		{
			nPort = lpComponents->nPort;
			dwServiceType = lpComponents->nScheme;
		}
		return !!bRetVal;
	}

	
}