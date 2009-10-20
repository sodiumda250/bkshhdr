/* $Id: hdrlist.h,v 1.2 2005/04/26 03:32:43 woods Exp $ */

#ifndef HDRLIST_H
#define HDRLIST_H

int LoadList(const char *szIni, char *buf, const int size);
int SaveList(const char *szIni, const char *buf);

#endif /* HDRLIST_H */
