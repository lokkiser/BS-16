#include "http_util.h"
#include <string.h>
#include "str_util.h"
#include <stdio.h>

char* http_server = NULL;

//const char http_get[] = "GET /";
//const char http_post[] = "POST /";
//const char http_put[] = "PUT /";
const char* http_request_str[] = {"UNKNOWN", "GET", "POST", "HEAD", "PUT", "PATCH", "DELETE", "TRACE", "CONNECT", "OPTIONS"};

const char http_code_200[] = "200 OK";
const char http_code_400[] = "400 Bad Request";
const char http_code_401[] = "401 Unauthorized";
const char http_code_404[] = "404 Not Found";
const char http_code_500[] = "500 Internal Server Error";
const char http_delimetr_n[] = "\n";
const char http_delimetr_rn[] = "\r\n";



//поиск значения хидера
char* find_header_value(char* headers, unsigned short headers_len, char* header_name, const char* r_n, unsigned short* header_len)
{
  char* ptr, *ptr_e;
  size_t len;


  ptr = strgate(headers, headers_len, header_name, r_n, &len);
  if ( (ptr = strgate(headers, headers_len, header_name, r_n, &len)) == NULL ) return NULL;
  if ( (ptr_e = strstr_s(ptr, len, ":")) == NULL ) return NULL;
  len -= (ptr_e - ptr + 1);
  ptr = ptr_e + 1;
  while (len > 0)
  {
    if (*ptr != ' ') break;
    len--;
    ptr++;
  }
  *header_len = len;
  return ptr;
}

//поиск authorization
char * find_authorization(char* headers, unsigned short headers_len, const char* r_n, unsigned short* authorization_len)
{
	unsigned short len;
	char * ptr, *ptrb;

	if ( (ptr = find_header_value(headers, headers_len, "Authorization", r_n, &len)) != NULL)
	{
 	    if ( (ptrb = strstr_s(ptr, len, "Basic")) != NULL) {
 	    	ptrb += (strlen("Basic") + 1);
            len = len - (ptrb - ptr);
            ptr = ptrb;
 	    }
	    *authorization_len = len;
	    return ptr;
	}
	return NULL;
}


//поиск значения по ключю (пример boundary=----WebKitFormBoundaryPT8oA3mn8yCHvuVu)
char * http_find_value(char* buf, unsigned short buf_len, char * key, size_t* value_len)
{
  char * ptr, * ptr_e;
  unsigned short len;

  if ( (ptr = strstr_s(buf, buf_len, key)) == NULL) return NULL;
  ptr += strlen(key);
  len = buf_len - (ptr - buf);

  if ( (ptr = strstr_s(ptr, len, "=")) == NULL) return NULL;
  ++ptr;
  len = buf_len - (ptr - buf);

  if ( (ptr_e = strstr_s(ptr, len, ";")) != NULL)
  {
     len = ptr_e - ptr;
  }

  *value_len = len;
  return ptr;
}


char* http_find_header_value(char* headers, unsigned short headers_len, const char* header_name, const char* http_delimetr, size_t* header_len)
{
  char* ptr, *ptr_e;
  size_t len;


  ptr = strgate(headers, headers_len, header_name, http_delimetr, &len);
  if (!ptr) return NULL;
  ptr_e = strright(ptr, len, ":");
  if (!ptr_e) return NULL;
  len -= (ptr_e - ptr);
  ptr = ptr_e;
  while (len > 0)
  {
    if (*ptr != ' ') break;
    len--;
    ptr++;
  }
  *header_len = len;
  return ptr;
}

bool http_parse_request(char* buf, size_t buf_size, http_request_data_t* http_request_data)
{
	size_t i, len;
	char * ptr, * ptr2;



	memset(http_request_data, 0, sizeof(http_request_data_t));
	ptr = NULL;
	for (i = 1; i < 10; ++i) {
		if (strcmp_s(buf, buf_size, http_request_str[i])) {
			if (buf[strlen(http_request_str[i])] != ' ') return false;
			http_request_data->request = i;
			ptr = buf + strlen(http_request_str[i]) + 1;
			len = buf_size - (ptr - buf);
			break;
		}
	}
	if (!ptr) return false;

	ptr2 = strstr_s(ptr, len, " ");
	if (!ptr2) return false;
	strncpy_s(http_request_data->path, sizeof(http_request_data->path), ptr, ptr2 - ptr);

	http_request_data->delimetr = http_get_delimetr(ptr, len);
	if (!http_request_data->delimetr) return false;


	http_request_data->headers = strright(ptr, len, http_request_data->delimetr);
	if (!http_request_data->headers) return true;

	len = buf_size - (http_request_data->headers - buf);


	http_request_data->body = http_find_body(http_request_data->headers, len, http_request_data->delimetr);

	if (http_request_data->body) 
    {
		http_request_data->headers_len = http_request_data->body - http_request_data->headers - strlen(http_request_data->delimetr);
		http_request_data->body_len = buf_size - (http_request_data->body - buf);

	}
	else 
    {
		http_request_data->headers_len = buf_size - (http_request_data->headers - buf);
	}

	if ((http_request_data->request == HTTP_POST) || (http_request_data->request == HTTP_PUT) ) 
    {
		ptr = http_find_header_value(http_request_data->headers, http_request_data->headers_len, "Content-Length", http_request_data->delimetr, &len);
		if (ptr) http_request_data->content_length = str2int(ptr);

		ptr = http_find_header_value(http_request_data->headers, http_request_data->headers_len, "Content-Type", http_request_data->delimetr, &len);
		if (ptr) 
        {
			ptr2 = http_find_value(ptr, len, "boundary", &len);
			if (ptr2) strncpy_s(http_request_data->boundary, sizeof(http_request_data->boundary), ptr2, len);
		}
	}
	return true;
}

bool http_parse_replay(char* buf, size_t buf_size, http_replay_data_t* http_replay_data)
{
	size_t len;
	char * ptr;



	memset(http_replay_data, 0, sizeof(http_replay_data_t));
	if (!strcmp_s(buf, buf_size, "HTTP/")) return false;
	ptr = strright(buf, buf_size, " ");
	if (!ptr) return false;
	http_replay_data->code = str2int(ptr);
	if (!http_replay_data->code) return false;

	http_replay_data->delimetr = http_get_delimetr(buf, buf_size);
	if (!http_replay_data->delimetr) return false;


	http_replay_data->headers = strright(buf, buf_size, http_replay_data->delimetr);
	if (!http_replay_data->headers) return true;

	ptr = http_replay_data->headers;
	len = buf_size - (http_replay_data->headers - buf);


	http_replay_data->body = http_find_body(ptr, len, http_replay_data->delimetr);

	if (http_replay_data->body) 
    {
		http_replay_data->headers_len = http_replay_data->body - http_replay_data->headers - strlen(http_replay_data->delimetr);
		http_replay_data->body_len = buf_size - (http_replay_data->body - buf);
	}
	else 
    {
		http_replay_data->headers_len = buf_size - (http_replay_data->headers - buf);
	}

	ptr = http_find_header_value(http_replay_data->headers, http_replay_data->headers_len, "Content-Length", http_replay_data->delimetr, &len);
	if (ptr) http_replay_data->content_length = str2int(ptr);

	return true;
}


const char* http_get_delimetr(char* buf, size_t buf_size)
{
	char* ptr;

	ptr = strchr_s(buf, buf_size, http_delimetr_n[0]);
	if ( (ptr == NULL) || (ptr == buf) ) return NULL;
	ptr--;
	if (ptr == buf) return NULL;
	if (*ptr == http_delimetr_rn[0]) return http_delimetr_rn;
	else return http_delimetr_n;
}

char * http_find_body(char* buf, size_t buf_size, const char* http_delimetr)
{
	char tmp[5], * ptr;

	strcpy_s(tmp, sizeof(tmp), http_delimetr);
	strcat_s(tmp, sizeof(tmp), http_delimetr);

	ptr = strstr_s(buf, buf_size, tmp);
	if (ptr == NULL) return NULL;
	else return ptr + strlen(tmp);
}

void http_create_answer(char* buf, size_t buf_size, const char* http_code, const char* http_delimetr, int content_length, const char* content_type)
{
	strcpy_s(buf, buf_size, "HTTP/1.1 ");
	strcat_s(buf, buf_size, http_code);
	strcat_s(buf, buf_size, http_delimetr);
	strcat_s(buf, buf_size, "Server: ");
	strcat_s(buf, buf_size, http_server);
	strcat_s(buf, buf_size, http_delimetr);
	strcat_s(buf, buf_size, "Content-Type: ");
	strcat_s(buf, buf_size, content_type);
	strcat_s(buf, buf_size, http_delimetr);
	strcat_s(buf, buf_size, "Content-Encoding: identity");
	strcat_s(buf, buf_size, http_delimetr);
	strcat_s(buf, buf_size, "Connection: Closed");
	strcat_s(buf, buf_size, http_delimetr);
	if (content_length) {
		strcat_s(buf, buf_size, "Content-Length: ");
		itoa(content_length, buf + strlen(buf), 10);
		strcat_s(buf, buf_size, http_delimetr);
	}
	strcat_s(buf, buf_size, http_delimetr);
	if (http_code != http_code_200) strcat_s(buf, buf_size, http_code);
}

void http_create_200_answer(char * buf, size_t buf_size, const char * http_delimetr, char * body, size_t body_size)
{
	http_create_answer(buf, buf_size, http_code_200, http_delimetr, body_size, "text/html");
	strncpy_s(buf + strlen(buf), buf_size - strlen(buf), body, body_size);
}


void parse_url(char * url, char * sheme, char * host, char * path, unsigned short * port)
{
  char * pch = url;
  char * pche, * pchb, * pcht;
  int l;
  int len = l = strlen(url);

  sheme[0] = '\0';
  host[0] = '\0';
  path[0] = '\0';
  *port = 0;

  if ( NULL != (pch = strstr_s(url, l, "://")) )
  {
     memcpy(sheme, url, pch - url);
     sheme[pch - url] = 0;
     pch += 3;
     l = len - (pch - url);
  } else pch = url;

  if ( NULL == (pche = strchr_s(pch, l, '/')) )
    pche = pch + l;

  pchb = pche;
  if ( NULL != (pcht = strchr_s(pch, pche - pch, ':')) )
  {
    *port = atoi(pcht + 1);
    if (*port == 0) {
       if ( NULL != (pcht = strchr_s(pcht + 1, pche - pch, ':')) ) {
         *port = atoi(pcht + 1);
         pche = pcht;
       }
    }
    else pche = pcht;
  }
  else *port = 0;

  memcpy(host, pch, pche - pch);
  host[pche - pch] = 0;
   pch = pchb;
  l = len - (pch - url);

  if ( NULL != (pche = strchr_s(pch, l, ' ')) ) {
    l = pche - pch;
  }

  memcpy(path, pch, l);
  path[l] = 0;
  if (strlen(path) == 0) { path[0] = '/'; path[1] = '\0'; }
}

void url_decode(char *input, char *output)
{
	char * ptr = output;
    char buffer[3];

	while(*input)
	{
		if(*input == '%')
		{
			buffer[0] = input[1];
            buffer[1] = input[2];
            buffer[2] = 0;
			*ptr++ = strtol(buffer, NULL, 16);
			input += 3;
		}
		else
		{
			if (*input == '+') *ptr = ' ';
			else *ptr = *input;
		    ptr++;
            input++;
        }

	}

	*ptr = 0; //null terminate
}

char * inner_text(char* str, size_t str_size, const char* tag,  size_t* len)
{
  char *ptr, *ptr2;
  char buf[128];
  size_t l;

  *len = 0;

  if (!str || !str_size || !tag) return NULL;

  snprintf(buf, sizeof(buf), "<%s", tag);

  ptr = strstr_s(str, str_size, buf);
  if (ptr) {
	  ptr += strlen(buf);
	  l = str_size - (ptr - str);
	  ptr2 = strchr_s(ptr, l, '>');
	  if (ptr2) {
		  ptr = ptr2 + 1;
		  l = str_size - (ptr - str);
		  snprintf(buf, sizeof(buf), "</%s>", tag);
		  ptr2 = strstr_s(ptr, l, buf);
		  if (ptr2) {
			  *len = ptr2 - ptr;
			  return ptr;
		  }
	  }
  }
  return NULL;
}


const unsigned char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64Decode(const unsigned char* input, unsigned int input_len, unsigned char* output, unsigned int* output_len )
{
    static char inalphabet[256], decoder[256];
    int i, bits, c = 0, char_count, errors = 0;
    unsigned int input_idx = 0;
    unsigned int output_idx = 0;

    for (i = (sizeof alphabet) - 1; i >= 0 ; i--) {
        inalphabet[alphabet[i]] = 1;
        decoder[alphabet[i]] = i;
    }

    char_count = 0;
    bits = 0;
    for( input_idx=0; input_idx < input_len ; input_idx++ ) {
        c = input[ input_idx ];
        if (c == '=')
            break;
        if (c > 255 || ! inalphabet[c])
            continue;
        bits += decoder[c];
        char_count++;
        if (char_count == 4) {
            output[ output_idx++ ] = (bits >> 16);
            output[ output_idx++ ] = ((bits >> 8) & 0xff);
            output[ output_idx++ ] = ( bits & 0xff);
            bits = 0;
            char_count = 0;
        } else {
            bits <<= 6;
        }
    }

    if( c == '=' ) {
        switch (char_count) {
            case 1:
#if (CC_TARGET_PLATFORM != CC_PLATFORM_BADA)
                fprintf(stderr, "base64Decode: encoding incomplete: at least 2 bits missing");
#endif
                errors++;
                break;
            case 2:
                output[ output_idx++ ] = ( bits >> 10 );
                break;
            case 3:
                output[ output_idx++ ] = ( bits >> 16 );
                output[ output_idx++ ] = (( bits >> 8 ) & 0xff);
                break;
            }
    } else if ( input_idx < input_len ) {
        if (char_count) {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_BADA)
            fprintf(stderr, "base64 encoding incomplete: at least %d bits truncated",
                    ((4 - char_count) * 6));
#endif
            errors++;
        }
    }

    *output_len = output_idx;
    return errors;
}

void base64Encode( const unsigned char* input, unsigned int input_len, char* output )
{
    unsigned int char_count;
    unsigned int bits;
    unsigned int input_idx = 0;
    unsigned int output_idx = 0;

    char_count = 0;
    bits = 0;
    for( input_idx=0; input_idx < input_len ; input_idx++ ) {
        bits |= input[ input_idx ];

        char_count++;
        if (char_count == 3) {
            output[ output_idx++ ] = alphabet[(bits >> 18) & 0x3f];
            output[ output_idx++ ] = alphabet[(bits >> 12) & 0x3f];
            output[ output_idx++ ] = alphabet[(bits >> 6) & 0x3f];
            output[ output_idx++ ] = alphabet[bits & 0x3f];
            bits = 0;
            char_count = 0;
        } else {
            bits <<= 8;
        }
    }

    if (char_count) {
        if (char_count == 1) {
            bits <<= 8;
        }

        output[ output_idx++ ] = alphabet[(bits >> 18) & 0x3f];
        output[ output_idx++ ] = alphabet[(bits >> 12) & 0x3f];
        if (char_count > 1) {
            output[ output_idx++ ] = alphabet[(bits >> 6) & 0x3f];
        } else {
            output[ output_idx++ ] = '=';
        }
        output[ output_idx++ ] = '=';
    }

    output[ output_idx++ ] = 0;
}


void parse_http_params(char* str, parse_http_params_item_func_t item_func, void* params)
{

    char* ptr, * ptre, * val;
    char buf[MAX_STR_SIZE];

    ptr = str;

    while(ptr) 
    {
        ptre = strstr_s(ptr, strlen(ptr), "&");
        if (ptre) 
        {
            strncpy_s(buf, sizeof(buf), ptr, ptre - ptr);
            ptr = ptre + 1;
        } 
        else 
        {
            strcpy_s(buf, sizeof(buf), ptr);
            ptr = NULL;
        }

        val = strstr_s(buf, strlen(buf), "=");
        if (val) 
        {
            *val = '\0';
            val++;
        }
        item_func(buf, val, params);
    }
}


const char*  http_find_uri_type(char* buf)
{
	if 	(strstr_s(buf, strlen(buf), ".htm")	        || strstr_s(buf, strlen(buf), ".html")) return "text/html";
	else if (strstr_s(buf, strlen(buf), ".gif"))						return "image/gif";
	else if (strstr_s(buf, strlen(buf), ".text") 	|| strstr_s(buf, strlen(buf),".txt"))	return "text/html";
	else if (strstr_s(buf, strlen(buf), ".jpeg") 	|| strstr_s(buf, strlen(buf),".jpg"))	return "image/jpeg";
	else if (strstr_s(buf, strlen(buf), ".swf")) 						return "application/x-shockwave-flash";
	else if (strstr_s(buf, strlen(buf), ".cgi") 	|| strstr_s(buf, strlen(buf),".CGI"))	return "text/html";
	else if (strstr_s(buf, strlen(buf), ".json") 	|| strstr_s(buf, strlen(buf),".JSON"))	return "application/json";
	else if (strstr_s(buf, strlen(buf), ".js") 	    || strstr_s(buf, strlen(buf),".JS"))	return "application/javascript";
	else if (strstr_s(buf, strlen(buf), ".xml") 	|| strstr_s(buf, strlen(buf),".XML"))	return "text/xml";
	else if (strstr_s(buf, strlen(buf), ".css") 	|| strstr_s(buf, strlen(buf),".CSS"))	return "text/css";
	else if (strstr_s(buf, strlen(buf), ".png") 	|| strstr_s(buf, strlen(buf),".PNG"))	return "image/pn";
	else if (strstr_s(buf, strlen(buf), ".ico") 	|| strstr_s(buf, strlen(buf),".ICO"))	return "image/x-icon";
	else if (strstr_s(buf, strlen(buf), ".ttf") 	|| strstr_s(buf, strlen(buf),".TTF"))	return "application/x-font-truetype";
	else if (strstr_s(buf, strlen(buf), ".otf") 	|| strstr_s(buf, strlen(buf),".OTF"))	return "application/x-font-opentype";
	else if (strstr_s(buf, strlen(buf), ".woff") 	|| strstr_s(buf, strlen(buf),".WOFF"))	return "application/font-woff";
	else if (strstr_s(buf, strlen(buf), ".eot") 	|| strstr_s(buf, strlen(buf),".EOT"))	return "application/vnd.ms-fontobject";
	else if (strstr_s(buf, strlen(buf), ".svg") 	|| strstr_s(buf, strlen(buf),".SVG"))	return "image/svg+xml";
	else 											return "text/html";
}
