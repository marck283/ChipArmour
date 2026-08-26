#ifndef STARTUP_MSPM0L222X_TICLANG_H
#define STARTUP_MSPM0L222X_TICLANG_H

/**
 * @brief Remain in the lockdown path indefinitely.
 * @post This function never returns.
 */
__attribute__((naked,noreturn))
void loop_forever(void);

#endif
