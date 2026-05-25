/**
 * @defgroup MSPM0L2228_MPU MPU configuration
 * @brief Constants, error codes, and functions used for MPU configuration.
 * @{
 */

#ifndef MPU_H
#define MPU_H

/**
 * @def MPU_REGIONS_NUM
 * @brief Number of MPU regions.
 */
#define MPU_REGIONS_NUM 8

/**
 * @def ENOPOINTER
 * @brief Null pointer error code.
 */
#define ENOPOINTER -1

/**
 * @def SUCCESS
 * @brief Success code.
 */
#define SUCCESS 0

/** @} */

/**
 * @brief Memory Protection Unit (MPU) region configuration utilities for the HSM firmware.
 * @author Marco Lasagna
 * @date April 2026
 * 
 */

#include "ti_msp_dl_config.h"
#include "armv7m_mpu.h"

/**
 * @enum mpu_access_t
 * @brief Enumeration for MPU access types, including normal memory, device memory, and ordered memory
 */
typedef enum {
    NORMAL = 0,
    DEVICE = 1,
    ORDERED = 2
} mpu_access_t;

/**
 * @struct mpu_region_t
 * @brief Configuration structure for an MPU region, including its number, base
 *        address, access type, permissions, size, execution disable flag, cache 
 *        policies, and shareability.
 */
typedef struct mpu_region_t {
    const uint8_t region_num; // Region number (1-7)
    const uintptr_t base_addr; // Region base address
    const mpu_access_t access_type; // MPU access type
    const uint8_t ap; // Access permissions
    const uint8_t region_size; // Region size
    const uint8_t disable_exec; // Disable instruction execution (NX bit)
    const uint32_t outer_cp; // Outer cache policy
    const uint32_t inner_cp; // Inner cache policy
    const uint32_t shareable; // Shareable attribute
} mpu_region_t;

/**
 * @ingroup MSPM0L2228_MPU
 * 
 * @brief Set the MPU region to the given configuration. 
 * 
 * @param region The MPU region configuration to apply.
 * 
 * @note This function assumes the region parameters are all valid and does not perform any checks.
 */
static inline void set_mpu_region(mpu_region_t region) {
    uint32_t rasr_attr;
    switch (region.access_type) {
        case DEVICE: {
            rasr_attr = ARM_MPU_ACCESS_DEVICE(region.shareable);
            break;
        }
        case ORDERED: {
            rasr_attr = ARM_MPU_ACCESS_ORDERED;
            break;
        }
        case NORMAL:
        default: {
            rasr_attr = ARM_MPU_ACCESS_NORMAL(region.outer_cp, region.inner_cp, region.shareable);
            break;
        }
    }
    
    ARM_MPU_SetRegion(
        ARM_MPU_RBAR(region.region_num, region.base_addr),
        ARM_MPU_RASR_EX(region.disable_exec, region.ap, rasr_attr, 0b00000000,
                     region.region_size));
}

/**
 * @ingroup MSPM0L2228_MPU
 * 
 * @brief Configure and enable the Memory Protection Unit (MPU).
 *
 * This function installs the MPU regions used to isolate privileged kernel
 * memory, unprivileged user memory, flash regions, peripheral access, and the
 * kernel stack guard region.
 * 
 * @return 0 on success, or `ENOPOINTER` if `regions` is NULL.
 *
 * @post The MPU is enabled with the memory layout required by the HSM
 *       privilege-separation model.
 * @note The configured regions restrict unprivileged access to privileged
 *       kernel memory and protect the kernel stack with a dedicated no-access
 *       guard region.
 * @warning This function assumes Cortex-M0+ MPU with DREGION=8; re-validate MPU_TYPE.DREGION 
 *          when writing MPU protections for a different ARM MCU.
 */
static inline uint8_t init_mpu(mpu_region_t regions[MPU_REGIONS_NUM]) {
    if (regions == NULL) {
        return ENOPOINTER; // Handle NULL pointer input gracefully, though this should not happen in practice
    }

    for(uint8_t i = 0; i < MPU_REGIONS_NUM; i++) {
        set_mpu_region(regions[i]); // This is an observable side-effect, so the compiler cannot optimize this for loop away
    }

    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

    return SUCCESS; // Return success
}

#endif // MPU_H
