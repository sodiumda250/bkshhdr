/*
 * $Id: charbuf.cpp,v 1.29 2005/04/26 05:29:09 woods Exp $
 */

static char id[] = "$Id: charbuf.cpp,v 1.29 2005/04/26 05:29:09 woods Exp $";

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "charbuf.h"

/// �������T�C�Y�ŏ��l
#define SIZE_MIN 128

static int strlenDBCS(const char *str);

/**
 * @brief ������ێ��\���̂ɕ�������Z�b�g����
 * @param cb ������ێ��\����
 * @param str �ێ����镶����
 * @return �Z�b�g����������
 *
 * ������ێ��\���̂ɕ�������Z�b�g����B
 */
int charbuf::operator=(const char *str)
{
    int len = strlen(str);

    if ((len + 1) > size) {
        int tmpsize = ((size <= 0) ? SIZE_MIN : size);

        while (tmpsize < (len + 1)) {
            tmpsize *= 2;
        }
        if (text == NULL) {
            size = tmpsize;
            text = new char[size];
            if (text == NULL) {
                size = 0;
                len = 0;
                return 0;
            }
        } else {
            char* tmp = new char[tmpsize];
            if (tmp == NULL) {
                return 0;
            } else {
                size = tmpsize;
                delete[] text;
                text = tmp;
            }
        }
    }
    strncpy(text, str, len);
    text[len] = 0;
    len = len;

    return len;
}

/**
 * @brief �\�������z��\���̂ɕ������ݒ肷��
 * @param rs �\�������z��\����
 * @param str �ݒ肷�镶����
 * @return �ݒ肵��������
 *
 * �\�������z��\���̂ɕ������ݒ肷��B
 */
int rectstr::operator=(const char *str)
{
    int i, j;
    int slen;
    int len = strlenDBCS(str);

    if (len + 1 > msize) {
        int tmpsize = ((msize <= 0) ? SIZE_MIN : msize);

        while (tmpsize < (len + 1)) {
            tmpsize *= 2;
        }
        if (mtext == NULL) {
            mtext = new rectchar[tmpsize];
            if (mtext == NULL) {
                msize = 0;
                return 0;
            }
        } else {
            rectchar *pt = new rectchar[tmpsize];
            if (pt == NULL) {
                return 0;
            } else {
                delete[] mtext;
                mtext = pt;
            }
        }
        msize = tmpsize;
    }

    i = j = 0;
    slen = strlen(str);
    while ((i < slen) && (j < len)) {
        if (::IsDBCSLeadByte(str[i])) {
            mtext[j++].set(str + i, 2);
            i += 2;
        } else if ((str[i] == '\r') && (str[i + 1] == '\n')) {
            mtext[j++].set("\n", 1);
            i += 2;
        } else {
            mtext[j++].set(str + i, 1);
            i += 1;
        }
    }
    mlen = len;
    return len;
}

/**
 * @brief �\�������z��\���̂̕\���̈���v�Z���Đݒ肷��
 * @param rs �\�������z��\����
 * @param hdc �\���p�f�o�C�X�R���e�L�X�g
 * @param pt �\���ʒu
 *
 * �\�������z��\���̂̕\���̈���v�Z���Đݒ肷��B
 */
void rectstr::setrect(HDC hdc, POINT& pt)
{
    int i;
    SIZE size;
    POINT next;

    next = pt;
    mrect.top = pt.y;
    mrect.bottom = pt.y;
    mrect.left = pt.x;
    mrect.right = pt.x;

    for (i = 0; i < mlen; i++) {
        if ((mtext[i].str()[0] == '\n') || (mtext[i].str()[0] == '\r')){
            mtext[i].rect().top = next.y;
            mtext[i].rect().left = next.x;
            mtext[i].rect().bottom = next.y;
            mtext[i].rect().right = next.x;
            next.y += size.cy;
            next.x = pt.x;
        } else {
            ::GetTextExtentPoint32(hdc, mtext[i].str(), mtext[i].length(), &size);
            mtext[i].rect().top = next.y;
            mtext[i].rect().left = next.x;
            mtext[i].rect().bottom = next.y + size.cy;
            mtext[i].rect().right = next.x + size.cx;
            next.x += size.cx;
        }
        if (mrect.bottom < mtext[i].rect().bottom) {
            mrect.bottom = mtext[i].rect().bottom;
        }
        if (mrect.right < mtext[i].rect().right) {
            mrect.right = mtext[i].rect().right;
        }
    }
}

/**
 * @brief �_�u���o�C�g�������܂ޕ�����̕�������ԋp����B
 * @param str ���������v�Z���镶����
 * @return ������
 *
 * �_�u���o�C�g�������܂ޕ�����̕�������ԋp����B
 */
static int strlenDBCS(const char *str)
{
    int len = 0;

    while (*str != 0) {
        if (::IsDBCSLeadByte(*str)) {
            str++;
            if (*str == 0) {
                break;
            }
        }
        len++;
        str++;
    }
    return len;
}
