#ifndef _PACKETS_H
#define _PACKETS_H

#include <stdint.h>
//#include "jd_smart.h"

#define PRAGMA(x) _Pragma(#x)

#define BEGIN_PACK PRAGMA(pack(push, 1))
#define END_PACK   PRAGMA(pack(pop))

//#pragma pack(1)
BEGIN_PACK
typedef struct {
    unsigned int magic;
    unsigned int len;
    unsigned int enctype;
    unsigned char checksum;
} common_header_t;

typedef struct {
    unsigned int type;
    unsigned char cmd[2];
} cmd_header;

////////////////////////���´��������°������Э��///////////////////

typedef struct {
    unsigned int	magic;
    unsigned short	optlen;
    unsigned short	payloadlen;

    unsigned char	version;	// �汾�Ŵ�1��ʼ
    unsigned char	type;		// {  1:����,  2:��Ȩ,  3:Json����, 4:Bin͸��}
    unsigned char	total;
    unsigned char	index;

    unsigned char	enctype;	// (0:������, 1:��̬AES��Կ, 2:ECDHЭ����Կ, 3:Accesskey��Կ)
    unsigned char	reserved;
    unsigned short	crc;
}packet_t;
//#pragma pack()
END_PACK

typedef enum {		// ����Android
    PT_UNKNOWN = 0,
    PT_SCAN=1,
    PT_WRITE_ACCESSKEY=2,
    PT_JSONCONTROL=3,
    PT_SCRIPTCONTROL=4,
    PT_OTAU = 7,
    PT_OTAU_STATUS_REPORT = 8,

    PT_AUTH = 9,
    PT_BEAT = 10,
    PT_SERVERCONTROL = 11,
    PT_UPLOAD = 12
}EpacketType;

typedef enum {		// ��������
    ET_NOTHING = 0,
    ET_PSKAES = 1,
    ET_ECDH = 2,
    ET_PSKDYAES = 3
    //ET_ACCESSKEYAES = 3,
    //ET_SESSIONKEYAES = 4
}EencType;
typedef struct {
    int			version;	// 0:�ɰ�Э��, 1:�°������������
    EpacketType		type;
}packetparam_t;

#define PACKET_SIZE 1400
typedef struct bytebuffer{
    int pos;
    int len;
    int size;
    uint8_t data[PACKET_SIZE];
}bytebuffer;

typedef struct {
	unsigned int	 timestamp; // �豸ʱ��
	unsigned int	random_unm; // �豸�������������
}auth_t;

typedef struct {
	unsigned int	 timestamp; // ������ʱ��
	unsigned int	 random_unm; // �ƶ��������������
	unsigned char	session_key[];
}auth_resp_t;

typedef struct {
	unsigned int timestamp; //�豸ʱ��
	unsigned short	verion; // �̼��汾
	unsigned short	rssi;
}heartbeat_t;

typedef struct {
	unsigned int	 timestamp;
	unsigned int	 code;
}heartbeat_resp_t;


typedef struct {
	unsigned int    timestamp;
	unsigned int    biz_code;
	unsigned int	serial;
	unsigned char	cmd[]; //�ű������������
}control_t;

typedef struct {
	unsigned int timestamp;
	unsigned int biz_code;
	unsigned int	 serial;
	unsigned char resp_length[4];
	unsigned char	resp[1]; // �豸������Ӧ���
	unsigned char	streams[]; // �豸��ǰ����״̬�����գ� 
}control_resp_t;

typedef struct {
	unsigned int timestamp;
	unsigned char	data[];	//��Ҫ�ű�����������,�䳤
}dataupload_t;

typedef struct {
	unsigned int	 timestamp;
	unsigned int	 code;
}dataupload_resp_t;

#define CONTROL_REQUEST_CODE		1002
#define CONTROL_RESP_CODE		102
#define GET_SNAPSHOT_CODE		1004
#define GET_SNAPSHOT_RESP_CODE		104
#define CLOUD_RECIPE_CODE		1050
#define CLOUD_RECIPE_RESP_CODE 		150



/*
�������յ���UDP���ݰ�,���н���
����:���յ����ݰ���������ĵĳ���

����:
pParam->�������ݰ�����������
pIn->��������������ݰ�
length->���������ݰ��ĳ���
pOut->��Ž��ܺ�����ĵĿռ�
maxlen->����ռ�Ĵ�С
*/
int packetAnalyseV1(packetparam_t *pParam, const uint8_t *pIn, int length, uint8_t* pOut, int maxlen);
int packetBuildV0(uint8_t* pBuf, int enctype, int type, const uint8_t* payload, int length);
int packetBuildV1(uint8_t* pBuf, int buflen, EencType enctype, EpacketType cmd, uint8_t* key, const uint8_t* payload, int length);
int serverPacketBuild(uint8_t* pBuf, int buflen, EpacketType cmd, uint8_t* key, const uint8_t* payload, int payloadLen);
int serverAnalyse(packetparam_t *pParam, const uint8_t *pIn, int length, uint8_t* pOut, int maxlen);





#endif
