#ifndef _NODECACHE_H
#define _NODECACHE_H

#include "../auth/uECC.h"

typedef struct {
	uint8_t priKey[uECC_BYTES];
	uint8_t devPubKey[uECC_BYTES * 2];
	uint8_t devPubKeyC[uECC_BYTES + 1];
}eccContex_t;

#define MAX_IPLEN     20
#define MAX_MACLEN    6
#define MAX_UUIDLEN   10
#define MAX_FEEDIDLEN 33
#define MAX_PKEYBIN   21
#define MAX_PKEYSTR   21*2+1
#define MAX_DBGLEN    128
#define MAX_OPTLEN    128
#define OTAU_URL_LEN  128
typedef struct {
	uint16_t    version;		//
	uint16_t    resetype;		// reset type,1: internal reset
	uint8_t     lancon;		//
	uint8_t     trantype;		//(1:Lua, 2:Js)
	uint8_t		isConfigured;
	uint8_t		ecckeyConfigured;
	uint8_t     mac[MAX_MACLEN];	// MAC��ַ
	uint8_t     uuid[MAX_UUIDLEN];	// ProductUUID

	uint8_t     feedid[MAX_FEEDIDLEN];
	uint8_t     accesskey[33];
	uint8_t	 	localkey[33];
	uint8_t 	priKey[uECC_BYTES];		// private key
	uint8_t 	devPubKeyC[uECC_BYTES + 1]; //compress pub key
	uint8_t     pubkeyS[MAX_PKEYSTR];		// string pub key
	//char     	 devdbg[MAX_DBGLEN];
	//char     	 servropt[MAX_OPTLEN];
	//uint8_t 	 sharedkey[uECC_BYTES];
	uint8_t     sessionKey[33];
	/** Maximum size for the Wifi SSID including the NULL termination.*/
	uint8_t 	ssid[33];
	uint8_t		ssid_len;
	/** Maximum size for the WPA PSK including the NULL termination.*/
	uint8_t	 	pwd[65];
	uint8_t		pwd_len;
	uint8_t		securityType;
	uint8_t		pad_0[2];
	uint32_t	magic;//0x112233CC
	uint8_t		app_otau_url[OTAU_URL_LEN];
	uint8_t		wifi_fw_otau_url[OTAU_URL_LEN];
	uint16_t	crc16;
	uint8_t		pad_1[2];
}jddevice_t;

typedef struct{
      uint8_t pubkeyC[MAX_PKEYBIN];
      uint8_t sharedkey[uECC_BYTES];
}jdkey_t;

extern jddevice_t  *jdDev;

#define MAX_KEYLIST  50
int eccContexInit(void);
int isNodeExist(jddevice_t* dev);

int nodeUpdate(jddevice_t* dev);
void nodeClean(void);
int nodeFormatJson(uint8_t* pBuffer, int length);
int Server_Auth(uint8_t *pbuf);
int Server_HB(uint8_t *pbuf);
int Upload_Data(uint8_t *pbuf);

#endif
