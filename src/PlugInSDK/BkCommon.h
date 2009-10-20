///////////////////////////////////////////////////////////////////////////
// Common routines which may be useful for developing Becky!'s plugins.
//
// You can modify and redistribute this file without any permission.

// Search string case insensitively.
LPSTR __stristr(LPSTR lpStr, LPSTR lpSch);

// Get a file name from a string
void GetLegalFileName(LPCTSTR lpName, LPSTR lpBuf, int nBuf);

// Read a file and return as a string. (must be free()'d later)
LPSTR FileToString(LPCTSTR lpszFileName);

// Load a string from a buf and move the pointer. \r\n will be converted to \n
int sGets(LPCTSTR& lpsz, LPTSTR lpszBuf, int nMax);

// Check if a character is a linear white space.
BOOL IsLWSP(char c);

// Get a filename part from a full path name.
// if lpDelim is NULL, '\' is used as default.
LPSTR GetFnameTop(LPCTSTR pszPath, LPCTSTR lpDelim);

// Parse "opt=val; opt2=val" string. Returns a pointer to the next "opt=val" conbination.
LPSTR GetOptValue(LPSTR lpOption, LPSTR& lpOpt, LPSTR& lpVal);

// Parse multiple e-mail addresses separated by ",".
LPSTR TokenAddr(LPSTR lpAddr);

// Parse an e-mail address to name and address.
// e.g. Name <mail@address> --> "Name" and "mail@address"
void GetNameAndAddr(LPSTR lpszName, int nLenName, LPSTR lpszEMail, int nLenEMail, LPCTSTR lpszSrc);

// File existing check
BOOL IsFileExist(LPCTSTR lpszFileName);

///////////////////////////////////////////////////////////////////////////////////////
// Common classes

// Manage string pointer
class CPointerItem
{
public:
	CPointerItem()
	{
		m_lpData = NULL;
		m_lpNext = NULL;
		m_lpPrev = NULL;
		m_nLen = 0;
	}
	~CPointerItem()
	{
		if (m_lpData) {
			free(m_lpData);
		}
	}
	void SetData(LPCTSTR lpStr)
	{
		if (m_lpData) free(m_lpData);
		m_lpData = strdup(lpStr);
		m_nLen = strlen(m_lpData);
	}
	int GetLength() { return m_nLen; }
	LPSTR GetData()
	{
		return m_lpData;
	}
	CPointerItem* GetNext()
	{
		return m_lpNext;
	}
	void SetNext(CPointerItem* pItem)
	{
		m_lpNext = pItem;
	}
	CPointerItem* GetPrev()
	{
		return m_lpPrev;
	}
	void SetPrev(CPointerItem* pItem)
	{
		m_lpPrev = pItem;
	}
protected:
	LPSTR m_lpData;
	CPointerItem* m_lpNext;
	CPointerItem* m_lpPrev;
	int m_nLen;
};

// Manage a list of string pointer
class CPointerList
{
public:
	CPointerList();
	~CPointerList();
	CPointerItem* AddTail(LPSTR lpStr);
	CPointerItem* InsertBefore(CPointerItem* pHere, LPSTR lpStr);
	void Remove(CPointerItem* pItem);
	CPointerItem* GetTop()
	{
		return m_lpTopItem;
	}
	CPointerItem* GetTail()
	{
		return m_lpTailItem;
	}
	void Empty();
protected:
	CPointerItem* m_lpTopItem;
	CPointerItem* m_lpTailItem;
};

// Manage e-mail message
class CMIMEItem
{
public:
	CMIMEItem();
	~CMIMEItem();
	void FromString(LPSTR& lpSrc, LPCTSTR lpBoundary=NULL);
	LPSTR ToString();
	static void DeleteItem(CMIMEItem* pItem);
	void GetHeader(LPCTSTR lpHeader, LPSTR lpszBuf, int nBuf);
	void SetHeader(LPCTSTR lpHeader, LPCTSTR lpData);
	CMIMEItem* FindMIMEItem(LPCTSTR lpszType, LPCTSTR lpszSubType);

	CMIMEItem* GetNext()
	{
		return m_lpNext;
	}
	CMIMEItem* GetChild()
	{
		return m_lpChild;
	}
	void SetChild(CMIMEItem* pChild)
	{
		if (m_lpChild) DeleteItem(m_lpChild);
		m_lpChild = pChild;
	}

	char m_szType[42];
	char m_szSubType[42];
	LPSTR m_lpBoundary;
	CPointerList m_lstHeaders;
	CPointerList m_lstBody;
	CPointerList m_lstTrail;

protected:
	CMIMEItem* m_lpNext;
	CMIMEItem* m_lpChild;
};


