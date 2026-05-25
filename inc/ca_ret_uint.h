#ifndef CA_RET_UINT_H
#define CA_RET_UINT_H

/**
 * @defgroup ca_ret_uint
 * @brief Structures and functions to return uint values with fault injection armouring.
 * @{
 */

/**
    uint32_t returned by ca_ret_N functions, must be passed to comparison
     functions.
*/
typedef struct ca_uint32_t {
    uint32_t value; //!< The actual value
    uint32_t invvalue; //!< The bitwise inverse of the value, used for integrity checks to detect fault injection attempts
} ca_uint32_t;

/**
    uint16_t returned by ca_ret_N functions, must be passed to comparison
     functions.
*/
typedef struct ca_uint16_t {
    uint16_t value; //!< The actual value
    uint16_t invvalue; //!< The bitwise inverse of the value, used for integrity checks to detect fault injection attempts
} ca_uint16_t;

/**
    uint8_t returned by ca_ret_N functions, must be passed to comparison
     functions.
*/
typedef struct ca_uint8_t {
    uint8_t value; //!< The actual value
    uint8_t invvalue; //!< The bitwise inverse of the value, used for integrity checks to detect fault injection attempts
} ca_uint8_t;

/**
 * State struct for ca_ret_u32 to assist with FI armouring.
 */
typedef struct {
    uint32_t delay; //!< Random delay value, generated at the start of the function this structure is used in
    uint32_t i; //!< Loop counter, incremented in the loop to create the delay
    uint32_t local_value; //!< Local copy of the value being returned, modified in the loop to create a changing state that can be checked for FI attempts
} ca_ret_u32_state_t;

/**
 * State struct for ca_ret_u16 to assist with FI armouring.
 */
typedef struct {
    uint16_t delay; //!< Random delay value, generated at the start of the function this structure is used in
    uint16_t i; //!< Loop counter, incremented in the loop to create the delay
    uint16_t local_value; //!< Local copy of the value being returned, modified in the loop to create a changing state that can be checked for FI attempts
} ca_ret_u16_state_t;

/**
 * State struct for ca_ret_u8 to assist with FI armouring.
 */
typedef struct {
    uint8_t delay; //!< Random delay value, generated at the start of the function this structure is used in
    uint8_t i; //!< Loop counter, incremented in the loop to create the delay
    uint8_t local_value; //!< Local copy of the value being returned, modified in the loop to create a changing state that can be checked for FI attempts
} ca_ret_u8_state_t;

/**
    Returns a 32-bit unsigned int, but after a random delay to assist with 
    FI armouring.
*/
ca_uint32_t ca_retfast_u32(uint32_t value);

/**
    Returns a 16-bit unsigned int, but after a random delay to assist with 
    FI armouring.
*/
ca_uint16_t ca_retfast_u16(uint16_t value);

/**
    Returns an 8-bit unsigned int, but after a random delay to assist with 
    FI armouring.
*/
ca_uint8_t ca_retfast_u8(uint8_t value);

/** @} */

#endif
