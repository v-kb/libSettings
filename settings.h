#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

//#include "version.h"
#include <string.h>
#include "main.h"

//#ifdef STM32L031xx
//#include "stm32l0xx_hal.h"
//#elif defined(STM32L071xx)
//// For STM32L071
//#elif defined(STM32G0B1xx)
//#include "stm32g0xx_hal.h"
//#else
//// For STM32L031
//#endif

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


typedef struct setting {
	const char* 	name;			///< Name of setting
//	Settings_IDs 	id;				///< ID as a string to identificate setting in debug - todo: redo with enum?
	int				val;			///< Actual value of the setting
	int 			def;			///< Default value for this setting
	int 			del;			///< Delta for this setting (used as a step to inc/dec value)
	int				min;			///< Minimum value for this setting
	int				max;			///< Maximum value for this setting
	int				is_need_to_save;
	int				is_change_cyclic;
} Setting_TypeDef;

extern Setting_TypeDef s_ptr[];
extern uint8_t settings_save;
extern uint8_t save_running_time;
extern uint32_t current_tick_counter; 	// milliseconds
extern uint32_t current_running_time; 	// seconds
extern uint32_t previous_running_time;	// seconds


Settings_Status settings_init(Setting_TypeDef *s_ptr, uint16_t number_of_settings);

void settings_read(Setting_TypeDef *s_ptr);
int settings_write(Setting_TypeDef *s_ptr);

int settings_value_inc			(Setting_TypeDef *s_ptr);
int settings_value_inc_cyclic	(Setting_TypeDef *s_ptr);
int settings_value_dec			(Setting_TypeDef *s_ptr);
int settings_value_dec_cyclic	(Setting_TypeDef *s_ptr);
int settings_value_tgl			(Setting_TypeDef *s_ptr);
int settings_value_set			(Setting_TypeDef *s_ptr, int new_val);
int settings_value_set_min		(Setting_TypeDef *s_ptr);
int settings_value_set_max		(Setting_TypeDef *s_ptr);
int settings_value_set_def		(Setting_TypeDef *s_ptr);
void settings_value_set_def_all	(Setting_TypeDef *s_ptr);
void settings_value_drop_all	(Setting_TypeDef *s_ptr);

//int device_running_time_check_old	(void);
void running_time_update		(void);
void running_time_save			(void);
uint32_t running_time_get_s		(void);
uint32_t running_time_get_m		(void);
uint32_t running_time_get_h		(void);


#endif /* INC_SETTINGS_H_ */
