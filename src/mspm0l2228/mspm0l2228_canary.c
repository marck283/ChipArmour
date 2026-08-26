/**
 * @file canary.c
 * @author Orso Bruno
 * @brief This file provides the runtime support required for stack smashing
 *        protection.
 * @date 2026
 * 
 * @copyright Copyright (c) 2026 Università degli Studi di Trento
 */
#include "mspm0l2228_canary.h"


/**
 * @brief Global stack canary value used by compiler-emitted stack protection
 *        checks.
 */
uint32_t __stack_chk_guard = 0;

/**
 * @brief Handle a detected stack smashing event.
 * @post A fatal security response is triggered and normal execution does not
 *       resume.
 * @warning This function must never return.
 */
__attribute__((noreturn))
void __stack_chk_fail(void) {
    while (1) {
        // Stack has been corrupted, there are no guarantees about memory integrity.
        loop_forever();
    }
}


/**
 * @brief Generate a randomized stack canary value.
 * @return Returns a randomized canary value with its least significant byte
 *         cleared.
 * @note Clearing the least significant byte preserves compatibility with common
 *       stack protector expectations.
 */
static inline unsigned long generate_canary(void) {
    unsigned long canary = 0UL;
    random(&canary, sizeof(canary), 0xFF);

    canary &= ~((unsigned long)0xFF);
    return canary;
}


/**
 * @brief Initialize the global stack canary used by stack smashing protection.
 * @post The global stack canary guard is updated with a fresh randomized value.
 * @note This function must run before executing code protected by the compiler
 *       stack protector.
 * @warning This function is marked with no_stack_protector to avoid stack 
 *      smashing checks during its execution, as it is responsible for initializing 
 *      the stack canary used by the stack protector. Therefore, any calling function 
 *      must also be marked with no_stack_protector to ensure that the stack canary is 
 *      not checked before it is fully initialized.
 */
__attribute__((no_stack_protector))
void stack_canary_init(void) {
    sec_write_u32(&__stack_chk_guard, generate_canary());
}
