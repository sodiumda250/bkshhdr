/*
 * $Id: hdrlist.cpp,v 1.12 2005/04/26 05:32:45 woods Exp $
 */

static char id[] = "$Id: hdrlist.cpp,v 1.12 2005/04/26 05:32:45 woods Exp $";

#include <stdio.h>
#include <windows.h>

#include "hdrlist.h"

/**
 * @brief iniファイルから表示ヘッダのリストを獲得する。
 * @param szIni iniファイルのパス名
 * @param buf リストを格納するバッファ
 * @param size バッファサイズ
 *
 * iniファイルから表示ヘッダのリストを獲得する。
 */
int LoadList(const char *szIni, char *buf, const int size)
{
    char section[1024];
    const char *p;
    const char *pp;
    int max;
    int len = 0;
    const char * const limit = buf + (size - 1);

    // iniファイルのセクション名が表示ヘッダ名なのでセクション名を全て取得する
    max = ::GetPrivateProfileSectionNames(section, sizeof(section), szIni);

    if (strlen(section) == 0) {
        memset(buf, 0, sizeof(buf));
        strncpy(buf, "X-Mailer", size);
        return 1;
    }

    // セクション名(ヘッダ名)ごとの処理
    p = section, pp = section;
    while ((*p != 0) && (p < section + max)) {
        int show;

        while (*pp != 0) {
            pp++;
        }
        // "Show"キーが0なら非表示
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
 * @brief 表示ヘッダのリストをiniファイルに反映する
 * @param szIni iniファイルのパス名
 * @param buf リストが格納されているバッファ
 *
 * 表示ヘッダのリストをiniファイルに反映する。
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

    // iniファイルのセクション名が表示ヘッダ名なのでセクション名を全て取得する
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

    // iniファイルの中身が空なら消し込みは行わない。
    if (strlen(section) != 0) {
        // セクション名(ヘッダ名)ごとの処理
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
            // リスト中にヘッダがなかったので非表示に
                ::WritePrivateProfileString(p, "Show",
                                            "0", szIni);
            }
            pp++;
            p = pp;
        }
    }
    return len;
}
