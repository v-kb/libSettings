#include <string.h>
#include "settings.h"
#include "version.h"

#ifdef STM32L031xx
	#define USER_DATA_BASEADDR		0x08080000
	#define EEPROM_SIZE				1024		// bytes
#elif defined(STM32L071xx)
	#define USER_DATA_BASEADDR		0x08080000	// For STM32L071
#else
	#define USER_DATA_BASEADDR		0x08080000	// For STM32L031
#endif

#define ADDR_DEVICE_ID			USER_DATA_BASEADDR
#define ADDR_DEVICE_FW			(USER_DATA_BASEADDR + 4)
#define ADDR_DEVICE_RT			(USER_DATA_BASEADDR + 8)
#define ADDR_SETTINGS_SIZE		(USER_DATA_BASEADDR + 12)
#define ADDR_SETTINGS			(USER_DATA_BASEADDR + 16)

#define ADDR_DEVICE_RT_OLD_1	(USER_DATA_BASEADDR + 16)
#define ADDR_DEVICE_RT_OLD_2	(USER_DATA_BASEADDR + 64)

#define DEVICE_ID 				((ID1 << 16) | ID2)
#define DEVICE_FW 				((FW1 << 24) | (FW2 << 16) | FW3)
#define DEVICE_ID_MIN 			(1 << 16)	// Minimum value for ID is 0x00010000
#define DEVICE_FW_MIN 			(1 << 24)	// Minimum value for FW is 0x01000000


/*
 * DO NOT DELETE FROM CODE!
 * This values for old firmware versions
 * where running time being written at these addresses.
 */
#ifdef OBSOLETE
#define EEPROM_BASEADDR		0x08080000

#define ADDR_IDE				((uint32_t)(EEPROM_BASEADDR + 0))
#define ADDR_QUICK_SIGHTING		((uint32_t)(EEPROM_BASEADDR + 4))
#define ADDR_AUTO_INVERSION		((uint32_t)(EEPROM_BASEADDR + 8))
#define ADDR_AR_COLOR			((uint32_t)(EEPROM_BASEADDR + 12))
#define ADDR_AR_BRIGHTNESS		((uint32_t)(EEPROM_BASEADDR + 16))

#define ADDR_MARK				(EEPROM_BASEADDR + 0)
#define ADDR_AMMO				(EEPROM_BASEADDR + 4)
#define ADDR_EYE_SENSOR			(EEPROM_BASEADDR + 8)
#define ADDR_EYE_SENSOR_VALUE	(EEPROM_BASEADDR + 12)

// On Clip-ons (Starting from Commits on Oct 21, 2022 "Added running time counter every 10s and on power off")
// It is only for Experimental type devices!
#define ADDR_DATE (EEPROM_BASEADDR + 16)

// On Scopes (Starting from Commits on Oct 27, 2022 "Added RTC to count and save running time")
#define ADDR_DATE (EEPROM_BASEADDR + 16)
// On Scopes (Starting from Commits on Feb 16, 2023 "Changed max mark brightness level to 5")
#define ADDR_DATE (EEPROM_BASEADDR + 64)	// To save month, date and year (all of them are uint8_t)


#define ADDR_TIME (ADDR_DATE + 3*4)			// Offset 3x32bit words from ADDR_DATE
#endif






uint32_t settings_size;
uint32_t device_id;
uint32_t current_tick_counter; // milliseconds
uint32_t previous_running_time; // seconds
uint32_t current_running_time; // seconds
uint8_t settings_save;
// todo: add statuses in all functions


/**
  * @brief  Writes user array to the specified FLASH memory address.
  * @param  page: is the used FLASH page.
  * @param  dest: specifies the start address to be erased.
  * @param  *src: is the source address of the data to be written.
  * @param  size: is the number of 32bit words to be written.
  * @note   This function is for various uCs.
  * @retval status returns number of errors occured in the process
  */
static int flash_write(uint32_t dest, int* src, uint32_t size) {
	//todo: add various uCs defined by device_id
	int status = 0;

	/* Erase and programm single word */
#ifdef STM32L031xx // For Scope type devices // todo: id1 change to ifdef STM32L031xx
	status = HAL_FLASHEx_DATAEEPROM_Unlock();
	if (status != HAL_OK) return status;

	for (int i = 0; i < size; ++i) {
		status += HAL_FLASHEx_DATAEEPROM_Erase(dest + 4*i);
		status += HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, dest + 4*i, src[i]);

		/* Exit loop on error */
		if (status != HAL_OK) break;
	}

	status += HAL_FLASHEx_DATAEEPROM_Lock();

#elif (DEVICE_ID == 1) || (DEVICE_ID == 2)
	FLASH_EraseInitTypeDef 	EraseInitStruct;
	uint32_t 				error;

	/* Unlock the Flash to enable the flash control register access */
	HAL_FLASH_Unlock();

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks		= FLASH_BANK_2;
	EraseInitStruct.Page		= page;
	EraseInitStruct.NbPages     = 1;

	/* Erase the user Flash area*/
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &error) != HAL_OK) {
		/*Error occurred while page erase.*/
		return HAL_FLASH_GetError ();
	}

	/* Program the user Flash area */
	if ( HAL_FLASH_Program(FLASH_TYPEPROGRAM_FAST, addr, ((uint64_t)(uintptr_t)data) ) != HAL_OK) {
		return HAL_FLASH_GetError ();
	}

	/* Lock the Flash */
	HAL_FLASH_Lock();
#else
//	FLASH_EraseInitTypeDef 	EraseInitStruct;
//	uint32_t 				error;
//
//	/* Unlock the Flash to enable the flash control register access */
//	HAL_FLASH_Unlock();
//
//	/* Fill EraseInit structure*/
//	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
//	EraseInitStruct.Banks		= FLASH_BANK_2;
//	EraseInitStruct.Page		= page;
//	EraseInitStruct.NbPages     = 1;
//
//	/* Erase the user Flash area*/
//	if (HAL_FLASHEx_Erase(&EraseInitStruct, &error) != HAL_OK) {
//		/*Error occurred while page erase.*/
//		return HAL_FLASH_GetError ();
//	}
//
//	/* Program the user Flash area */
//	if ( HAL_FLASH_Program(FLASH_TYPEPROGRAM_FAST, addr, ((uint64_t)(uintptr_t)data) ) != HAL_OK) {
//		return HAL_FLASH_GetError ();
//	}
//
//	/* Lock the Flash */
//	HAL_FLASH_Lock();
#endif

	return status;
}

/**
  * @brief  Read data from FLASH to user array from the specified memory address.
  * @param  *dest: is the destination address of the data to be written.
  * @param  src: specifies the start address to be read.
  * @param  size: is the number of 32bit words to be written.
  * @note   This function todo: MIGHT BE for various uCs.
  */
static void flash_read(int* dest, uint32_t src, uint32_t size) {
	for (int i = 0; i < size; ++i) dest[i] = *( (volatile int*)(src + 4*i) );
}

/**
  * @brief  Check if whole user page is empty
  * @retval	return 1 if falsh NOT empty, 0 if empty
  * @note   This function todo: MIGHT BE for various uCs.
  */
static int flash_check_is_empty(void) {
	int temp_buf[EEPROM_SIZE/4] = {0};

	flash_read(temp_buf, USER_DATA_BASEADDR, EEPROM_SIZE/4);

	for (int i = 0; i < EEPROM_SIZE/4; ++i)
		if (temp_buf[i] != 0) return 0;	// Break on the first non-zero value

	return 1;
}

/**
  * @brief  Check the size of settings that were written in user FLASH
  * @retval returns 1 if size is 0; 2 if not correct and 0 if OK
  */
static uint32_t settings_size_read(void) {
	return *( (volatile uint32_t*)ADDR_SETTINGS_SIZE );
}

/**
  * @brief  Check the size of settings that were written in user FLASH
  * @retval returns 1 if size is not correct and 0 if OK
  */
static inline int settings_size_write(int size) {
	return flash_write(ADDR_SETTINGS_SIZE, &size, 1);
}

/**
 * @brief  Read device ID from the FLASH
 * @retval returns 1 if fields are "empty"; 2 if value is not correct; 0 if OK.
 */
static uint32_t device_id_read(void) {
	return *( (volatile uint32_t*)ADDR_DEVICE_ID );
}

/**
 * @brief  Read device firmware version from the FLASH
 * @retval returns device firmware version as uint32_t
 */
static uint32_t device_fw_read(void) {
    return *( (volatile uint32_t*)ADDR_DEVICE_FW );
}

/**
 * @brief  Write device ID to the FLASH
 * @retval returns write results.
 */
static int device_id_fw_write(void) {
	int data[2] = {((ID1 << 16) | ID2), ((FW1 << 24) | (FW2 << 16) | FW3)};
    return flash_write(ADDR_DEVICE_ID, data, 2);
}


/**
 * @brief	Checks if any of settings is out of boundaries and set them to defaults
 * @retval	returns count of out-of-boundaries settings.
 */
static uint8_t settings_check_is_valid(Setting_TypeDef *s_ptr) {
	assert_param(s_ptr == NULL);

	uint8_t non_valid_values = 0;

	for (Settings_IDs id = 0; id < NUM_OF_SETTINGS; ++id) {
		if(s_ptr[id].val > s_ptr[id].max || s_ptr[id].val < s_ptr[id].min) {
			s_ptr[id].val = s_ptr[id].def;
			++non_valid_values;
		}
	}

	if(non_valid_values > 0) return 0;

	return 1;
}

void settings_read(Setting_TypeDef *s_ptr) {
	assert_param(s_ptr == NULL);

	int temp_buffer[NUM_OF_SETTINGS] = {0};

	/* Copy all settings (N x uint32_t) from FLASH to the buffer */
	flash_read(temp_buffer, ADDR_SETTINGS, NUM_OF_SETTINGS);

	/* Transfer them to the structures */
	for (Settings_IDs id = 0; id < NUM_OF_SETTINGS; ++id)
		s_ptr[id].val = temp_buffer[id];
}

int settings_write(Setting_TypeDef *s_ptr) {
	assert_param(s_ptr == NULL);

	int	status = 0;
	int temp_buffer[NUM_OF_SETTINGS] = {0};

	/* Copy values to the temp buffer */
	for (int i = 0; i < NUM_OF_SETTINGS; ++i) {
		temp_buffer[i] = s_ptr[i].val;
	}

	/* Write from temp buffer to the FLASH memory */
	status = flash_write(ADDR_SETTINGS, temp_buffer, NUM_OF_SETTINGS);
	if(status != 0) return status;

	/*
	 * Check Read:
	 * Copy from memory to the temp again
	 * and compare with values in structures
	 */
//	memcpy(temp_buffer, (const int*)ADDR_SETTINGS, NUM_OF_SETTINGS*sizeof(int));
	flash_read(temp_buffer, ADDR_SETTINGS, NUM_OF_SETTINGS);

	for (int i = 0; i < NUM_OF_SETTINGS; ++i) {
		if (temp_buffer[i] != s_ptr[i].val) ++status;
	}

	/* If data is equal then status == 0, everything is ok, return 0 */
	return status;
}



/**
 * @brief  Restore settings from FLASH or set the defaults.
 * @retval returns 1 if s_ptr is NULL or ID is wrong; 2 if new value is out of range, and 0 if OK.
 */
Settings_Status settings_init(Setting_TypeDef *s_ptr) {
	assert_param(s_ptr == NULL);

	Settings_Status status 					= 0;
	uint8_t 		restore_defaults_flag 	= 0;
	uint8_t 		is_have_old_rtc_data 	= 0;

	/*
	 * Check if all user-defined IDs correspond to predefined enums
	 */
	for (Settings_IDs id = 0; id < NUM_OF_SETTINGS; ++id) {
		if (id != s_ptr[id].id) return SETTINGS_FAIL;
	}

	/*
	 * Check if FLASH has any records
	 */
	volatile uint8_t is_empty = flash_check_is_empty();
	if (is_empty) {
		restore_defaults_flag = 1;
		status |= FLASH_EMPTY;
	} else {
		/*
		 * Check FW, ID and settings size
		 * to fit the device type
		 * and current firmware version
		 */
		volatile uint32_t id 	= device_id_read	();
		volatile uint32_t fw 	= device_fw_read	();
		volatile uint32_t size 	= settings_size_read();

		if (id == DEVICE_ID && size == NUM_OF_SETTINGS && fw >= DEVICE_FW_MIN) {
			/*
			 * Restore settings from FLASH
			 */
			settings_read(s_ptr);

			/*
			 * Correct and rewrite values if some of them are incorrect
			 */
			volatile uint8_t is_valid = settings_check_is_valid(s_ptr);
			if (!is_valid) {
				restore_defaults_flag = 1;
				status |= OUT_OF_RANGE;
			}
		} else {
			restore_defaults_flag = 1;
			status |= (ID_WRONG | SIZE_WRONG);

#if (ID1 == 1 || ID1 == 2) // Only for Scopes and Clip-ons
			/*
			 * Search for the saved running time records
			 * in case FW tells us that it's previous version of firmware
			 */
			if (fw < DEVICE_FW_MIN) {
				is_have_old_rtc_data = device_running_time_check_old();
				status |= FW_WRONG;
			}
#endif
		}
	}

	/*
	 * Set everything to defaults, save and exit (returns 3 write results)
	 */
	if (restore_defaults_flag) {
		settings_value_reset_all(s_ptr);

		int res = 0;
		res += device_id_fw_write	();
		res += settings_size_write	(NUM_OF_SETTINGS); // What if size will overlay RTC values?
		res += settings_write		(s_ptr);
		if (res > 0) status |= WRITE_FAIL;
	}

	/*
	 * Read saved running time in seconds
	 */
	if (!is_have_old_rtc_data) {		// Restore previous value to variable
		previous_running_time = *( (volatile uint32_t*)ADDR_DEVICE_RT );
//		status |= RT_NOT_FOUND;
	} else {							// Save to the new location old RT value
		rt_time_save();
	}


	return status;
};

/**
 * @brief  Increase the value of a setting.
 * @param  id: ID of the setting to increase.
 * @retval returns 1 if s_ptr is NULL or ID is wrong; 2 if new value is out of range, and 0 if OK.
 */
int settings_value_inc(Setting_TypeDef *s_ptr, Settings_IDs id) {
	assert_param(s_ptr == NULL);

    if(id != s_ptr[id].id) return 1;

	int new_val = s_ptr[id].val + s_ptr[id].del;

	if (new_val > s_ptr[id].max) return 2;

	s_ptr[id].val = new_val;
	return 0;
}

/**
 * @brief  Increase the value of a setting in cycle.
 * @param  id: ID of the setting to increase.
 * @retval returns 1 if s_ptr is NULL or ID is wrong; 2 if new value is out of range, and 0 if OK.
 */
int settings_value_inc_cyclic(Setting_TypeDef *s_ptr, Settings_IDs id) {
	assert_param(s_ptr == NULL);

    if (id != s_ptr[id].id) return 1;

	int new_val = s_ptr[id].val + s_ptr[id].del;

	if (new_val > s_ptr[id].max) new_val = s_ptr[id].min;

	s_ptr[id].val = new_val;
	return 0;
}

/**
 * @brief  Decrease the value of a setting.
 * @param  id ID of the setting to decrease.
 * @retval returns 1 if s_ptr is NULL; 2 if new value is out of range, and 0 if OK.
 */
int settings_value_dec(Setting_TypeDef *s_ptr, Settings_IDs id) {
	assert_param(s_ptr == NULL);

    if (id != s_ptr[id].id) return 1;

	int new_val = s_ptr[id].val - s_ptr[id].del;

	if (new_val < s_ptr[id].min) return 2;

	s_ptr[id].val = new_val;
	return 0;
}

/**
 * @brief  Decrease the value of a setting in cycle.
 * @param  id: ID of the setting to increase.
 * @retval returns 1 if s_ptr is NULL or ID is wrong; 2 if new value is out of range, and 0 if OK.
 */
int settings_value_dec_cyclic(Setting_TypeDef *s_ptr, Settings_IDs id) {
	assert_param(s_ptr == NULL);

    if (id != s_ptr[id].id) return 1;

	int new_val = s_ptr[id].val - s_ptr[id].del;

	if (new_val < s_ptr[id].min) new_val = s_ptr[id].max;

	s_ptr[id].val = new_val;
	return 0;
}


/**
 * @brief  Toggle between min and max values of a setting.
 * @param  id: ID of the setting to decrease.
 * @retval returns 1 if s_ptr is NULL; 0 if OK.
 */
int settings_value_tgl(Setting_TypeDef *s_ptr, Settings_IDs id) {
	assert_param(s_ptr == NULL);

    if (id != s_ptr[id].id) return 1;

	s_ptr[id].val = s_ptr[id].val == s_ptr[id].min ? s_ptr[id].max : s_ptr[id].min;

	return 0;
}

/**
 * @brief  Set the value of a setting to the specified value.
 * @param  id: ID of the setting to decrease.
 * @param  new_val: New value to be set
 * @retval returns -1 if ID is not found or s_ptr is NULL; -2 new value is out of range, and 0 if OK.
 */
int settings_value_set(Setting_TypeDef *s_ptr, Settings_IDs id, int new_val) {
	assert_param(s_ptr == NULL);

    if (id != s_ptr[id].id) return 1;

	if (new_val < s_ptr[id].min || new_val > s_ptr[id].max) return 2;

	s_ptr[id].val = new_val;
	return 0;
}

/**
 * @brief  Resets the value of the specified setting to the default value.
 * @param  id: ID of the setting to reset.
 * @retval returns 2 if ID is not found; 1 if s_ptr is NULL; 0 if OK.
 */
int settings_value_reset(Setting_TypeDef *s_ptr, Settings_IDs id) {
	assert_param(s_ptr == NULL);

    if (id != s_ptr[id].id) return 1;

	s_ptr[id].val = s_ptr[id].def;
	return 0;
}

/**
 * @brief  Sets values in all settings to zero (reset flash).
 * @param  id: ID of the setting to decrease.
 * @retval returns -2 if ID is not found; -1 if s_ptr is NULL; 0 if OK.
 */
void settings_value_drop_all(Setting_TypeDef *s_ptr) {
	assert_param(s_ptr == NULL);
	for (int i = 0; i < NUM_OF_SETTINGS; ++i)
		s_ptr[i].val = -1;
}

/**
 * @brief  Resets values in all settings.
 * @param  id: ID of the setting to decrease.
 * @retval returns -2 if ID is not found; -1 if s_ptr is NULL; 0 if OK.
 */
void settings_value_reset_all(Setting_TypeDef *s_ptr) {
	assert_param(s_ptr == NULL);
	for (int i = 0; i < NUM_OF_SETTINGS; ++i)
		s_ptr[i].val = s_ptr[i].def;
}



///**
// * @brief  Read device running time counter from the FLASH
// * @retval returns 1 if data is off and 2 if time is off; 0 if OK.
// */
//void device_running_time_read(RTC_DateTypeDef *date, RTC_TimeTypeDef *time) {
//	int temp_buffer[6] = {0};
//
//
//
////    /* Set correct values */
////    date->Date 		= temp_buffer[0];
////    date->Month 	= temp_buffer[1];
////    date->Year 		= temp_buffer[2];
////    time->Hours 	= temp_buffer[3];
////    time->Minutes 	= temp_buffer[4];
////    time->Seconds 	= temp_buffer[5];
//}

///**
// * @brief  Read device running time counter from the FLASH
// * @retval returns 1 if data is off and 2 if time is off; 0 if OK.
// */
//int device_running_time_write(uint32_t *rt) {
//    return
//}

/**
 * @brief  Read device running time counter from the FLASH
 * @retval match returns 1 if time records found @ first addr; 2 if @ second; 0 if nothing found.
 */
int device_running_time_check_old(void) {
	int temp_buffer[6] = {0};
	int match = 0;

	/*
	 * For the Scopes check if RT is written @ (USER_DATA_BASEADDR + 16) or (USER_DATA_BASEADDR + 64) - check both
	 * For the Clip-Ons only check if RT is written @ (USER_DATA_BASEADDR + 16)
	 */

	/*
	 * Check first possible address for Clip-ons and Scopes
	 * 	* Check only hours, minutes amd seconds
	 * 	 because they are most likely non-zeros.
	 * 	 ** And year field can be non-zero
	 * 	 because it is actually mark brightness field in Scopes.
	 */
	flash_read(temp_buffer, ADDR_DEVICE_RT_OLD_1, 6);
	if (temp_buffer[3] > 0 || temp_buffer[4] > 0 || temp_buffer[5] > 0) {
		match = 1;

	/*
	 * If nothing found @ ADDR_DEVICE_RT_OLD_1, check second possible address for Scopes
	 */
	} else if (match == 0 && ID1 == 1) {
		flash_read(temp_buffer, ADDR_DEVICE_RT_OLD_2, 6);
		if (temp_buffer[3] > 0 || temp_buffer[4] > 0 || temp_buffer[5] > 0)
			match = 2;
	}

	/* Set values if there is a match */
	if (match > 0) {
		previous_running_time = \
								temp_buffer[1]*30*24*60*60 	+ \
								temp_buffer[2]*24*60*60 	+ \
								temp_buffer[3]*60*60 		+ \
								temp_buffer[4]*60 			+
								temp_buffer[5];
	}

	// Note: check for correctness is performed separately in rtc_init() function

	return match;
}


void rt_update(void) {
	static uint16_t save_counter = 0;

	current_tick_counter = HAL_GetTick();
	current_running_time = current_tick_counter/1000;
	/*
	 * Add 1 second to counter and check for counting 10 minutes
	 * (1x60x10 = 600, with rtc_save_counter updates every second)
	 */
	if((++save_counter) == 600) {
		rt_time_save();
		save_counter = 0;
	}
}

void rt_time_save(void) {
	current_tick_counter = HAL_GetTick();
	current_running_time = current_tick_counter/1000;
//	previous_running_time += current_running_time;
	uint32_t running_time = previous_running_time + current_running_time;
	flash_write(ADDR_DEVICE_RT, &running_time, 1);
}

void HAL_PWR_PVDCallback(void) {
	static uint8_t done = 0;

	if(!done) {
//		settings_write_running_time(&date, &time);
		done = 1;
	}
}
