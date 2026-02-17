#ifndef HTTP_UTIL_H_
#define HTTP_UTIL_H_

#include <string.h>
#include "def_util.h"


#ifdef __cplusplus
extern "C" {
#endif


extern char * http_server;

extern const char http_get[];
extern const char http_post[];
extern const char http_put[];

extern const char http_code_200[];
extern const char http_code_400[];
extern const char http_code_401[];
extern const char http_code_404[];
extern const char http_code_500[];

#define	HTTP_UNKNOWN  	0
#define	HTTP_GET  		1
#define	HTTP_POST 		2
#define	HTTP_HEAD		3
#define	HTTP_PUT		4
#define	HTTP_PATCH		5
#define	HTTP_DELETE 	6
#define	HTTP_TRACE		7
#define	HTTP_CONNECT	8
#define	HTTP_OPTIONS	9





typedef struct _http_request_data {
	size_t request;
	const char * delimetr;
	char path[512];
	char * headers;
	size_t headers_len;
	char * body;
	size_t body_len;
	size_t content_length;
	char boundary[128];
} http_request_data_t;

bool http_parse_request(char * buf, size_t buf_size, http_request_data_t * http_request_data);

typedef struct _http_replay_data {
	size_t code;
	const char * delimetr;
	char * headers;
	size_t headers_len;
	char * body;
	size_t body_len;
	size_t content_length;
} http_replay_data_t;

bool http_parse_replay(char * buf, size_t buf_size, http_replay_data_t * http_replay_data);

//parse url
void parse_url(char * url, char * sheme, char * host, char * path, unsigned short * port);
//url decode
void url_decode(char *input, char *output);
//create http answer
void http_create_answer(char * buf, size_t buf_size, const char * http_code, const char * http_delimetr, int content_length, const char * content_type);
// create http 200 answer
void http_create_200_answer(char * buf, size_t buf_size, const char * http_delimetr, char * body, size_t body_size);
//get http delimetr \n or \r\n
const char * http_get_delimetr(char * buf, size_t buf_size);
//get http body
char * http_find_body(char * buf, size_t buf_size, const char * http_delimetr);
//inner tag text
char * inner_text(char * str, size_t str_size, const char * tag,  size_t *len);

int base64Decode(const unsigned char *input, unsigned int input_len, unsigned char *output, unsigned int *output_len);
void base64Encode( const unsigned char *input, unsigned int input_len, char *output );

//поиск authorization
char * find_authorization(char * headers, unsigned short headers_len, const char * r_n, unsigned short *authorization_len);
//поиск значения хидера
char * find_header_value(char * headers, unsigned short headers_len, char * header_name, const char * r_n, unsigned short *header_len);


typedef void parse_http_params_item_func_t(char * key, char * val, void * params);
//пармть строку вида "aaa=1&bbb=2&ccc=3"
void parse_http_params(char * str, parse_http_params_item_func_t item_func, void * params);


extern const char * http_request_str[];



const char *  http_find_uri_type(char * buf);


#ifdef __cplusplus
}
#endif

#endif /* HTTP_UTIL_H_ */
