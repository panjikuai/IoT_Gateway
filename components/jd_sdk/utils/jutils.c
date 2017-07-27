#include <stdio.h>
#include <string.h>
#include "jutils.h"


int byte2hexstr(const uint8_t *pBytes, int srcLen, uint8_t *pDstStr, int dstLen)
{
	const char tab[] = "0123456789abcdef";
	int i = 0;

	memset(pDstStr, 0, dstLen);

	if (dstLen < srcLen * 2)
		srcLen = (dstLen - 1) / 2;

	for (i = 0; i < srcLen; i++)
	{
		*pDstStr++ = tab[*pBytes >> 4];
		*pDstStr++ = tab[*pBytes & 0x0f];
		pBytes++;
	}
	*pDstStr++ = 0;
	return srcLen * 2;
}

/* hex string to bytes*/
int hexStr2bytes(const char *hexStr, uint8_t *buf, int bufLen)
{
	int i;
	int len;

	if (NULL == hexStr)
	{
		len = 0;
	}
	else
	{
		len = (int)strlen(hexStr) / 2;

		if (bufLen < len)
			len = bufLen;
	}
	memset(buf, 0, bufLen);

	for (i = 0; i < len; i++)
	{
		char ch1, ch2;
		int val;

		ch1 = hexStr[i * 2];
		ch2 = hexStr[i * 2 + 1];
		if (ch1 >= '0' && ch1 <= '9')
			val = (ch1 - '0') * 16;
		else if (ch1 >= 'a' && ch1 <= 'f')
			val = ((ch1 - 'a') + 10) * 16;
		else if (ch1 >= 'A' && ch1 <= 'F')
			val = ((ch1 - 'A') + 10) * 16;
		else
			return -1;

		if (ch2 >= '0' && ch2 <= '9')
			val += ch2 - '0';
		else if (ch2 >= 'a' && ch2 <= 'f')
			val += (ch2 - 'a') + 10;
		else if (ch2 >= 'A' && ch2 <= 'F')
			val += (ch2 - 'A') + 10;
		else
			return -1;

		buf[i] = val & 0xff;
	}

	return 0;
}
