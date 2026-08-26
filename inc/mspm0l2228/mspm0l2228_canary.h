/**
 * @file canary.h
 * @author Orso Bruno
 * @brief This header file provides the API used to initialize stack canary
 *        protection for the firmware.
 * @date 2026
 * 
 * @copyright Copyright (c) 2026 Università degli Studi di Trento
 */
#ifndef MSPM0L2228_CANARY_H
#define MSPM0L2228_CANARY_H

#pragma once

#include "random.h"

/**
 * @brief Initialize the global stack canary value used by stack smashing
 *        protection.
 * @post The global stack canary guard is initialized with fresh random data.
 * @note This function must be called before executing code protected by the
 *       compiler stack protector.
 */
__attribute__((no_stack_protector))
void stack_canary_init(void);

#endif
