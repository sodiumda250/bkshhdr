/* $Id: bkshhdr.h,v 1.2 2005/04/19 04:46:16 woods Exp $ */

#ifndef BKSHHDR_H_
#define BKSHHDR_H_

#include "PlugInSDK/BeckyAPI.h"

/// Becky! API�p�C���X�^���X
extern CBeckyAPI bka;  // You can have only one instance in a project.

/// DLL�̃C���X�^���X�n���h��
extern HINSTANCE g_hInstance;

/// ini�t�@�C���̃p�X��
extern char szIni[_MAX_PATH + 2]; // Ini file to save your plugin settings.

#endif /* BKSHHDR_H_ */
