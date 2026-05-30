#include "random.h"

/**
 * @struct random_state_t
 * @brief Internal state used by the random number generation subsystem.
 */
typedef struct {
    uint8_t key[32];          /**< Key material used by the DRBG. */
    uint8_t nonce[12];        /**< Nonce used for random generation. */
    uint32_t ctr;             /**< Internal counter used by the generator. */
    uint8_t trng_initialized; /**< Indicates whether the TRNG has been initialized. */
} random_state_t;

/** Internal state of the random generator. */
static random_state_t random_state = { .trng_initialized = -1 };