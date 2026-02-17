#include "math_util.h"
#include <math.h>


const short sin_const_20_2047[20] =  { 0, 632, 1203, 1656, 1946, 2047, 1946, 1656, 1203, 632, 0, -632, -1203, -1656, -1946, -2047, -1946, -1656, -1203, -632 };

const short sin_const_100_2047[100] =  { 0, 128, 256, 383, 509, 632, 753, 871, 986, 1096, 1203, 1304, 1401, 1492, 1577, 1656, 1728, 1793, 1852, 1903, 1946, 1982, 2010, 2030, 2042, 2047, 2042, 2030, 2010, 1982, 1946, 1903, 1852, 1793, 1728, 1656, 1577, 1492, 1401, 1304, 1203, 1096, 986, 871, 753, 632, 509, 383, 256, 128, 0, -128, -256, -383, -509, -632, -753, -871, -986, -1096, -1203, -1304, -1401, -1492, -1577, -1656, -1728, -1793, -1852, -1903, -1946, -1982, -2010, -2030, -2042, -2047, -2042, -2030, -2010, -1982, -1946, -1903, -1852, -1793, -1728, -1656, -1577, -1492, -1401, -1304, -1203, -1096, -986, -871, -753, -632, -509, -383, -256, -128 };
//лучшая длина
unsigned char ullbestlength(unsigned long long val)
{
  if (val <= 0xFF) return 1;
  else if (val <= 0xFFFF) return 2;
  else if (val <= 0xFFFFFF) return 3;
  else if (val <= 0xFFFFFFFF) return 4;
  else if (val <= 0xFFFFFFFFFF) return 5;
  else if (val <= 0xFFFFFFFFFFFF) return 6;
  else if (val <= 0xFFFFFFFFFFFFFF) return 7;
  else return 8;
}

//взято отсюда
//https://ru.wikibooks.org/wiki/%D0%A0%D0%B5%D0%B0%D0%BB%D0%B8%D0%B7%D0%B0%D1%86%D0%B8%D0%B8_%D0%B0%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC%D0%BE%D0%B2/%D0%A6%D0%B8%D0%BA%D0%BB%D0%B8%D1%87%D0%B5%D1%81%D0%BA%D0%B8%D0%B9_%D0%B8%D0%B7%D0%B1%D1%8B%D1%82%D0%BE%D1%87%D0%BD%D1%8B%D0%B9_%D0%BA%D0%BE%D0%B4
const unsigned short Crc16Table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

unsigned short Crc16(unsigned char * pcBlock, unsigned short len)
{
    unsigned short crc = 0xFFFF;

    while (len--)
        crc = (crc << 8) ^ Crc16Table[(crc >> 8) ^ *pcBlock++];

    return crc;
}

unsigned int CrcSum(unsigned char * pcBlock, unsigned short len)
{
	unsigned short i;
	unsigned int res;

	res = 0;

	for (i = 0; i < len ; ++i) res += pcBlock[i];
	return res;
}

unsigned long long ulCrcSum(unsigned char * pcBlock, unsigned long len)
{
	unsigned long i;
	unsigned long long res;

	res = 0;

	for (i = 0; i < len ; ++i) res += pcBlock[i];
	return res;
}

unsigned int CrcSum16(unsigned short * pcBlock, unsigned short len)
{
	unsigned short i;
	unsigned int res;

	res = 0;

	for (i = 0; i < len ; ++i) res += pcBlock[i];
	return res;
}

//----- Gloabl variables ------------------------------------------------------
const unsigned int crc32_table[]=       // Table of 8-bit remainders
{
0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,
0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039, 0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,
0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,
0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,
0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02,
0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,
0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,
0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,
0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,
0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,
0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B,
0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,
0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,
0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,
0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,
0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,
0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C,
0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
};

//=============================================================================
//=  CRC32 generation                                                         =
//=============================================================================
unsigned int crc32(unsigned int crc_accum, unsigned char * data_blk_ptr, unsigned int data_blk_size)
{
   register unsigned int i, j;

   for (j = 0; j < data_blk_size; j++) {
     i = ((int) (crc_accum >> 24) ^ *data_blk_ptr++) & 0xFF;
     crc_accum = (crc_accum << 8) ^ crc32_table[i];
   }
   crc_accum = ~crc_accum;


   return crc_accum ^ 0xFFFFFFFF;
}

//------------------------------  Герцель  ---------------------------------------------
//numSamples - кол-во точек для расчета, target_frequency необходимая частота в Гц, sampling_rate - частота дискретизации исходного сигнала
void goertzel_get_coeff(int numSamples,int target_frequency, int sampling_rate, goertzel_coeff_t * pgoertzel_coeff)
{
    int     k;
    float   floatnumSamples;
    float omega;


    pgoertzel_coeff->numSamples = numSamples;
    pgoertzel_coeff->scalingFactor = numSamples / 2.0;
    floatnumSamples = (float) numSamples;
    k = (int) (0.5 + ((floatnumSamples * target_frequency) / sampling_rate));
    omega = (2.0 * M_PI * k) / floatnumSamples;
    pgoertzel_coeff->sine = sin(omega);
    pgoertzel_coeff->cosine = cos(omega);
    pgoertzel_coeff->coeff = 2.0 * pgoertzel_coeff->cosine;
}

//data - данные, pgoertzel_coeff - коэф-ты Герцеля
float goertzel_get_mag(unsigned short * data, goertzel_coeff_t * pgoertzel_coeff)
{
  int i;
  float q0,q1,q2,magnitude,real,imag;

  q0 = 0;
  q1 = 0;
  q2 = 0;

  for(i = 0; i < pgoertzel_coeff->numSamples; i++)
  {
    q2 = q1;
    q1 = q0;
    q0 = pgoertzel_coeff->coeff * q1 - q2 + data[i];
  }

  // calculate the real and imaginary results
  // scaling appropriately
  real = (q0 - q1 * pgoertzel_coeff->cosine) / pgoertzel_coeff->scalingFactor;
  imag = (-q1 * pgoertzel_coeff->sine) / pgoertzel_coeff->scalingFactor;

  magnitude = sqrt(real*real + imag*imag);
  return magnitude;
}

unsigned int words_sum(unsigned short * data,  unsigned short data_size)
{
	unsigned int res;
	unsigned short i;

	res = data[0];
	for (i = 1; i < data_size; ++i) res += data[i];
	return res;
}

unsigned int bytes_sum(unsigned char * data,  unsigned short data_size)
{
	unsigned int res;
	unsigned short i;

	res = data[0];
	for (i = 1; i < data_size; ++i) res += data[i];
	return res;
}


void med(unsigned short *a, unsigned short *b, unsigned short n)
{
	unsigned short x;

    if ((a[n] > a[n+1]) && (a[n] > a[n+2])) {
        if(a[n+2] < a[n+1])
            x = a[n+1];
        else x = a[n+2];
	}
    else if((a[n+1] > a[n]) && (a[n+1] > a[n+2])) {
        if(a[n+2] < a[n])
            x = a[n];
        else x = a[n+2];
    }
    else if((a[n+2] > a[n]) && (a[n+2] > a[n+1])) {
        if(a[n] < a[n+1])
            x = a[n+1];
        else x = a[n];
    }
    else x = a[n];
    b[n] = x;
}

void noise_filter_median(unsigned short * data, unsigned short data_size)
{
	unsigned short i;

	for (i = 0; i < data_size - 2; ++i)  med(data, data, i);
	data[data_size - 2] = data[data_size - 3];
	data[data_size - 1] = data[data_size - 2];
}


unsigned short noise_filter_normalised(unsigned short * data, unsigned short data_size, unsigned short val, float noise_coeff)
{
	unsigned short i;

	for (i = 0; i < data_size; ++i)  {
		val += (data[i] - val) * noise_coeff;
		data[i] = val;
	}

	return val;
}


float ABfilter_dt = 0.05;
float ABfilter_sigma_process = 3.0;
float ABfilter_sigma_noise = 0.7;

unsigned short ABfilter(unsigned short newVal)
{
  static float xk_1, vk_1, a, b;
  static float xk, vk, rk;
  static float xm;
  float lambda = (float)ABfilter_sigma_process * ABfilter_dt * ABfilter_dt / ABfilter_sigma_noise;
  float r = (4 + lambda - (float)sqrt(8 * lambda + lambda * lambda)) / 4;
  a = (float)1 - r * r;
  b = (float)2 * (2 - a) - 4 * (float)sqrt(1 - a);
  xm = newVal;
  xk = xk_1 + ((float) vk_1 * ABfilter_dt );
  vk = vk_1;
  rk = xm - xk;
  xk += (float)a * rk;
  vk += (float)( b * rk ) / ABfilter_dt;
  xk_1 = xk;
  vk_1 = vk;
  return xk_1;
}


#define KIH_COEF_SIZE   5
const float kih_coef[KIH_COEF_SIZE] = { 0.021, 0.096, 0.146, 0.096, 0.021};
int kih_index; //k сохраняет индекс текущего массива, считанного для создания циклической памяти через массив
float kih_gain; //объявляем коэффициент усиления, чтобы масштабировать вывод до нормального

unsigned short kih_filter(unsigned short in, unsigned short * values)
{
  unsigned short out;
  int i;

  out = 0; //out — это возвращаемая переменная. Он устанавливается в 0 каждый раз, когда мы вызываем фильтр!

  values[kih_index] = in; //cохраняем ввод подпрограммы (содержимое переменной 'in') в массиве в текущей позиции указателя

  for (i = 1; i <= KIH_COEF_SIZE; i++) { //шагаем по массиву
    //... и складываем и умножаем каждое значение для накопления вывода
    //(i + kih_index) % filterTaps создает циклический способ прохождения массива
    out += kih_coef[i - 1] * values[(i + kih_index) % KIH_COEF_SIZE];

  }

  out /= kih_gain; //Нам нужно масштабировать вывод (если только коэффициенты не обеспечивают единичное усиление в полосе пропускания)

  kih_index = (kih_index + 1) % KIH_COEF_SIZE; // k увеличивается и обходит filterTaps, поэтому в следующий раз мы перезапишем самый старый сохраненный образец в массиве

  return out; //мы отправляем выходное значение обратно тому, кто вызвал подпрограмму
}


//период в микросекундах в Герцы
unsigned long period_us2frequency(unsigned long period_us)
{
	if (!period_us) return 0;

	return 1000000  / period_us;
}


unsigned char bitget(void * mem, int num)
{
	int byte_num, bit_num;
	unsigned char * bytes;

	byte_num  = num / 8;
	bit_num = num - byte_num * 8;
	bytes = (unsigned char *)mem;

	return bytes[byte_num] & (1 << bit_num) ? 1u : 0u;
}


int bitis1cnt(void * mem, int max_num)
{
	int i, res;

	res = 0;
	for (i = 0; i < max_num; ++i) {
		if (bitget(mem, i)) res++;
	}
	return res;
}



void bitset(void * mem, int num, unsigned char value)
{
	int byte_num, bit_num;
	unsigned char * bytes;

	byte_num  = num / 8;
	bit_num = num - byte_num * 8;
	bytes = (unsigned char *)mem;

	if (value) bytes[byte_num] |= (1 << bit_num);
	else bytes[byte_num] &=  ~(1 << bit_num);
}


/*
#define Ntap 5

#define DCgain 131072

unsigned short fir(unsigned short NewSample) {
    const unsigned short FIRCoef[Ntap] = {
        25420,
        26552,
        27125,
        26552,
        25420
    };

    static unsigned short x[Ntap]; //input samples
    unsigned long y=0;            //output sample
    int n;

    //shift the old samples
    for(n=Ntap-1; n>0; n--)
       x[n] = x[n-1];

    //Calculate the new output
    x[0] = NewSample;
    for(n=0; n<Ntap; n++)
        y += FIRCoef[n] * x[n];

    return y / DCgain;
}
*/
/*
#define NCoef 4
#define DCgain 8

short iir(short NewSample)
{
	const short ACoef[NCoef+1] = {
        13379,
            0,
        -26759,
            0,
        13379
    };

    const short BCoef[NCoef+1] = {
         8192,
        -18034,
        14416,
        -6127,
         1657
    };

    static int y[NCoef+1]; //output samples
    //Warning!!!!!! This variable should be signed (input sample width + Coefs width + 4 )-bit width to avoid saturation.

    static short x[NCoef+1]; //input samples
    int n;

    //shift the old samples
    for(n=NCoef; n>0; n--) {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    //Calculate the new output
    x[0] = NewSample;
    y[0] = ACoef[0] * x[0];
    for(n=1; n<=NCoef; n++)
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];

    y[0] /= BCoef[0];

    return y[0] / DCgain;
}


#define NCoef 2
#define DCgain 1

__int16 fir(__int16 NewSample) {
    const __int16 ACoef[NCoef+1] = {
        19627,
            0,
        -19627
    };

    const __int16 BCoef[NCoef+1] = {
        32768,
        -25198,
        -5112
    };

    static __int32 y[NCoef+1]; //output samples
    //Warning!!!!!! This variable should be signed (input sample width + Coefs width + 2 )-bit width to avoid saturation.

    static __int16 x[NCoef+1]; //input samples
    int n;

    //shift the old samples
    for(n=NCoef; n>0; n--) {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    //Calculate the new output
    x[0] = NewSample;
    y[0] = ACoef[0] * x[0];
    for(n=1; n<=NCoef; n++)
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];

    y[0] /= BCoef[0];

    return y[0] / DCgain;
}


#define NCoef 4
#define DCgain 1

short iir(short NewSample) {
	const short ACoef[NCoef+1] = {
        13707,
            0,
        -27414,
            0,
        13707
    };

	const short BCoef[NCoef+1] = {
        16384,
        -5329,
        -16454,
         1674,
         6071
    };

    static int y[NCoef+1]; //output samples
    //Warning!!!!!! This variable should be signed (input sample width + Coefs width + 4 )-bit width to avoid saturation.

    static short x[NCoef+1]; //input samples
    int n;

    //shift the old samples
    for(n=NCoef; n>0; n--) {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    //Calculate the new output
    x[0] = NewSample;
    y[0] = ACoef[0] * x[0];
    for(n=1; n<=NCoef; n++)
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];

    y[0] /= BCoef[0];

    return y[0] / DCgain;
}


#define Ntap 4

#define DCgain 65536

__int16 fir(__int16 NewSample) {
    const __int16 FIRCoef[Ntap] = {
        18219,
        27208,
        18219,
         1887
    };

    static __int16 x[Ntap]; //input samples
    __int32 y=0;            //output sample
    int n;

    //shift the old samples
    for(n=Ntap-1; n>0; n--)
       x[n] = x[n-1];

    //Calculate the new output
    x[0] = NewSample;
    for(n=0; n<Ntap; n++)
        y += FIRCoef[n] * x[n];

    return y / DCgain;
}
*/
