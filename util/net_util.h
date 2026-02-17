#ifndef NET_UTIL_H
#define NET_UTIL_H

#ifndef HTONS
#define HTONS(a)			((((a)>>8)&0xff)|(((a)<<8)&0xff00))
#endif

#ifndef NTOHS
#define NTOHS(a)			HTONS(a)
#endif

#define HTONL(a)			( (((a)>>24)&0xff) | (((a)>>8)&0xff00) |\
							(uint32_t)((((uint64_t)a)<<8) & 0xff0000) |\
							(uint32_t)((((uint64_t)a)<<24) & 0xff000000) )

#define NTOHL(a)			HTONL(a)


#define IPV4ADDR(a,b,c,d)	        ( ((uint32_t)a) | ((uint32_t)b << 8) |\
									((uint32_t)c << 16) | ((uint32_t)d << 24) )

#ifdef __cplusplus
extern "C" {
#endif

//ip to string
char * ip2str(unsigned long ip);
unsigned long str2ip(char * str);
//mac to string
char * mac2str(unsigned char * mac);
unsigned char * str2mac(char * str);

#ifdef __cplusplus
}
#endif


#endif
