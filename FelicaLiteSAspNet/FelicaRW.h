#pragma once

#include "felica_nfc_library.h"

#pragma comment(lib ,"felica_nfc_library.lib")
//#pragma warning(disable : 4996)



// FelicaRW
#define IDM_SIZE        (8)         // IDm �̃T�C�Y
#define BLOCK_SIZE      (16)        // �u���b�N�i�ǂݏ����̍ŏ��P�ʁj�̃T�C�Y

#define SYSTEM_CODE     (0x88b4)    // 0x88b4 = FeliCa Lite-S �̃V�X�e���R�[�h
#define SERVICE_CODE_RW (0x0009)    // 0x0009 = Random Read/Write �̃T�[�r�X�R�[�h
#define SERVICE_CODE_RO (0x000B)    // 0x000B = Random Read Only �̃T�[�r�X�R�[�h

public bool PrintNfcLastErr(char* pFuncName);
public bool PrintHex(unsigned char *pBuf, int iLen);
public bool MyPolling(unsigned short usSystemCode, unsigned char pIDm[IDM_SIZE]);
public bool MyWriteWithoutEncryption(unsigned char *pIDm, unsigned short usServiceCode, unsigned char ucBlockOffset, unsigned char *pDataW);
public bool MyReadWithoutEncryption(unsigned char *pIDm, unsigned short usServiceCode, unsigned char ucBlockOffset, unsigned char *pDataR);
public int FelicaRW::FelicaRW_main(int block, unsigned char* wData, bool read);