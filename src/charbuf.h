/* $Id: charbuf.h,v 1.15 2005/04/26 03:44:36 woods Exp $ */

#ifndef CHARBUF_H
#define CHARBUF_H

#include <windows.h>

/// ������ێ��\����
class charbuf {
    /// �������T�C�Y
    int size;
    /// ������
    int len;
    /// ������i�[�̈�
    char *text;
public:
    /**
     * @brief �C���X�^���X�̏�����
     */
    void init(void) {
        size = 0;
        len = 0;
        text = NULL;
    };
    /**
     * @brief �C���X�^���X�̃N���A
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
     * @brief �e�L�X�g�o�b�t�@�̗v�f�A�N�Z�X
     * @param index �e�L�X�g�o�b�t�@�̃C���f�b�N�X
     */
    const char operator[](int index) { return text[index]; };
    /**
     * @brief �����񒷂�ԋp����
     */
    int length(void) { return len; };
    /**
     * @brief �������ݒ肷��
     * @param str �ݒ肷�镶����
     */
    int operator=(const char *str);
    /**
     * @brief �e�L�X�g�o�b�t�@��ԋp����
     */
    const char *textbuf(void) {return text;};
};

/// �\�������\����
class rectchar {
    /// �\���̈�
    RECT mrect;
    /// �\������
    char mstr[3];
    /// �\��������
    unsigned char mlen;
    /// ���]�\���L��
    int mreverse;
public:
    /// @brief �\���̈��ԋp����
    RECT& rect(void){return mrect;};
    /// @brief �\��������ԋp����
    const char* str(void){return mstr;};
    /// @brief �\����������ԋp����
    const int length(void){return mlen;};
    /// @brief ���]�\���L����ԋp����
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
     * @brief ������ݒ肷��
     * @param str �ݒ肷�镶��
     * @param len �����̒���
     */
    void set(const char* str, int len) {
        mlen = (((size_t)len > sizeof(mstr) - 1) ? (sizeof(mstr) - 1) : (size_t)len);
        strncpy(mstr, str, mlen);
        mstr[mlen] = 0;
    };

    /**
     * @brief ���]�\���L����ݒ肷��
     * @param rev ���]�\���L��
     */
    int setreverse(const int rev) {
        return mreverse = rev;
    };

    /**
     * @brief �\���̈悪�w�肳�ꂽ�̈�ɏd�Ȃ邩�`�F�b�N����
     * @param rect �`�F�b�N����̈�
     * @retval TRUE �d�Ȃ�
     * @retval FALSE �d�Ȃ�Ȃ�
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

/// �\�������z��\����
class rectstr {
    /// �������T�C�Y
    int msize;
    /// ������
    int mlen;
    /// ������
    rectchar *mtext;
    /// �\���̈�
    RECT mrect;
public:
    /// @brief ������
    void init(void) {
        msize = 0;
        mlen = 0;
        mtext = NULL;
        memset(&(mrect), 0, sizeof(mrect));
    };
    /// @brief �̈�̉��
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
     * @brief �������ݒ肷��
     * @param str �ݒ肷�镶����
     */
    int operator=(const char *str);

    /**
     * @brief �`��̈���v�Z����
     * @param hdc �f�o�C�X�R���e�L�X�g
     * @param pt �\���J�n�ʒu
     */
    void setrect(HDC hdc, POINT& pt);
    /// @brief �`��̈��ԋp����
    const RECT& rect(void) {return mrect;};

    /**
     * @brief �����v�f�ɃA�N�Z�X����
     * @param index �C���f�b�N�X
     */
    rectchar& operator[](int index) {
        return mtext[index];
    };
    /// @brief �����񒷂�ԋp����
    int length(void){return mlen;};
};

#endif /* CHARBUF_H */
