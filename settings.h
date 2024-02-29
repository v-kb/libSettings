#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

#include "stm32l0xx_hal.h"
#include "property.h"
#include "version.h"


typedef enum Settings {
#if (ID1 == 1) // For Scope type devices
	IDE,
	QUICK_SIGHTING,
	AUTO_INVERSION,
	MARK_COLOR,
	MARK_BRIGHTNESS,
	MENU_BRIGHTNESS,
	DISP_BRIGHTNESS,
#elif (ID1 == 2)
	MARK_TYPE,
	AMMO_TYPE,
	EYE_SENSOR_EN,
	EYE_SENSOR_VALUE,
#else
	IDE,
	QUICK_SIGHTING,
	AUTO_INVERSION,
	MARK_COLOR,
	MARK_BRIGHTNESS,
#endif
	NUM_OF_SETTINGS
} Settings_IDs;

typedef struct setting {
	Settings_IDs 	id;				///< ID as a string to identificate setting in debug - todo: redo with enum?
	int				val;			///< Actual value of the setting
	int 			def;			///< Default value for this setting
	int 			del;			///< Delta for this setting (used as a step to inc/dec value)
	int				min;			///< Minimum value for this setting
	int				max;			///< Maximum value for this setting
} Setting_TypeDef;

extern uint8_t settings_save;
extern uint32_t current_tick_counter; // milliseconds
extern uint32_t current_running_time; 	// seconds
extern uint32_t previous_running_time;


uint8_t settings_init(Setting_TypeDef *s_ptr);

void settings_read(Setting_TypeDef *s_ptr);
int settings_write(Setting_TypeDef *s_ptr);

int settings_value_inc			(Setting_TypeDef *s_ptr, Settings_IDs id);
int settings_value_inc_cyclic	(Setting_TypeDef *s_ptr, Settings_IDs id);
int settings_value_dec			(Setting_TypeDef *s_ptr, Settings_IDs id);
int settings_value_tgl			(Setting_TypeDef *s_ptr, Settings_IDs id);
int settings_value_set			(Setting_TypeDef *s_ptr, Settings_IDs id, int new_val);
int settings_value_reset		(Setting_TypeDef *s_ptr, Settings_IDs id);
void settings_value_reset_all	(Setting_TypeDef *s_ptr);

int device_running_time_check_old	(void);
void rt_update			(void);
void rt_time_save		(void);


#endif /* INC_SETTINGS_H_ */
