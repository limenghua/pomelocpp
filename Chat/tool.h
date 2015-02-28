#ifndef __TOOL_H__
#define __TOOL_H__

inline CString UTF8ToUnicode( const char* str )
{
	int  len = 0;
	len = strlen(str);
	int  unicodeLen = ::MultiByteToWideChar( CP_UTF8,
		0,
		str,
		-1,
		NULL,
		0 ); 
	wchar_t *  pUnicode; 
	pUnicode = new  wchar_t[unicodeLen+1];
	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t)); 
	::MultiByteToWideChar( CP_UTF8,
		0,
		str,
		-1,
		(LPWSTR)pUnicode,
		unicodeLen ); 
	CString  rt;
	rt = ( wchar_t* )pUnicode;
	delete  pUnicode;

	return  rt;
}
inline std::string UnicodeToUTF8( const CString& str )
{  

	char*     pElementText;
	int    iTextLen;  // wide char to multi char  
	iTextLen = WideCharToMultiByte( CP_UTF8,
		0,
		str,
		-1,
		NULL,
		0,
		NULL,
		NULL );
	pElementText = new char[iTextLen + 1];
	memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
	::WideCharToMultiByte( CP_UTF8,
		0,
		str,
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL );
	std::string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

#endif