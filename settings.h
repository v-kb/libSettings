#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

#include "version.h"

#ifdef STM32L031xx
#include "stm32l0xx_hal.h"
#elif defined(STM32L071xx)
// For STM32L071
#elif defined(STM32G0B1xx)
#include "stm32g0xx_hal.h"
#else
// For STM32L031
#endif

// todo: drop some of this values as they are not needed
typedef enum Status {
	SETTINGS_OK		= 0,
	SETTINGS_FAIL	= 1,	// 0b0000000000000001,
	FLASH_EMPTY		= 2,	// 0b0000000000000010,
	ID_WRONG		= 4,	// 0b0000000000000100,
	FW_WRONG		= 8,	// 0b0000000000001000,
	RT_NOT_FOUND	= 16,	// 0b0000000000010000,
	RT_FOUND		= 32,	// 0b0000000000100000,
	SIZE_WRONG		= 64,	// 0b0000000001000000,
	OUT_OF_RANGE	= 128,	// 0b0000000010000000,
	WRITE_FAIL		= 256	// 0b0000000100000000
} Settings_Status;

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
	BRIGHTNESS_OLED,
	BRIGHTNESS_MENU,
	MARK_COLOR,
#elif (ID1 == 10)
	CONTRAST,
	BRIGHTNESS,
	MARK_COLOR,
	MARK_BRIGHTNESS,
	PROFILE,
	MARK_OFFSET_X,
	MARK_OFFSET_Y,
	RF_OFFSET_X,
	RF_OFFSET_Y,
	RF_FREQUENCY_HZ,
	BINNING,
	LEAD_VISIBILITY,
	AGC_VISIBILITY,
	AGC_OFFSET_X,
	AGC_OFFSET_Y,
	AGC_WIDTH,
	AGC_HEIGHT,
	MODULE_CONTRAST,
	MODULE_BRIGHTNESS,
	GAMMA_CORRECTION,
	BC_MODE,
	CONTRAST_BIAS,
	MAX_CONTRAST,
	ORIENTATION,
	ALC_OFF,
	SLOW_FPS,
	SHUTTER_SPEED,
	EXPOSURE_TIME_NORMAL,
	EXPOSURE_TIME_SLOW_FPS,
	MANUAL_GAIN,
	DISPLAY_BRIGHTNESS,
	DISPLAY_CONTRAST,
	DISPLAY_VCOM,
	DISPLAY_TEMP_COMP,
	DISPLAY_GAMMA_ON,
	DISPLAY_GAMMA_VALUE,
	DISPLAY_TEST_PATTERN,
#elif (ID2 == 4) || (ID2 == 5)
	CONTRAST,
	BRIGHTNESS,
#else
	IDE,
	QUICK_SIGHTING,
	AUTO_INVERSION,
	MARK_COLOR,
	MARK_BRIGHTNESS,
#endif
	NUM_OF_SETTINGS
} Settings_IDs;

#define IS_EMPTY

typedef struct setting {
	Settings_IDs 	id;				///< ID as a string to identificate setting in debug - todo: redo with enum?
	int				val;			///< Actual value of the setting
	int 			def;			///< Default value for this setting
	int 			del;			///< Delta for this setting (used as a step to inc/dec value)
	int				min;			///< Minimum value for this setting
	int				max;			///< Maximum value for this setting
} Setting_TypeDef;

extern Setting_TypeDef s_ptr[NUM_OF_SETTINGS];
extern uint8_t settings_save;
extern uint32_t current_tick_counter; 	// milliseconds
extern uint32_t current_running_time; 	// seconds
extern uint32_t previous_running_time;	// seconds


Settings_Status settings_init(Setting_TypeDef *s_ptr);

void settings_read(Setting_TypeDef *s_ptr);
int settings_write(Setting_TypeDef *s_ptr);

int settings_value_inc			(Setting_TypeDef *s_ptr, Settings_IDs id);
int settings_value_inc_cyclic	(Setting_TypeDef *s_ptr, Settings_IDs id);
int settings_value_dec			(Setting_TypeDef *s_ptr, Settings_IDs id);
int settings_value_dec_cyclic	(Setting_TypeDef *s_ptr, Settings_IDs id);
int settings_value_tgl			(Setting_TypeDef *s_ptr, Settings_IDs id);
int settings_value_set			(Setting_TypeDef *s_ptr, Settings_IDs id, int new_val);
int settings_value_set_min		(Setting_TypeDef *s_ptr, Settings_IDs id);
int settings_value_set_max		(Setting_TypeDef *s_ptr, Settings_IDs id);
int settings_value_reset		(Setting_TypeDef *s_ptr, Settings_IDs id);
void settings_value_reset_all	(Setting_TypeDef *s_ptr);
void settings_value_drop_all	(Setting_TypeDef *s_ptr);

int device_running_time_check_old	(void);
void rt_update			(void);
void rt_time_save		(void);


#endif /* INC_SETTINGS_H_ */
