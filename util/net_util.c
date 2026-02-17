#include "net_util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char * ip2str(unsigned long ip)
{
  static char __ip2str[5][16];
  static unsigned char __ip2str_index;
  unsigned char * ipaddr;

  __ip2str_index++;
  if (__ip2str_index > 4) __ip2str_index = 0;

  ipaddr = (unsigned char *)&ip;

  sprintf(__ip2str[__ip2str_index], "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
  return __ip2str[__ip2str_index];
}


unsigned long str2ip(char * str)
{
  char * ptr = str;
  int i = 0;
  unsigned long res = 0;
  unsigned char * ipaddr = (unsigned char *)&res;

  do
  {
    ipaddr[i] = (unsigned char)atoi(ptr);
    ++i;
    if ( (ptr = strchr(ptr, '.')) != NULL ) ++ptr;
  }
  while (ptr != NULL);

  return res;
}


char * mac2str(unsigned char * mac)
{
  static char __mac2str[18];

  sprintf(__mac2str, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return __mac2str;
}

unsigned char * str2mac(char * str)
{
  char * ptr = str;
  unsigned char i = 0;
  static unsigned char _str2mac[6];

  memset(_str2mac, 0, sizeof(_str2mac));
  do
  {
    _str2mac[i] = strtoul(ptr, NULL, 16);
    if ( (ptr = strchr(ptr, ':')) != NULL ) {
      ++ptr;
      ++i;
    }
  }
  while ( (i < 6) && (ptr != NULL) );

  return _str2mac;
}

