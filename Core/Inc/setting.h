/*
 * setting.h
 *
 *  Created on: 16 мар. 2023 г.
 *      Author: Rafael Boltachev
 */

#ifndef _SETTING_H_
#define _SETTING_H_

#include "main.h"

#define	SETTING_KEY_MIC_VOLUME	"mic_volume"
#define	SETTING_KEY_SPK_VOLUME	"spk_volume"
#define	SETTING_KEY_DATE_TIME	"date_time"


typedef struct _settings
{
	uint16_t mic_volume; //
	uint16_t spk_volume; //
	char date_time[64]; //
} setting_t;

extern setting_t setting;

//загрузить настройки
bool load_setting(void);

//сохранить настройки
bool save_setting(void);


#endif /* CORE_INC_SETTING_H_ */
