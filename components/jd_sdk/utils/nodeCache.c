#include <stdio.h>
#include <stdlib.h>
#include "jutils.h"
#include "nodeCache.h"
#include "../auth/uECC.h"
#include "../auth/aes.h"
#include "packets.h"
#include "jd_apps.h"
#include "debug.h"

int eccContexInit(void)
{
    eccContex_t myKey = {0};

    if (!uECC_make_key(myKey.devPubKey, myKey.priKey)) {
//            IoT_DEBUG(JOYLINK_DBG | IoT_DBG_SERIOUS,("uECC_make_key() failed\n"));
            return 1;
    }
    uECC_compress(myKey.devPubKey, myKey.devPubKeyC);

    memcpy(jdDev->devPubKeyC, myKey.devPubKeyC, uECC_BYTES + 1);
    byte2hexstr(myKey.devPubKeyC, uECC_BYTES + 1, (uint8_t*)jdDev->pubkeyS, uECC_BYTES * 2 + 3);
    memcpy((uint8_t*)jdDev->priKey, (uint8_t*)myKey.priKey, uECC_BYTES);

    return 0;
}


int isNodeExist(jddevice_t* dev)
{
    return 0;
}


int Server_Auth(uint8_t *pbuf)
{
    auth_t auth = { 0 };
    auth.random_unm = 0xaa55bb55;
    auth.timestamp = 0x54E67900;
    int len = serverPacketBuild(pbuf, HB_BUFFER_SIZE, PT_AUTH, jdDev->accesskey, (const uint8_t*)&auth, sizeof(auth));
    return len;
}
extern uint32_t g_server_time;
int Server_HB(uint8_t *pbuf)
{
    heartbeat_t heartbeat = { 0 };
    heartbeat.rssi = 90;
    heartbeat.verion = APP_VERSION_SEQUENCE;
    heartbeat.timestamp = g_server_time + 10;
    //heartbeat.timestamp = 50;

    int len = serverPacketBuild(pbuf, HB_BUFFER_SIZE, PT_BEAT, jdDev->sessionKey, (const uint8_t*)&heartbeat, sizeof(heartbeat_t));
    return len;
}
