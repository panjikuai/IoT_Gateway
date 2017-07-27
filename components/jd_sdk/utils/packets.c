#include <stdio.h>
#include <stdint.h>
#include "packets.h"
#include "../auth/uECC.h"
#include "../auth/aes.h"
#include "../auth/crc.h"
#include "nodeCache.h"
#include "iot_debug.h"

#define NUM_TOKENS  100
#define UDP_MTU		1400

int packetBuildV0(uint8_t* pBuf, int enctype, int type, const uint8_t* payload, int length)
{
#ifdef QCOM_4004B
#else
    unsigned int i;
    common_header_t* pCommon = (common_header_t*)pBuf;
    pCommon->magic = 0x55AA;
    pCommon->enctype = 0;	// Encryption Type

    pBuf += sizeof(common_header_t);
    cmd_header* pCmd = (cmd_header*)(pBuf);
    pCmd->type = type;
    memcpy(pCmd->cmd, "OK", 2);

    char* pData = (char *)pBuf + sizeof(cmd_header);

    char* pStrJson = (char*)payload;
    memcpy(pData, pStrJson, length);

    pCommon->len = length + sizeof(cmd_header);

    unsigned char sum = 0;
    for (i = 0; i < pCommon->len; i++)
    {
            sum += *(pBuf + i);
    }
    pCommon->checksum = sum;
    return pCommon->len + sizeof(common_header_t);
#endif
}


int packetBuildV1(uint8_t* pBuf, int buflen, EencType enctype, EpacketType cmd, uint8_t* key, const uint8_t* payload, int length)
{
    packet_t head = {
            .magic = 0x123455BB,
            .optlen = 0,
            .payloadlen = 0,
            .version = 1,
            .type = (char)cmd,
            .total = 0,
            .index = 0,
            .enctype = (char)enctype,
            .reserved = 0,
            .crc = 0
    };
    char* psJson = (char*)payload;
    uint8_t* pOut = pBuf + sizeof(packet_t);

    switch (enctype)
    {
    case ET_NOTHING:
            memcpy(pOut, psJson, length);
            pOut += length;
            head.optlen = 0;
            head.payloadlen = (uint16_t)length;
            break;
    case ET_PSKAES:
            memcpy(pOut, psJson, length);
            pOut += length;
            head.optlen = 0;
            head.payloadlen = length;
            break;
    case ET_PSKDYAES:
            head.optlen = 0;
            length = device_aes_encrypt(key, 16, key+16, (uint8_t*)psJson, length, pOut, length + 16);
            pOut += length;
            head.payloadlen = length;
            break;
    case ET_ECDH:
      {
            memcpy(pOut, jdDev->devPubKeyC, uECC_BYTES + 1);
            pOut += (uECC_BYTES + 1);
            head.optlen = (uECC_BYTES + 1);
            uint8_t peerPubKey[uECC_BYTES * 2];
            uECC_decompress(key, peerPubKey);
            uint8_t secret[uECC_BYTES];
            uECC_shared_secret(peerPubKey, jdDev->priKey, secret);
            length = device_aes_encrypt(secret, 16, key + 4, (uint8_t*)psJson, length, pOut, length + 16);
            head.payloadlen = length;
            pOut += length;
            break;
      }
    default:
            break;
    }

    length = pOut - pBuf;
    head.crc = CRC16(pBuf + sizeof(packet_t), length - sizeof(packet_t));
    memcpy(pBuf, &head, sizeof(head));
    return length;
}

int serverPacketBuild(uint8_t* pBuf, int buflen, EpacketType cmd, uint8_t* key, const uint8_t* payload, int payloadLen)
{
    packet_t head = {
            .magic = 0x123455CC,
            .optlen = 0,
            .payloadlen = 0,
            .version = 1,
            .type = (char)cmd,
            .total = 0,
            .index = 0,
            .enctype = (char)1,
            .reserved = 0,
            .crc = 0
    };

    int len = 0;
    uint8_t* pOut = pBuf + sizeof(packet_t);
    switch (cmd)
    {
    case PT_AUTH:
            head.enctype = ET_PSKDYAES;
            len = strlen((char *)jdDev->feedid);
            head.optlen = len + 4;
            memcpy(pOut, jdDev->feedid, len);
            pOut += len;
            memcpy(pOut, payload+4, 4);
            pOut += 4;
            len = device_aes_encrypt(key, 16, key + 16, (uint8_t*)payload, payloadLen, pOut, 1024);
            head.payloadlen = len;
            pOut += len;

            break;
    default:
            head.enctype = ET_PSKDYAES;
            head.optlen = 0;
            len = device_aes_encrypt(key, 16, key + 16, (uint8_t*)payload, payloadLen, pOut, 1024);
            head.payloadlen = len;
            pOut += len;
            break;
    }

    payloadLen = pOut - pBuf;
    head.crc = CRC16(pBuf + sizeof(packet_t), payloadLen - sizeof(packet_t));
    memcpy(pBuf, &head, sizeof(head));
    return payloadLen;
}

int serverAnalyse(packetparam_t *pParam, const uint8_t *pIn, int length, uint8_t* pOut, int maxlen)
{
    int ret = 0;
    int retLen = 0;
    uint8_t* p = NULL;
    packet_t* pPack = (packet_t*)pIn;
    if (0x123455CC != pPack->magic)
            return 0;
    switch (pPack->type)
    {
    case PT_AUTH:
            pParam->version = 1;
            pParam->type = (EpacketType)pPack->type;
            p = (uint8_t*)pIn + sizeof(packet_t)+pPack->optlen;
            retLen = device_aes_decrypt(jdDev->accesskey, 16, &jdDev->accesskey[0] + 16, p, pPack->payloadlen, p, 1024);
            if (retLen>0)
            {
                    ret = retLen;
                    memcpy(pOut, p, retLen);
                    pPack->payloadlen = retLen;
            }
            break;
    default:
            pParam->version = 1;
            pParam->type = (EpacketType)(pPack->type);
            p = (uint8_t*)pIn + sizeof(packet_t)+pPack->optlen;
            retLen = device_aes_decrypt(jdDev->sessionKey, 16, &jdDev->sessionKey[0] + 16, p, pPack->payloadlen, p, 1024);
            if (retLen > 0)
            {
                    ret = retLen;
                    memcpy(pOut, p, retLen);
                    pPack->payloadlen = retLen;
            }
            break;
    }
    return ret;
}

int packetAnalyseV1(packetparam_t *pParam, const uint8_t *pIn, int length, uint8_t* pOut, int maxlen)
{
    int retLen = 0;
    char* pJsonStr = NULL;
    common_header_t* pCommon = (common_header_t*)pIn;
    if (0x123455bb == pCommon->magic){
        packet_t* pPack = (packet_t*)pIn;
        if ((length != (pPack->optlen + pPack->payloadlen + sizeof(packet_t))) || (pPack->payloadlen > UDP_MTU)){
                return 0;
        }
        if(pParam != NULL) {
                pParam->version = pPack->version;
                pParam->type = (EpacketType)(pPack->type);
        }
        if((pPack->enctype == ET_ECDH) && (pParam != NULL)) {
                return pPack->payloadlen;
        }
        IoT_DEBUG(ZIGBEE_DBG | IoT_DBG_INFO,("pPack->enctype: %d\r\n", pPack->enctype));
        switch (pPack->enctype){
        case ET_NOTHING:{
            pJsonStr = (char*)pIn + sizeof(packet_t)+pPack->optlen;
            retLen = pPack->payloadlen;
            memcpy(pOut, pJsonStr, retLen);
            return retLen;
            break;
        }
        case ET_PSKDYAES:{
            if((pPack->type == PT_JSONCONTROL) || (pPack->type == PT_SCRIPTCONTROL)) {
                    pJsonStr = (char*)pIn + sizeof(packet_t)+pPack->optlen;
                    retLen = device_aes_decrypt(jdDev->localkey, 16, &jdDev->localkey[0]+ 16, pIn + sizeof(packet_t)+pPack->optlen, pPack->payloadlen, (uint8_t*)pJsonStr, 1024);
                    memcpy(pOut, pJsonStr, retLen);
                    return retLen;
            }
            break;
        }
        case ET_ECDH:{
           uint8_t devPubKey[uECC_BYTES * 2];
           uECC_decompress(pIn + sizeof(packet_t), devPubKey);
           uint8_t secret[uECC_BYTES];
           uECC_shared_secret(devPubKey, jdDev->priKey, secret);
           pJsonStr = (char*)pIn + sizeof(packet_t)+pPack->optlen;
           retLen = device_aes_decrypt(secret, 16, secret+4, pIn + sizeof(packet_t)+pPack->optlen, pPack->payloadlen, (uint8_t*)pJsonStr, maxlen);
           memcpy(pOut, pJsonStr, retLen);
           return retLen;
           break;
        }
        default:
                break;
        }
        return 0;
    }
    else if (0x55AA == pCommon->magic){
        char* pCommonHeader = (char*)pIn + sizeof(common_header_t);
        int type = ((cmd_header*)(pCommonHeader))->type;

        pParam->version = 0;
        switch (type){
        case 1:
        case 2:
                pParam->type = PT_SCAN;
                break;
        case 3:
        case 4:
                pParam->type = PT_WRITE_ACCESSKEY;
                break;
        default:
                pParam->type = PT_UNKNOWN;
                return 0;
                break;
        }

        unsigned char sum = 0;
        unsigned int i = 0;
        for (i = 0; i < pCommon->len; i++){
                sum += *(pCommonHeader + i);
        }
        if (pCommon->checksum != sum){
                pParam->type = PT_UNKNOWN;
                return 0;
        }
        char* pJsonStr = pCommonHeader + sizeof(cmd_header);
        retLen = strlen(pJsonStr);
        memcpy(pOut, pJsonStr, retLen);
        return retLen;
    }
    return 0;
}


