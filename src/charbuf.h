/* $Id: charbuf.h,v 1.15 2005/04/26 03:44:36 woods Exp $ */

#ifndef CHARBUF_H
#define CHARBUF_H

#include <windows.h>

/// 文字列保持構造体
class charbuf {
    /// メモリサイズ
    int size;
    /// 文字列長
    int len;
    /// 文字列格納領域
    char *text;
public:
    /**
     * @brief インスタンスの初期化
     */
    void init(void) {
        size = 0;
        len = 0;
        text = NULL;
    };
    /**
     * @brief インスタンスのクリア
     */
    void free(void) {
        size = 0;
        len = 0;
        delete[] text;
        text = NULL;
    }
    charbuf(void) { this->init(); };
    ~charbuf(void) { this->free(); };
    /**
     * @brief テキストバッファの要素アクセス
     * @param index テキストバッファのインデックス
     */
    const char operator[](int index) { return text[index]; };
    /**
     * @brief 文字列長を返却する
     */
    int length(void) { return len; };
    /**
     * @brief 文字列を設定する
     * @param str 設定する文字列
     */
    int operator=(const char *str);
    /**
     * @brief テキストバッファを返却する
     */
    const char *textbuf(void) {return text;};
};

/// 表示文字構造体
class rectchar {
    /// 表示領域
    RECT mrect;
    /// 表示文字
    char mstr[3];
    /// 表示文字長
    unsigned char mlen;
    /// 反転表示有無
    int mreverse;
public:
    /// @brief 表示領域を返却する
    RECT& rect(void){return mrect;};
    /// @brief 表示文字を返却する
    const char* str(void){return mstr;};
    /// @brief 表示文字長を返却する
    const int length(void){return mlen;};
    /// @brief 反転表示有無を返却する
    const int reverse(void){ return mreverse;};

    void init(void) {
        memset(this, 0, sizeof(*this));
    };
    void free(void) {
        memset(this, 0, sizeof(*this));
    };
    rectchar(void){init();};
    ~rectchar(void){free();};

    /**
     * @brief 文字を設定する
     * @param str 設定する文字
     * @param len 文字の長さ
     */
    void set(const char* str, int len) {
        mlen = (((size_t)len > sizeof(mstr) - 1) ? (sizeof(mstr) - 1) : (size_t)len);
        strncpy(mstr, str, mlen);
        mstr[mlen] = 0;
    };

    /**
     * @brief 反転表示有無を設定する
     * @param rev 反転表示有無
     */
    int setreverse(const int rev) {
        return mreverse = rev;
    };

    /**
     * @brief 表示領域が指定された領域に重なるかチェックする
     * @param rect チェックする領域
     * @retval TRUE 重なる
     * @retval FALSE 重ならない
     */
    BOOL in_rect(const RECT& rect) {
        if (((mrect.left <= rect.right) && (mrect.right >= rect.left)) &&
            ((mrect.top <= rect.bottom) && (mrect.bottom >= rect.top)))
        {
            return TRUE;
        }
        return FALSE;
    };
};

/// 表示文字配列構造体
class rectstr {
    /// メモリサイズ
    int msize;
    /// 文字列長
    int mlen;
    /// 文字列
    rectchar *mtext;
    /// 表示領域
    RECT mrect;
public:
    /// @brief 初期化
    void init(void) {
        msize = 0;
        mlen = 0;
        mtext = NULL;
        memset(&(mrect), 0, sizeof(mrect));
    };
    /// @brief 領域の解放
    void free(void) {
        msize = 0;
        mlen = 0;
        if (mtext != NULL) {
            delete[] mtext;
        }
        mtext = NULL;
        memset(&(mrect), 0, sizeof(mrect));
    };
    rectstr(void) {init();};
    ~rectstr(void) {free();};

    /**
     * @brief 文字列を設定する
     * @param str 設定する文字列
     */
    int operator=(const char *str);

    /**
     * @brief 描画領域を計算する
     * @param hdc デバイスコンテキスト
     * @param pt 表示開始位置
     */
    void setrect(HDC hdc, POINT& pt);
    /// @brief 描画領域を返却する
    const RECT& rect(void) {return mrect;};

    /**
     * @brief 文字要素にアクセスする
     * @param index インデックス
     */
    rectchar& operator[](int index) {
        return mtext[index];
    };
    /// @brief 文字列長を返却する
    int length(void){return mlen;};
};

#endif /* CHARBUF_H */
