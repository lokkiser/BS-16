
#include "ogg_util.h"
#include <string.h>
#include "math_util.h"


ogg_decoder_t  global_ogg_decoder = {0};

void ogg_decoder_init(ogg_decoder_t * ogg_decoder, ogg_stream_t stream, ogg_stream_read_t * stream_read)
{
  ogg_decoder->stream = stream;
  ogg_decoder->stream_read = stream_read;
  ogg_decoder->page_load = false;
  ogg_decoder->segment_index = 0;
#ifdef OGG_STATISTIC
  ogg_decoder->stat_pages_cnt = 0;
  ogg_decoder->stat_segments_cnt = 0;
  ogg_decoder->stat_bytes_cnt = 0;
#endif  
}


ogg_result_t ogg_read_page_data(ogg_decoder_t * ogg_decoder, ogg_page_t * ogg_page)
{
    uint8_t len, rc;

    rc = ogg_decoder->stream_read(ogg_decoder->stream, (uint8_t *)&ogg_page->capture_pattern, sizeof(ogg_page->capture_pattern));

    if (!rc) return OGG_RESULT_ERROR_READ_STREAM_EOFF;

    if (rc != sizeof(ogg_page->capture_pattern)) return OGG_RESULT_ERROR_READ_STREAM_CNT;

    if (ogg_page->capture_pattern != OGG_CAPTURE_PATTERN_UINT) return OGG_RESULT_ERROR_CAPTURE_PATTERN;

    len = sizeof(ogg_page_t) - sizeof(ogg_page->capture_pattern) - sizeof(ogg_page->segment_size);

    rc = ogg_decoder->stream_read(ogg_decoder->stream, (uint8_t *)&ogg_page->version, len);

    if (!rc) return OGG_RESULT_ERROR_READ_STREAM_EOFF;

    if (rc != len) return OGG_RESULT_ERROR_READ_STREAM_CNT;


    rc = ogg_decoder->stream_read(ogg_decoder->stream, (uint8_t *)ogg_page->segment_size, ogg_page->segments_cnt);

    if (!rc) return OGG_RESULT_ERROR_READ_STREAM_EOFF;

    if (rc != ogg_page->segments_cnt) return OGG_RESULT_ERROR_READ_STREAM_CNT;

    return OGG_RESULT_NO_ERROR;
}




ogg_result_t ogg_next_segment(ogg_decoder_t * ogg_decoder, uint8_t * segment, uint8_t * segment_size)
{

   if (!ogg_decoder->page_load) {
      ogg_result_t res = ogg_read_page_data(ogg_decoder, &ogg_decoder->page);
      if (res != OGG_RESULT_NO_ERROR) return res;
      ogg_decoder->page_load = TRUE;
      ogg_decoder->segment_index = 0;
#ifdef OGG_STATISTIC
      ogg_decoder->stat_pages_cnt++;
#endif
      if (ogg_decoder->page.segments_cnt == 0) {
        ogg_decoder->page_load = FALSE;
        ogg_decoder->segment_index = 0;
        return ogg_next_segment(ogg_decoder, segment, segment_size);
      }
   }


   if (ogg_decoder->page.segment_size[ogg_decoder->segment_index] == 0) {
        *segment_size = 0;
        ogg_decoder->segment_index++;
#ifdef OGG_STATISTIC
        ogg_decoder->stat_segments_cnt++;
#endif
   } else {
        *segment_size = ogg_decoder->stream_read(ogg_decoder->stream, segment, ogg_decoder->page.segment_size[ogg_decoder->segment_index]);
        if (!(*segment_size)) return OGG_RESULT_ERROR_READ_STREAM_EOFF;
        if (*segment_size != ogg_decoder->page.segment_size[ogg_decoder->segment_index]) return OGG_RESULT_ERROR_READ_STREAM_CNT;
        ogg_decoder->segment_index++;
#ifdef OGG_STATISTIC
        ogg_decoder->stat_segments_cnt++;
        ogg_decoder->stat_bytes_cnt += *segment_size;
#endif
   }

   if (ogg_decoder->segment_index >= ogg_decoder->page.segments_cnt) {
      ogg_decoder->page_load = FALSE;
      ogg_decoder->segment_index = 0;
   }

   return OGG_RESULT_NO_ERROR;
}

uint32_t ogg_get_page_full_segments_size(ogg_page_t * ogg_page)
{
   uint16_t i;
   uint32_t res;

   res = 0;
   for (i = 0; i < ogg_page->segments_cnt; i++) {
       res += ogg_page->segment_size[i];
   }
   return res;
}

uint8_t ogg_get_page_packets_count(ogg_page_t * ogg_page)
{
   uint16_t i;
   uint8_t res;

   res = 0;


   for (i = 0; i < ogg_page->segments_cnt; i++) {
     if (ogg_page->segment_size[i] == 255) continue;
     res++;
   }

   return res;
}


ogg_result_t ogg_write_page(ogg_stream_t stream, ogg_stream_write_t * ogg_stream_write, ogg_page_t * ogg_page, uint8_t segments_data[256][256])
{

   uint16_t i, page_len;
   uint8_t rc;

   ogg_page->checksum = 0;

   page_len = sizeof(ogg_page_t) - sizeof(ogg_page->segment_size) + ogg_page->segments_cnt;

   ogg_page->checksum = crc32(0, (unsigned char *)ogg_page, page_len);

   for (i = 0; i < ogg_page->segments_cnt; i++) {
       ogg_page->checksum = crc32(ogg_page->checksum, segments_data[i], ogg_page->segment_size[i]);
   }

   rc = ogg_stream_write(stream, (uint8_t *)ogg_page, sizeof(ogg_page_t) - sizeof(ogg_page->segment_size));
   if (rc != sizeof(ogg_page_t) - sizeof(ogg_page->segment_size)) return OGG_RESULT_ERROR_WRITE_PAGE_HEADER;

   rc = ogg_stream_write(stream, (uint8_t *)ogg_page->segment_size, ogg_page->segments_cnt);
   if (rc != ogg_page->segments_cnt) return OGG_RESULT_ERROR_WRITE_PAGE_SEGMENTS_SIZE;


   if (!segments_data) return OGG_RESULT_NO_ERROR;

   for (i = 0; i < ogg_page->segments_cnt; i++) {
     rc = ogg_stream_write(stream, segments_data[i], ogg_page->segment_size[i]);
     if (rc != ogg_page->segment_size[i]) return OGG_RESULT_ERROR_WRITE_SEGMENT;
   }

   return OGG_RESULT_NO_ERROR;
}

ogg_result_t ogg_write_page_segment(ogg_stream_t stream, ogg_stream_write_t * ogg_stream_write, uint8_t * segment, uint8_t segment_size)
{
	return ogg_stream_write(stream, segment, segment_size);
}


//------------------------- OPUS -----------------------------------------------

uint8_t * get_opus_tag(uint8_t * data, char * buf, uint8_t buf_size)
{
   uint32_t * plen, len;

   if (!buf_size) return data;

   plen = (uint32_t *)data;

   len = MIN(*plen, (uint32_t)(buf_size - 1));

   memcpy(buf, data + sizeof(uint32_t), len);
   buf[len] = '\0';

   return  data + sizeof(uint32_t) + *plen;
}

uint8_t * add_opus_data(uint8_t * buf, uint32_t buf_size, uint8_t * data, uint32_t data_size)
{
    uint8_t len;

    if (!buf || !data || (data_size > buf_size) || !buf_size || !data_size) return buf;

    len = MIN(buf_size, data_size);
    memcpy(buf, data, len);
    return buf + len;
}


uint8_t * add_opus_uint32(uint8_t * buf, uint32_t buf_size, uint32_t value)
{
  return add_opus_data(buf, buf_size, (uint8_t *)&value, sizeof(value));
}

uint8_t * add_opus_tag(uint8_t * buf, uint32_t buf_size, char * tag)
{
  uint32_t len;
  uint8_t * ptr;

  len = strlen(tag);
  ptr = add_opus_uint32(buf, buf_size, len);
  
  return add_opus_data(ptr, buf_size - (ptr - buf), (uint8_t *)tag, len);
}


