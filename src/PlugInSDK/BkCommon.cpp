///////////////////////////////////////////////////////////////////////////
// Common routines which may be useful for developing Becky!'s plugins.
//
// You can modify and redistribute this file without any permission.

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <mbstring.h>
#include <time.h>
#include "BkCommon.h"
#include "BeckyAPI.h"


//////////////////////////////////////////////////////////////////////////////////////
// Common routines
LPSTR __stristr(LPSTR lpStr, LPSTR lpSch)
{
	int nLen = strlen(lpSch);
	while (*lpStr) {
		if (strnicmp(lpStr, lpSch, nLen) == 0) {
			return lpStr;
		}
		lpStr++;
	}
	return NULL;
}

BOOL IsFileExist(LPCTSTR lpszFileName)
{
	WIN32_FIND_DATA FindData;
	HANDLE hFind = ::FindFirstFile(lpszFileName, &FindData);
	if (hFind != INVALID_HANDLE_VALUE) {
		::FindClose(hFind);
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		} else {
			return TRUE;
		}
	}
	return FALSE;
}

void GetLegalFileName(LPCTSTR lpName, LPSTR lpBuf, int nBuf)
{
	LPSTR lpPtr = lpBuf;
	LPCTSTR lpSub = lpName;
	int nLen = 0;
	while (*(LPBYTE)lpSub <= (BYTE)'\x20') lpSub++;
	while (*lpSub) {
		char c = *lpSub++;
		nLen++;
		if ((lpPtr - lpBuf) >= nBuf-3) {
			break;
		}
		if (_ismbblead(c)) {
			*lpPtr++ = c;
			*lpPtr++ = *lpSub;
			nLen++;
			if (*lpSub == '\0') break;
			else lpSub++;
		} else if ((BYTE)c < (BYTE)'\x20') {
			c = ' ';
			*lpPtr++ = c;
		} else {
			switch (c) {
			case '*':
			case '<':
			case '>':
			case '?':
			case '\\':
			case '|':
			case '/':
				c = '=';
				break;
			case ':':
				c = ';';
				break;
			case '\"':
				c = '\'';
				break;
			default:
				break;
			}
			*lpPtr++ = c;
		}
	}
	while (nLen && (*(lpPtr-1) == '.' || *(lpPtr-1) == ' ')) {
		lpPtr--;
		nLen--;
	}
	*lpPtr++ = '\0';
	if (*lpBuf == '\0') {
		strcpy(lpBuf, "(noname)");
	}
}

LPSTR FileToString(LPCTSTR lpszFileName)
{
	LPSTR lpStr = NULL;
	HANDLE hFile;
	OFSTRUCT of;
	of.cBytes = sizeof(OFSTRUCT);
	DWORD lSize;
	if ( (hFile = (HANDLE)OpenFile(lpszFileName, &of, OF_READ)) != (HANDLE)HFILE_ERROR) {
		int nSize = GetFileSize(hFile, NULL);
		if (!nSize) {
			CloseHandle(hFile);
			return NULL;
		}
		lpStr = (LPSTR)malloc(nSize + 10);
		if (lpStr) {
			if (!ReadFile(hFile, lpStr, nSize, &lSize, NULL)) return NULL;
			CloseHandle(hFile);
			lpStr[lSize] = '\0';
		}
	}
	return lpStr;
}


//int sGets(LPCTSTR& lpsz, LPTSTR lpszBuf, int nMax)
int sGets(LPSTR& lpsz, LPTSTR lpszBuf, int nMax)
{
	LPSTR lpOrg = lpszBuf;
	char ch = *lpsz++;
	if (ch == '\0') {
		lpsz--;
		return 0;
	}
	nMax--;
	while (ch) {
		if (!nMax) {
			*lpszBuf = '\0';
			return (int)(lpszBuf - lpOrg);
		}
		switch (ch) {
		case '\r':
			if (*lpsz == '\n') {
				lpsz++;
			}
			ch = '\n';
		case '\n':
			*lpszBuf++ = ch;
			*lpszBuf = '\0';
			return (int)(lpszBuf - lpOrg);
		default:
			*lpszBuf++ = ch;
			break;
		}
		nMax--;
		ch = *lpsz++;
	}
	*lpszBuf = '\0';
	lpsz--;
	return (int)(lpszBuf - lpOrg);
}

BOOL IsLWSP(char c)
{
	if (c == '\x20' || c == '\t' || c == '\n' || c == '\r') return TRUE;
	return FALSE;
}

LPSTR GetOptValue(LPSTR lpOption, LPSTR& lpOpt, LPSTR& lpVal)
{
	while (*lpOption == '\x20' || *lpOption == '\t' || *lpOption == '\r' || *lpOption == '\n' || *lpOption == ';') lpOption++;
	lpOpt = lpVal = NULL;
	lpVal = strchr(lpOption, '=');
	if (!lpVal) return NULL;
	*lpVal = '\0';
	lpVal++;
	lpOpt = lpOption;
	while (*lpVal == '\t' || *lpVal == '\x20' || *lpVal == '\r' || *lpVal == '\n') lpVal++;
	LPSTR lpEnd;
	if (*lpVal == '\"') {
		lpVal++;
		lpEnd = lpVal;
		while (*lpEnd != '\"' && *lpEnd) lpEnd++;
		if (*lpEnd) {
			lpOption = lpEnd + 1;
			*lpEnd = '\0';
		} else {
			lpOption = lpEnd;
		}
	} else {
		lpEnd = lpVal;
		while (*lpEnd != '\t' && *lpEnd != '\x20' && *lpEnd != ';' && 
			   *lpEnd != '\r' && *lpEnd != '\n'  && *lpEnd) lpEnd++;
		if (*lpEnd) {
			*lpEnd = '\0';
			lpOption = lpEnd + 1;
		} else {
			lpOption = lpEnd;
		}
	}
	while (*lpOption == ';' || *lpOption == '\x20' || *lpOption == '\t'
			|| *lpOption == '\r' || *lpOption == '\n') lpOption++;
	return lpOption;
}

LPSTR GetFnameTop(LPCTSTR pszPath, LPCTSTR lpDelim)
{
	LPSTR pTop = (LPSTR)pszPath;
	if (lpDelim) {
		int nLen = strlen(lpDelim);
		if (lpDelim[0] != '\0') {
			char c;
			while((c = *pszPath++) != '\0') {
				if (_ismbblead(c)) {
					pszPath++;
				} else if (c == lpDelim[0]) {
					if (strncmp(pszPath-1, lpDelim, nLen) == 0) {
						if (pszPath[nLen-1] == '\0') return pTop;
						pTop = (LPSTR)(pszPath -1 + nLen);
					}
				}
			}
		}
	} else {
		char c;
		while((c = *pszPath++) != '\0') {
			if (_ismbblead(c)) {
				pszPath++;
			} else if (c == '\\') {
				if (*pszPath == '\0') {
					return pTop;
				}
				pTop = (LPSTR)pszPath;
			}
		}
	}
	return pTop;
}

LPSTR TokenAddr(LPSTR lpAddr)
{
	static LPSTR lpOrg = NULL;
	if (lpAddr) lpOrg = lpAddr;
	LPSTR lpEnd, lpRet;
	BOOL bQ = FALSE;
	if (lpOrg) {
		while (*lpOrg &&
			(*(LPBYTE)lpOrg <= (BYTE)'\x20' || *lpOrg == ',' || *lpOrg == ';')) lpOrg++;
		if (*lpOrg == '(') {
			while (*lpOrg != ')') {
				if (*lpOrg == '\0') return NULL;
				lpOrg++;
			}
			lpOrg++;
			while (*lpOrg &&
				(*(LPBYTE)lpOrg <= (BYTE)'\x20' || *lpOrg == ',' || *lpOrg == ';')) lpOrg++;
		}
		lpEnd = lpOrg;
		while (*lpEnd) {
			char ch = *lpEnd;
			if (_ismbblead(ch)) {
				lpEnd += 2;
			} else {
				while (ch == '(') {
					while (*lpEnd != ')' && *lpEnd) {
						lpEnd++;
					}
					ch = *lpEnd;
				}
				if (ch == '\"') bQ ^= TRUE;
				else if ((ch == ',' || ch == ';') && !bQ) {
					*lpEnd = '\0';
					LPSTR lpBack = lpEnd;
					while (*(LPBYTE)(lpBack-1) <= (BYTE)'\x20' && lpBack > lpOrg) {
						lpBack--;
						*lpBack = '\0';
					}
					lpRet = lpOrg;
					lpOrg = lpEnd + 1;
					return lpRet;
				}
				lpEnd++;
			}
		}
		LPSTR lpRet = lpOrg;
		lpOrg = NULL;
		if (lpRet[0] == '\0') {
			return NULL;
		} else {
			LPSTR lpBack = lpEnd;
			while (*(LPBYTE)(lpBack-1) <= (BYTE)'\x20' && lpBack > lpRet) {
				lpBack--;
				*lpBack = '\0';
			}
			return lpRet;
		}
	} else return NULL;
}

void GetNameAndAddr(LPSTR lpszName, int nLenName, LPSTR lpszEMail, int nLenEMail, LPCTSTR lpszSrc)
{
	LPSTR lpsz = strdup(lpszSrc);
	LPSTR lpBuf = lpsz;
	while (*lpsz == '\x20' || *lpsz == '\t') lpsz++;
	BOOL bQ = FALSE;
	LPSTR lpszAddr = lpsz;
	while (TRUE) {
		BYTE ch = *lpszAddr;
		if (_ismbblead(ch)) {
			lpszAddr++;
		} else if (ch == '\"') {
			bQ ^= TRUE;
		} else if (ch == '<' && !bQ) {
			break;
		}
		lpszAddr++;
		if (!*lpszAddr) {
			lpszAddr = NULL;
			break;
		}
	}
	LPSTR lpszTmp;
	if (lpszAddr) {
		lpszAddr++;
		lpszTmp = (LPSTR)_mbstok((LPBYTE)lpszAddr, (LPBYTE)">");
		if (lpszTmp) {
			while (*lpszTmp == ' ' || *lpszTmp == '\t') lpszTmp++;
			strncpy(lpszEMail, lpszTmp, nLenEMail-1);
			lpszEMail[nLenEMail-1] = '\0';
		} else {
			strcpy(lpszEMail, "");
		}
		if (*lpsz != '<') {
			if (*lpsz == '\"') {
				lpszTmp = (LPSTR)_mbstok((LPBYTE)lpsz, (LPBYTE)"\"");
			} else {
				lpszTmp = (LPSTR)_mbstok((LPBYTE)lpsz, (LPBYTE)"<");
			}
			if (lpszTmp) {
				while (*lpszTmp == ' ' || *lpszTmp == '\t') lpszTmp++;
				strncpy(lpszName, lpszTmp, nLenName-1);
				lpszName[nLenName-1] = '\0';
			} else {
				strcpy(lpszName, "");
			}
		}
	} else {
		lpszAddr = (LPSTR)_mbschr((LPBYTE)lpsz, (BYTE)'(');
		if (lpszAddr) {
			lpszAddr++;
			lpszTmp = (LPSTR)_mbstok((LPBYTE)lpszAddr, (LPBYTE)")");
			if (lpszTmp) {
				while (*lpszTmp == ' ' || *lpszTmp == '\t') lpszTmp++;
				strncpy(lpszName, lpszTmp, nLenName-1);
			} else {
				strcpy(lpszName, "");
			}
			lpszName[nLenName-1] = '\0';
			lpszTmp = (LPSTR)_mbstok((LPBYTE)lpsz, (LPBYTE)"(");
			if (lpszTmp) {
				while (*lpszTmp == ' ' || *lpszTmp == '\t') lpszTmp++;
				strncpy(lpszEMail, lpszTmp, nLenEMail-1);
				lpszEMail[nLenEMail-1] = '\0';
			} else {
				strcpy(lpszEMail, "");
			}
		} else {
			strcpy(lpszName, "");
			strncpy(lpszEMail, lpsz, nLenEMail-1);
			lpszEMail[nLenEMail-1] = '\0';
		}
	}
	int nLen;
	nLen = strlen(lpszName);
	if (nLen) {
		nLen--;
		while (nLen && (lpszName[nLen] == ' ' || lpszName[nLen] == '\t')) {
			lpszName[nLen] = '\0';
			nLen--;
		}
	}
	nLen = strlen(lpszEMail);
	if (nLen) {
		nLen--;
		while (nLen && (lpszEMail[nLen] == ' ' || lpszEMail[nLen] == '\t')) {
			lpszEMail[nLen] = '\0';
			nLen--;
		}
	}
	free (lpBuf);
}

/////////////////////////////////////////////////////////////////////////////////
// Collection classes
CPointerList::CPointerList()
{
	m_lpTopItem = NULL;
	m_lpTailItem = NULL;
}

CPointerList::~CPointerList()
{
	Empty();
}

void CPointerList::Empty()
{
	CPointerItem* pItem = GetTop();
	while (pItem) {
		CPointerItem* pNext = pItem->GetNext();
		delete pItem;
		pItem = pNext;
	}
	m_lpTopItem = m_lpTailItem = NULL;
}

void CPointerList::Remove(CPointerItem* pItem)
{
	CPointerItem* pPrev = pItem->GetPrev();
	CPointerItem* pNext = pItem->GetNext();
	if (pPrev) {
		pPrev->SetNext(pNext);
	}
	if (pNext) {
		pNext->SetPrev(pPrev);
	}
	pItem->SetPrev(NULL);
	pItem->SetNext(NULL);
	if (m_lpTopItem == pItem) {
		m_lpTopItem = pNext;
	}
	if (m_lpTailItem == pItem) {
		m_lpTailItem = pPrev;
	}
}

CPointerItem* CPointerList::AddTail(LPSTR lpStr)
{
	CPointerItem* pItem = new CPointerItem;
	pItem->SetData(lpStr);
	pItem->SetPrev(m_lpTailItem);
	if (m_lpTailItem) {
		m_lpTailItem->SetNext(pItem);
	}
	m_lpTailItem = pItem;
	if (!m_lpTopItem) m_lpTopItem = pItem;
	return pItem;
}

CPointerItem* CPointerList::InsertBefore(CPointerItem* pHere, LPSTR lpStr)
{
	CPointerItem* pItem = new CPointerItem;
	pItem->SetData(lpStr);
	CPointerItem* pPrev = pHere->GetPrev();
	pItem->SetPrev(pPrev);
	if (pPrev) {
		pPrev->SetNext(pItem);
	} else {
		m_lpTopItem = pItem;
	}
	pHere->SetPrev(pItem);
	pItem->SetNext(pHere);
	return pItem;
}

CMIMEItem::CMIMEItem()
{
	m_lpNext = NULL;
	m_lpChild = NULL;
	m_lpBoundary = NULL;
}


CMIMEItem::~CMIMEItem()
{
	if (m_lpChild) DeleteItem(m_lpChild);
	if (m_lpBoundary) {
		free(m_lpBoundary);
	}
}

void CMIMEItem::DeleteItem(CMIMEItem* pItem)
{
	if (pItem->m_lpNext) {
		DeleteItem(pItem->m_lpNext);
	}
	delete pItem;
}

CMIMEItem* CMIMEItem::FindMIMEItem(LPCTSTR lpszType, LPCTSTR lpszSubType)
{
	if ((!lpszType || (lpszType && stricmp(lpszType, m_szType) == 0)) &&
		(!lpszSubType || (lpszSubType && stricmp(lpszSubType, m_szSubType) == 0)) ) {
		return this;
	}
	CMIMEItem* pMimeItem = NULL;
	if (GetChild()) {
		if (pMimeItem = GetChild()->FindMIMEItem(lpszType, lpszSubType)) {
			return pMimeItem;
		}
	}
	if (GetNext()) {
		if (pMimeItem = GetNext()->FindMIMEItem(lpszType, lpszSubType)) {
			return pMimeItem;
		}
	}
	return NULL;
}

void CMIMEItem::GetHeader(LPCTSTR lpHeader, LPSTR lpszBuf, int nBuf)
{
	int nHead = strlen(lpHeader);
	CPointerItem* pItem = m_lstHeaders.GetTop();
	while (pItem) {
		if (strnicmp(lpHeader, pItem->GetData(), nHead) == 0 &&
			*(LPSTR)(pItem->GetData() + nHead) == ':') {
			LPSTR lp = pItem->GetData() + nHead + 1;
			while (IsLWSP(*lp)) lp++;
			strncpy(lpszBuf, lp, nBuf-1);
			lpszBuf[nBuf-1] = '\0';
			return;
		}
		pItem = pItem->GetNext();
	}
	*lpszBuf = '\0';
}

void CMIMEItem::SetHeader(LPCTSTR lpHeader, LPCTSTR lpData)
{
	int nHead = strlen(lpHeader);
	int nData = strlen(lpData);
	LPSTR lpField = (LPSTR)malloc(nData + nHead + 4);
	sprintf(lpField, "%s: %s", lpHeader, lpData);
	CPointerItem* pItem = m_lstHeaders.GetTop();
	while (pItem) {
		if (strnicmp(lpHeader, pItem->GetData(), nHead) == 0 &&
			*(LPSTR)(pItem->GetData() + nHead) == ':') {
			if (*lpData) {
				pItem->SetData(lpField);
			} else {
				m_lstHeaders.Remove(pItem);
				delete pItem;
			}
			free(lpField);
			return;
		}
		pItem = pItem->GetNext();
	}
	m_lstHeaders.InsertBefore(m_lstHeaders.GetTail(), lpField);
	free(lpField);
}

void CMIMEItem::FromString(LPSTR& lpSrc, LPCTSTR lpBoundParent)
{
	char szBuf[65540];
	int n;
	BOOL bBody = FALSE;
	LPSTR lpOrg = lpSrc;
	int nBound = 0;
	int nBoundParent = 0;
	m_lstHeaders.Empty();
	m_lstBody.Empty();
	m_lstTrail.Empty();
	while (n = sGets(lpSrc, szBuf, 65536)) {
		szBuf[n-1] = '\r';
		szBuf[n] = '\n';
		szBuf[n+1] = '\0';
		n++;
		if (bBody) {
			if (strncmp(szBuf, "--", 2) == 0) {
				szBuf[n-2] = '\0'; n -= 2;
				if (m_lpBoundary) { // multipart
					if (!nBound) nBound = strlen(m_lpBoundary);
					if (strcmp(&szBuf[2], m_lpBoundary) == 0) {
						CMIMEItem* pSub = new CMIMEItem;
						pSub->FromString(lpSrc, m_lpBoundary);
						if (!m_lpChild) {
							m_lpChild = pSub;
						} else {
							CMIMEItem* pNext = m_lpChild;
							while (pNext->m_lpNext) {
								pNext = pNext->m_lpNext;
							}
							pNext->m_lpNext = pSub;
						}
						lpOrg = lpSrc;
						continue;
					} else 
					if (strncmp(&szBuf[2], m_lpBoundary, nBound) == 0 &&
						strcmp(&szBuf[nBound+2], "--") == 0) {
						if (lpBoundParent) {
							lpOrg = lpSrc;
							while (n = sGets(lpSrc, szBuf, 65536)) {
								szBuf[n-1] = '\r';
								szBuf[n] = '\n';
								szBuf[n+1] = '\0';
								n++;
								if (!nBoundParent) nBoundParent = strlen(lpBoundParent);
								if (strncmp(&szBuf[2], lpBoundParent, nBoundParent) == 0) {
									lpSrc = lpOrg;
									break;
								}
								m_lstTrail.AddTail(szBuf);
								lpOrg = lpSrc;
							}
						}
						break;
					}
				}
				if (lpBoundParent) {
					if (!nBoundParent) nBoundParent = strlen(lpBoundParent);
					if (strncmp(&szBuf[2], lpBoundParent, nBoundParent) == 0) {
						lpSrc = lpOrg;
						break;
					}
				}
				szBuf[n] = '\r'; n += 2;
			}
			m_lstBody.AddTail(szBuf);
		} else {
			if (szBuf[0] == '\r' && szBuf[1] == '\n') {
				m_lstHeaders.AddTail(szBuf);
				bBody = TRUE;
			} else {
				while (*lpSrc == ' ' || *lpSrc == '\t') {
					LPSTR lpBuf = &szBuf[n];
					int nn = sGets(lpSrc, lpBuf, 65536-n);
					lpBuf[nn-1] = '\r';
					lpBuf[nn] = '\n';
					lpBuf[nn+1] = '\0';
					nn++;
					n += nn;
					if (n >= 65536) {
						// Header too long. Give up.
						break;
					}
				}
				m_lstHeaders.AddTail(szBuf);
				if (strnicmp(szBuf, "Content-Type:", 13) == 0) {
					LPSTR lpsz = &szBuf[13];
					LPSTR lpType = strtok(lpsz, "/ \t");
					LPSTR lpSubType = strtok(NULL, "; \r\n\t");
					if (lpType && lpSubType) {
						strncpy(m_szType, lpType, 40); m_szType[40] = '\0';
						strncpy(m_szSubType, lpSubType, 40); m_szSubType[40] = '\0';

						LPSTR lpOpt, lpVal;
						LPSTR lpOption = strtok(NULL, "");
						if (lpOption) {
							lpOption = GetOptValue(lpOption, lpOpt, lpVal);
							while (lpOption) {
								if (stricmp(lpOpt, "boundary") == 0) {
									if (m_lpBoundary) free(m_lpBoundary);
									m_lpBoundary = strdup(lpVal);
								}
								if (*lpOption == '\0') break;
								lpOption = GetOptValue(lpOption, lpOpt, lpVal);
							}
						}
					}
				}

			}
		}
		lpOrg = lpSrc;
	}
}

LPSTR CMIMEItem::ToString()
{
	LPSTR lpBuf = (LPSTR)malloc(65536);
	LPSTR lpPtr = lpBuf;
	*lpPtr = '\0';
	int nSize = 65536;
	CPointerItem* pItem = m_lstHeaders.GetTop();
	while (pItem) {
		if (!pItem->GetLength()) break; // Should not happen, though.
		while (lpPtr + pItem->GetLength() >= &lpBuf[nSize]) {
			nSize += 65536;
			int nDif = lpPtr - lpBuf;
			lpBuf = (LPSTR)realloc(lpBuf, nSize);
			lpPtr = lpBuf + nDif;
		}
		strcat(lpPtr, pItem->GetData());
		lpPtr += pItem->GetLength();
		pItem = pItem->GetNext();
	}
	pItem = m_lstBody.GetTop();
	while (pItem) {
		if (!pItem->GetLength()) break;
		while (lpPtr + pItem->GetLength() >= &lpBuf[nSize]) {
			nSize += 65536;
			int nDif = lpPtr - lpBuf;
			lpBuf = (LPSTR)realloc(lpBuf, nSize);
			lpPtr = lpBuf + nDif;
		}
		strcat(lpPtr, pItem->GetData());
		lpPtr += pItem->GetLength();
		pItem = pItem->GetNext();
	}
	if (GetChild()) {
		CMIMEItem* pMimeItem = GetChild();
		int nBound = strlen(m_lpBoundary);
		do {
			while (lpPtr + nBound + 4 >= &lpBuf[nSize]) {
				nSize += 65536;
				int nDif = lpPtr - lpBuf;
				lpBuf = (LPSTR)realloc(lpBuf, nSize);
				lpPtr = lpBuf + nDif;
			}
			strcat(lpPtr, "--");
			strcat(lpPtr, m_lpBoundary);
			strcat(lpPtr, "\r\n");
			lpPtr += nBound + 4;
			LPSTR lpSub = pMimeItem->ToString();
			int nSub = strlen(lpSub);
			while (lpPtr + nSub + 10 >= &lpBuf[nSize]) {
				nSize += 65536;
				int nDif = lpPtr - lpBuf;
				lpBuf = (LPSTR)realloc(lpBuf, nSize);
				lpPtr = lpBuf + nDif;
			}
			strcat(lpPtr, lpSub);
			lpPtr += nSub;
			free(lpSub);
			pMimeItem = pMimeItem->GetNext();
		} while (pMimeItem);

		while (lpPtr + nBound + 6 >= &lpBuf[nSize]) {
			nSize += 65536;
			int nDif = lpPtr - lpBuf;
			lpBuf = (LPSTR)realloc(lpBuf, nSize);
			lpPtr = lpBuf + nDif;
		}
		strcat(lpPtr, "--");
		strcat(lpPtr, m_lpBoundary);
		strcat(lpPtr, "--\r\n");
		lpPtr += nBound + 6;
		pItem = m_lstTrail.GetTop();
		while (pItem) {
			if (!pItem->GetLength()) break;
			while (lpPtr + pItem->GetLength() >= &lpBuf[nSize]) {
				nSize += 65536;
				int nDif = lpPtr - lpBuf;
				lpBuf = (LPSTR)realloc(lpBuf, nSize);
				lpPtr = lpBuf + nDif;
			}
			strcat(lpPtr, pItem->GetData());
			lpPtr += pItem->GetLength();
			pItem = pItem->GetNext();
		}
	}
	lpBuf = (LPSTR)realloc(lpBuf, (lpPtr - lpBuf) + 2);
	return lpBuf;
}
