
#ifndef MATH_UTIL_H_
#define MATH_UTIL_H_


#ifdef __cplusplus
extern "C" {
#endif

#ifndef M_PI
#define M_PI  3.1415926535 
#endif

//лучшая длина
unsigned char ullbestlength(unsigned long long val);
//crc16
unsigned short Crc16(unsigned char * pcBlock, unsigned short len);
//crc sum
unsigned int CrcSum(unsigned char * pcBlock, unsigned short len);
//crc32
unsigned int crc32(unsigned int crc_accum, unsigned char * data_blk_ptr, unsigned int data_blk_size);

unsigned long long ulCrcSum(unsigned char * pcBlock, unsigned long len);

unsigned int CrcSum16(unsigned short * pcBlock, unsigned short len);

//коэффиценты Герцеля
typedef struct _goertzel_coeff {
        float sine;
        float cosine;
        float coeff;
        float scalingFactor;
        int numSamples;
} goertzel_coeff_t;


void goertzel_get_coeff(int numSamples,int target_frequency, int sampling_rate, goertzel_coeff_t * pgoertzel_coeff);
float goertzel_get_mag(unsigned short * data, goertzel_coeff_t * pgoertzel_coeff);
unsigned int words_sum(unsigned short * data,  unsigned short data_size);
//Фильтр `Экспоненциальное бегущее среднее`. val - начальное значение, noise_coeff < 1 - коэффицент (1 без сглаживания), возвращает последнее значение
unsigned short noise_filter_normalised(unsigned short * data, unsigned short data_size, unsigned short val, float noise_coeff);
unsigned int bytes_sum(unsigned char * data,  unsigned short data_size);

extern float ABfilter_dt;
extern float ABfilter_sigma_process;
extern float ABfilter_sigma_noise;
unsigned short ABfilter(unsigned short newVal);

extern int kih_index;
extern float kih_gain;
unsigned short kih_filter(unsigned short in, unsigned short * values);

//typedef  short __int16;
//typedef int __int32;

//__int16 fir(__int16 NewSample);

#define ARRAY_MAX(arr, arr_cnt, res) { res = arr[0]; for (int i = 1; i < arr_cnt; ++i) if  (arr[i] > res) res = arr[i]; }
#define ARRAY_MIN(arr, arr_cnt, res) { res = arr[0]; for (int i = 1; i < arr_cnt; ++i) if  (arr[i] < res) res = arr[i]; }

#ifndef MODUL
#define MODUL(x) (x < 0 ? (-x) : (x))
#endif


#ifndef MIDDLE
#define MIDDLE(l, h) (l + (h - l) / 2)
#endif

#ifndef PROC
#define PROC(v, p)	((v * p) / 100)
#endif

//период в микросекундах в Герцы
unsigned long period_us2frequency(unsigned long period_us);


extern const short sin_const_20_2047[20];
extern const short sin_const_100_2047[100];

unsigned char bitget(void * mem, int num);
void bitset(void * mem, int num, unsigned char value);
//кол-во битов в 1
int bitis1cnt(void * mem, int max_num);


#ifdef __cplusplus
}
#endif

#endif /* UTIL_MATH_UTIL_H_ */
