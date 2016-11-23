// FelicaRW.cpp : �����t�@�C��
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FelicaRW.h"

#pragma warning(disable : 4996)

// FelicaRW
unsigned char account[16];
unsigned char domain[16];
unsigned char user_id[16];
unsigned char password[16];
unsigned char str[100];

unsigned char name[16];

unsigned char* ustrcat(unsigned char *dest, const unsigned char *src) {
	unsigned int length_1 = 0;
	unsigned int length_2 = 0;
	// find the end of the first unsigned char string
	for (; dest[length_1] != '\0'; length_1++);
	//input unsigned chars into the string from that position to the end
	for (; src[length_2] != '\0'; length_2++)
		dest[length_1 + length_2] = src[length_2];
	// place a NULL terminating character at the end
	dest[length_1 + length_2] = '\0';
	return dest;
}

unsigned char* Felica::GetName()
{
	return name;
}

unsigned char* Felica::GetData()
{
	return str;
}

void Felica::SetData()
{
	strcpy((char*)str, (char*)account);

	strcat((char*)str, ",");

	strcat((char*)str, (char*)domain);

	strcat((char*)str, ",");

	strcat((char*)str, (char*)user_id);

	strcat((char*)str, ",");

	strcat((char*)str, (char*)password);

	return str;
}

bool Felica::PrintNfcLastErr(char* pFuncName)
{
	unsigned long pErrInfo[2];
	bool bSuccess;

	bSuccess = felicalib_nfc_get_last_error(pErrInfo);
	if (bSuccess)
	{
		printf("%s �ŃG���[���� [%x, %x].\n",
			pFuncName,
			pErrInfo[0],    // NFC�A�N�Z�X���C�u�����G���[�R�[�h
			pErrInfo[1]);   // �h���C�o�G���[�R�[�h
	}
	else
	{
		printf("%s �ŃG���[����. �G���[���擾���s.\n",
			pFuncName);
	}

	return true;
}

bool Felica::MyPrintHex(unsigned char *pBuf, int iLen) {

	char buf[16]; 
	for (int i = 0; i < iLen; i++)
	{
		sprintf(buf, "%02x ", pBuf[i]);
		//strcat(po, buf);
	}
	sprintf(buf, "%s", "\r\n");
	//strcat(po, buf);

	return true;
}

bool Felica::MyPolling(unsigned short usSystemCode,
	unsigned char pIDm[IDM_SIZE]) {
	bool bSuccess;

	// Polling �R�}���h�̑g�ݗ���
	unsigned char pCom[] = {
		0x00,           // �f�[�^��
		0x00,           // �R�}���h�R�[�h
		0x00, 0x00,     // �V�X�e���R�[�h
		0x00,           // ���N�G�X�g�R�[�h
		0x00            // �^�C���X���b�g
	};
	unsigned char pRes[512];

	unsigned short usComLen = sizeof(pCom);
	unsigned short usResLen = sizeof(pRes);

	pCom[0] = sizeof(pCom);

	pCom[2] = usSystemCode >> 8;
	pCom[3] = usSystemCode & 0xFF;

	// �R�}���h�̔��s
	bSuccess = felicalib_nfc_thru(pCom, usComLen, pRes, &usResLen);
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_thru(polling)");
		return false;
	}

	// ���X�|���X�̃`�F�b�N
	if (usResLen != 1 + 1 + 8 + 8)
	{
		printf("polling �̃��X�|���X�� %d ���s�� #1\n", usResLen);
		return false;
	}
	if (pRes[0] != 1 + 1 + 8 + 8)
	{
		printf("polling �̃��X�|���X�� %d ���s�� #2\n", pRes[0]);
		return false;
	}
	if (pRes[1] != 0x01)
	{
		printf("polling �̃��X�|���X�R�[�h %d ���s��\n", pRes[1]);
		return false;
	}

	// IDm �̃R�s�[
	memcpy_s(pIDm, IDM_SIZE, &pRes[2], IDM_SIZE);

	return true;
}

/*
bSuccess = MyWriteWithoutEncryption(
pIDm,                       // Polling �R�}���h�Ŏ擾���� IDm IN
SERVICE_CODE_RW,            // �T�[�r�X�R�[�h IN
pBlockNum[iBlockCount],     // �u���b�N�ԍ� IN
pDataR);                    // �ǂݏo�����f�[�^�̊i�[�ꏊ OUT
*/

bool Felica::MyWriteWithoutEncryption(unsigned char *pIDm,
	unsigned short usServiceCode,
	unsigned char ucBlockOffset,
	unsigned char *pDataW) {
	bool bSuccess;

	// Write Without Encryption �R�}���h�̑g�ݗ���
	unsigned char pCom[] = {
		0x00,                                           // �f�[�^��
		0x08,                                           // �R�}���h�R�[�h
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // IDm
		0x01,                                           // �T�[�r�X��
		0x00, 0x00,                                     // �T�[�r�X�R�[�h
		0x01,                                           // �u���b�N��
		0x00, 0x00, 0x00,                               // �u���b�N
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // �C�ӂ̃f�[�^
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // �C�ӂ̃f�[�^
	};

	unsigned char pRes[512];

	unsigned short usComLen = sizeof(pCom);
	unsigned short usResLen = sizeof(pRes);

	pCom[0] = sizeof(pCom);

	memcpy_s(&pCom[2], sizeof(pCom) - 2, pIDm, IDM_SIZE);

	pCom[11] = usServiceCode & 0xFF;
	pCom[12] = usServiceCode >> 8;

	pCom[15] = ucBlockOffset;

	memcpy_s(&pCom[17], sizeof(pCom) - 17, pDataW, BLOCK_SIZE);

	// �R�}���h�̔��s
	bSuccess = felicalib_nfc_thru(pCom, usComLen, pRes, &usResLen);
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_thru(write without encryption)");
		return false;
	}

	// ���X�|���X�̃`�F�b�N
	if (usResLen != 1 + 1 + 8 + 1 + 1)
	{
		printf("write w/o enc �̃��X�|���X�� %d ���s�� #1\n", usResLen);
		return false;
	}
	if (pRes[0] != 1 + 1 + 8 + 1 + 1)
	{
		printf("write w/o enc �̃��X�|���X�� %d ���s�� #2\n", pRes[0]);
		return false;
	}
	if (pRes[1] != 0x09)
	{
		printf("write w/o enc �̃��X�|���X�R�[�h %d ���s��\n", pRes[1]);
		return false;
	}
	if (pRes[10] != 0x00)
	{
		printf("write w/o enc �̃X�e�[�^�X�t���O�� [%02x, %02x] �̃G���[\n", pRes[10], pRes[11]);
		return false;
	}

	return true;
}

bool Felica::MyReadWithoutEncryption(unsigned char *pIDm,
	unsigned short usServiceCode,
	unsigned char ucBlockOffset,
	unsigned char *pDataR) {
	bool bSuccess;

	// Read Without Encryption �R�}���h�̑g�ݗ���
	unsigned char pCom[] = {
		0x00,                                           // �f�[�^��
		0x06,                                           // �R�}���h�R�[�h
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // IDm
		0x01,                                           // �T�[�r�X��
		0x00, 0x00,                                     // �T�[�r�X�R�[�h
		0x01,                                           // �u���b�N��
		0x00, 0x00, 0x00                                // �u���b�N
	};

	unsigned char pRes[512];

	unsigned short usComLen = sizeof(pCom);
	unsigned short usResLen = sizeof(pRes);

	pCom[0] = sizeof(pCom);

	memcpy_s(&pCom[2], sizeof(pCom) - 2, pIDm, IDM_SIZE);

	pCom[11] = usServiceCode & 0xFF;
	pCom[12] = usServiceCode >> 8;

	pCom[15] = ucBlockOffset;

	// �R�}���h�̔��s
	bSuccess = felicalib_nfc_thru(pCom, usComLen, pRes, &usResLen);
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_thru(read without encryption)");
		return false;
	}

	// ���X�|���X�̃`�F�b�N
	if (usResLen < 1 + 1 + 8 + 1 + 1)
	{
		printf("read w/o enc �̃��X�|���X�� %d ���s�� #1\n", usResLen);
		return false;
	}
	if (pRes[0] < 1 + 1 + 8 + 1 + 1)
	{
		printf("read w/o enc �̃��X�|���X�� %d ���s�� #2\n", pRes[0]);
		return false;
	}
	if (pRes[1] != 0x07)
	{
		printf("read w/o enc �̃��X�|���X�R�[�h %d ���s��\n", pRes[1]);
		return false;
	}
	if (pRes[10] != 0x00)
	{
		printf("read w/o enc �̃X�e�[�^�X�t���O�� [%02x, %02x] �̃G���[\n", pRes[10], pRes[11]);
		return false;
	}

	// �ǂݏo�����f�[�^�̃R�s�[
	memcpy_s(pDataR, BLOCK_SIZE, &pRes[13], BLOCK_SIZE);

	return true;
}

unsigned char* Felica::FelicaRW_main(int block, unsigned char* wData, bool read)
{
	//strcpy(po, "IDm = ");

	bool bSuccess;
	unsigned char pIDm[IDM_SIZE];

	// ���C�u�����̏�����
	bSuccess = felicalib_nfc_initialize();
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_initialize");
		goto L_End;
	}

	// �|�[�g�̃I�[�v��
	char szPort[16] = "";
	bSuccess = felicalib_nfc_open(szPort);
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_open");
		goto L_Uninitialize;
	}
	printf("port=%s\n", szPort);

	// �f�o�C�X�̕⑫�ƃA�N�Z�X���̎擾
	DEVICE_INFO *pInfo;
	bSuccess = felicalib_nfc_poll_and_start_dev_access(
		DEVICE_TYPE_NFC_18092_212K      // FeliCa 212kbps
		| DEVICE_TYPE_NFC_18092_424K,   // FeliCa 424kbps
		3000,                           // �^�C���A�E�g ms
		&pInfo);
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_poll_and_start_dev_access");
		goto L_Close;
	}

	// IDm �̕\��
	DEVICE_DATA_NFC_18092_212_424K *pf;
	pf = (DEVICE_DATA_NFC_18092_212_424K *)pInfo->dev_info;
	printf("IDm=");
	MyPrintHex(pf->NFCID2, IDM_SIZE);

	// Polling �R�}���h���s
	bSuccess = MyPolling(SYSTEM_CODE, pIDm);
	if (!bSuccess)
	{
		goto L_StopDevAccess;
	}

	int pBlockNum[] =
	{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
		0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88,
	};
	char *pBlockName[] =
	{
		"S_PAD0", "S_PAD1", "S_PAD2", "S_PAD3", "S_PAD4", "S_PAD5", "S_PAD6", "S_PAD7",
		"S_PAD8", "S_PAD9", "S_PAD10", "S_PAD11", "S_PAD12", "S_PAD13", "REG",
		"ID", "D_ID", "SER_C", "SYS_C", "CKV", "CK",
		"MC",
	};

	if (true == read)
	{
		bSuccess = MyReadWithoutEncryption(
			pIDm,                       // Polling �R�}���h�Ŏ擾���� IDm
			SERVICE_CODE_RO,            // �T�[�r�X�R�[�h
			pBlockNum[0],     // �u���b�N�ԍ�
			account);                    // �ǂݏo�����f�[�^�̊i�[�ꏊ

		bSuccess = MyReadWithoutEncryption(
			pIDm,                       // Polling �R�}���h�Ŏ擾���� IDm
			SERVICE_CODE_RO,            // �T�[�r�X�R�[�h
			pBlockNum[1],     // �u���b�N�ԍ�
			domain);                    // �ǂݏo�����f�[�^�̊i�[�ꏊ

		bSuccess = MyReadWithoutEncryption(
			pIDm,                       // Polling �R�}���h�Ŏ擾���� IDm
			SERVICE_CODE_RO,            // �T�[�r�X�R�[�h
			pBlockNum[2],     // �u���b�N�ԍ�
			user_id);                    // �ǂݏo�����f�[�^�̊i�[�ꏊ

		bSuccess = MyReadWithoutEncryption(
			pIDm,                       // Polling �R�}���h�Ŏ擾���� IDm
			SERVICE_CODE_RO,            // �T�[�r�X�R�[�h
			pBlockNum[3],     // �u���b�N�ԍ�
			password);                    // �ǂݏo�����f�[�^�̊i�[�ꏊ

		bSuccess = MyReadWithoutEncryption(
			pIDm,                       // Polling �R�}���h�Ŏ擾���� IDm
			SERVICE_CODE_RO,            // �T�[�r�X�R�[�h
			pBlockNum[4],     // �u���b�N�ԍ�
			name);                    // �ǂݏo�����f�[�^�̊i�[�ꏊ

		SetData();
	}
	else
	{
		unsigned char pDataW[16] =
		{
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};

		MyWriteWithoutEncryption(
			pIDm,
			SERVICE_CODE_RW,		    	//usServiceCode,
			block,							//ucBlockOffset,
			wData);
	}

L_StopDevAccess:
	// �f�o�C�X�g�p���̉��
	bSuccess = felicalib_nfc_stop_dev_access(
		RE_NOTIFICATION_SAME_DEVICE);
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_stop_dev_access");
	}

	// �f�o�C�X�ߑ������̒�~
	bSuccess = felicalib_nfc_stop_poll_mode();
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_stop_poll_mode");
	}

L_Close:
	// �|�[�g�̃N���[�Y
	bSuccess = felicalib_nfc_close();
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_close");
	}

L_Uninitialize:
	// ���C�u�����̏I������
	bSuccess = felicalib_nfc_uninitialize();
	if (!bSuccess)
	{
		PrintNfcLastErr("felicalib_nfc_uninitialize");
	}

L_End:

	return 0;
}