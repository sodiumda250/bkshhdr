/*
 * $Id: hdrlist.cpp,v 1.12 2005/04/26 05:32:45 woods Exp $
 */

static char id[] = "$Id: hdrlist.cpp,v 1.12 2005/04/26 05:32:45 woods Exp $";

#include <stdio.h>
#include <windows.h>

#include "hdrlist.h"

/**
 * @brief ini�t�@�C������\���w�b�_�̃��X�g���l������B
 * @param szIni ini�t�@�C���̃p�X��
 * @param buf ���X�g���i�[����o�b�t�@
 * @param size �o�b�t�@�T�C�Y
 *
 * ini�t�@�C������\���w�b�_�̃��X�g���l������B
 */
int LoadList(const char *szIni, char *buf, const int size)
{
    char section[1024];
    const char *p;
    const char *pp;
    int max;
    int len = 0;
    const char * const limit = buf + (size - 1);

    // ini�t�@�C���̃Z�N�V���������\���w�b�_���Ȃ̂ŃZ�N�V��������S�Ď擾����
    max = ::GetPrivateProfileSectionNames(section, sizeof(section), szIni);

    if (strlen(section) == 0) {
        memset(buf, 0, sizeof(buf));
        strncpy(buf, "X-Mailer", size);
        return 1;
    }

    // �Z�N�V������(�w�b�_��)���Ƃ̏���
    p = section, pp = section;
    while ((*p != 0) && (p < section + max)) {
        int show;

        while (*pp != 0) {
            pp++;
        }
        // "Show"�L�[��0�Ȃ��\��
        show = ::GetPrivateProfileInt(p, "Show", 1, szIni);
        if (show != 0) {
            len++;
            while (*p != 0) {
                *(buf++) = *(p++);
                if (buf >= limit) {
                    *buf = 0;
                    return len;
                }
            }
            if (buf < limit) {
                *(buf++) = ',';
            } else {
                *buf = 0;
                return len;
            }
        }
        pp++;
        p = pp;
    }
    *(--buf) = 0;

    return len;
}

/**
 * @brief �\���w�b�_�̃��X�g��ini�t�@�C���ɔ��f����
 * @param szIni ini�t�@�C���̃p�X��
 * @param buf ���X�g���i�[����Ă���o�b�t�@
 *
 * �\���w�b�_�̃��X�g��ini�t�@�C���ɔ��f����B
 */
int SaveList(const char *szIni, const char *buf)
{
    const char *plist[512];
    size_t plen[512];
    char header[128];
    char section[1024];
    const char *p;
    const char *pp;
    int i;
    int max;
    int len = 0;

    // ini�t�@�C���̃Z�N�V���������\���w�b�_���Ȃ̂ŃZ�N�V��������S�Ď擾����
    max = ::GetPrivateProfileSectionNames(section, sizeof(section), szIni);

    plist[0] = buf;
    plen[0] = 0;
    size_t hlen = 0;
    while (*buf != 0) {
        if (*buf != ',') {
            hlen++;
        } else {
            plen[len++] = hlen;
            plist[len] = buf + 1;
            hlen = 0;
        }
        buf++;
    }
    if (hlen != 0) {
        plen[len++] = hlen;
    }
    for (i = 0; i < len; i++) {
        if (plen[i] >= sizeof(header)) {
            plen[i] = sizeof(header) - 1;
        }
        memcpy(header, plist[i], plen[i]);
        header[plen[i]] = 0;
        ::WritePrivateProfileString(header, "Show",
                                    "1", szIni);
    }

    // ini�t�@�C���̒��g����Ȃ�������݂͍s��Ȃ��B
    if (strlen(section) != 0) {
        // �Z�N�V������(�w�b�_��)���Ƃ̏���
        p = section, pp = section;
        while ((*p != 0) && (p < section + max)) {
            while (*pp != 0) {
                pp++;
            }

            for (i = 0; i < len; i++) {
                if (strncmp(p, plist[i], plen[i]) == 0) {
                    break;
                }
            }
            if (i >= len) {
            // ���X�g���Ƀw�b�_���Ȃ������̂Ŕ�\����
                ::WritePrivateProfileString(p, "Show",
                                            "0", szIni);
            }
            pp++;
            p = pp;
        }
    }
    return len;
}
