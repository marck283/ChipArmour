/*
This file is part of ChipArmour™, by NewAE Technology Inc.

ChipArmour™ is Copyright 2019-2020 NewAE Technology Inc.

ChipArmour™ is a trademark of NewAE Technology Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at:

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/
#include "../inc/chiparmour.h"
#include "panic.h"

int ca_atmine(void)
{}

int ca_atwait(void)
{}

int ca_fastwait(void)
{}

typedef void (*ca_funcpointer)(void *);

#define __check_and_set_min(input, min) \
do { \
    FULL_IF_FAILOUT (ca_comp(input.value, min.value, <)) { \
        input.value = min.value; \
        input.invvalue = min.invvalue; \
    } \
} while(0)

#define __check_and_set_max(input, max) \
do { \
    FULL_IF_FAILOUT (ca_comp(input.value, max.value, >)) { \
        input.value = max.value; \
        input.invvalue = max.invvalue; \
    } \
} while(0)

/**
 * @brief Checks if the input is within the min and max values, if not sets it to the min or max. 
 * Jumps to panic if the input value is tampered with during the function.
 * 
 * @param input The value to check and limit
 * @param min The minimum value allowed
 * @param max The maximum value allowed
 * @return uint32_t The limited value
 */
uint32_t _ca_limit_u32(ca_uint32_t input, ca_uint32_t min, ca_uint32_t max)
{
    ca_landmine();

    volatile ca_uint32_t input_copy = input;
    volatile ca_uint32_t min_copy = min;
    volatile ca_uint32_t max_copy = max;
    
    
    //Quick version - just have multiple checks
    __cmp_and_panic(input_copy.invvalue, ~input_copy.value, !=);
    __cmp_and_panic(min_copy.invvalue, ~min_copy.value, !=);
    __cmp_and_panic(max_copy.invvalue, ~max_copy.value, !=);
    
    __check_and_set_min(input_copy, min);
    __check_and_set_max(input_copy, max);
  
    __cmp_and_panic(input_copy.invvalue, ~input_copy.value, !=);
    __cmp_and_panic(min_copy.invvalue, ~min_copy.value, !=);
    __cmp_and_panic(max_copy.invvalue, ~max_copy.value, !=);

    __check_and_set_min(input_copy, min);    
    __check_and_set_max(input_copy, max);
  
    __cmp_and_panic(input_copy.invvalue, ~input_copy.value, !=);
    __cmp_and_panic(min_copy.invvalue, ~min_copy.value, !=);
    __cmp_and_panic(max_copy.invvalue, ~max_copy.value, !=);

    __check_and_set_min(input_copy, min);    
    __check_and_set_max(input_copy, max);
  
    __cmp_and_panic(input_copy.invvalue, ~input_copy.value, !=);
    __cmp_and_panic(min_copy.invvalue, ~min_copy.value, !=);
    __cmp_and_panic(max_copy.invvalue, ~max_copy.value, !=);
    
    ca_landmine();
    
    return input_copy.value;

    ca_panic(); // You can only get here by skipping the return instruction, which is a fault injection attempt.
}

#define CA_CMP_LOOPS 3

/*
  Compares two numbers, jumps to a function if they are the same or different. Commonly used for
  verifing a signature.
*/
ca_return_t _ca_compare_u32_eq(ca_uint32_t op1,
                   ca_uint32_t op2,
                  ca_fptr_voidptr_t  equal_function,
                  void * equal_func_param,
                  ca_fptr_voidptr_t  unequal_function,
                  void * unequal_func_param)
{
    uintptr_t equal_function_mask = (uintptr_t)equal_function;
    uintptr_t equal_func_param_mask = (uintptr_t)equal_func_param;
    uintptr_t unequal_function_mask = (uintptr_t)unequal_function;
    uintptr_t unequal_func_param_mask = (uintptr_t)unequal_func_param;

    ca_landmine();
    
    //Mask values we'll jump to, make later FI skips increase chance we jump
    //to some invalid value.
    equal_function_mask ^= (CA_CMP_LOOPS << 15);
    equal_func_param_mask ^= (CA_CMP_LOOPS << 15);
    ca_landmine();
    unequal_function_mask ^= (CA_CMP_LOOPS << 15);
    unequal_func_param_mask ^= (CA_CMP_LOOPS << 15);
    
    volatile uint32_t equal = 0;
    volatile uint32_t unequal = 0;
    
    volatile int i = -1000;
    ca_landmine();
    
    goto CA_DO_LOOP;
    
    ca_panic();
    ca_landmine();
    ca_panic();
    ca_landmine();
    ca_panic();
    
CA_DO_COMPARE:
    FULL_IF_FAILOUT (i == CA_CMP_LOOPS) {
        
        ca_atmine();
        ca_atwait();
        
        FULL_IF_FAILOUT (equal == CA_CMP_LOOPS) {
            ca_atmine();
            ca_atwait();
            FULL_IF_FAILOUT (equal == CA_CMP_LOOPS) {
                __cmp_and_panic(equal_function_mask, (uintptr_t)equal_function, !=);
                __cmp_and_panic(equal_func_param_mask, (uintptr_t)equal_func_param, !=);
                if(equal_function) {
                    equal_function(equal_func_param);
                }
                return CA_SUCCESS;
            } else {
                ca_panic();
            }
        }
        
        FULL_IF_FAILOUT (unequal == CA_CMP_LOOPS) {
            ca_atmine();
            ca_atwait();            
            FULL_IF_FAILOUT (unequal == CA_CMP_LOOPS) {
                __cmp_and_panic(unequal_function_mask, (uintptr_t)unequal_function, !=);
                __cmp_and_panic(unequal_func_param_mask, (uintptr_t)unequal_func_param, !=);
                if(unequal_function) {
                    unequal_function(unequal_func_param);
                }                
                return CA_FAIL;
            } else {
                ca_panic();
            }
        }

        ca_panic();
    } else {
        ca_panic();
    }
    
    ca_panic();

    return -1;
    
CA_DO_LOOP:
    i = 0;
    while(1)
    {
        volatile ca_uint32_t op1_copy = op1;
        volatile ca_uint32_t op2_copy = op2;
        FULL_IF_FAILOUT ((op1_copy.value == op2_copy.value) & (op1_copy.invvalue == op2_copy.invvalue)) {
            equal++;
        } else {
            unequal++;
        }
        
        ca_fastwait();        
        i++;
        
        ca_landmine();
        __cmp_and_panic(((uint32_t) i) != equal, ((uint32_t) i) != unequal, &&);
        
        FULL_IF_FAILOUT (i == CA_CMP_LOOPS) {
            ca_landmine();
            FULL_IF_FAILOUT (((uint32_t) i) == equal) {
                equal_function_mask ^= (equal << 15);
                equal_func_param_mask ^= (equal << 15);
                goto CA_DO_COMPARE;
            } else FULL_IF_FAILOUT (((uint32_t) i) == unequal) {
                unequal_function_mask ^= (unequal << 15);
                unequal_func_param_mask ^= (unequal << 15);
                goto CA_DO_COMPARE;
            } else {
                ca_panic();
            }
        }
        
        __cmp_and_panic(((uint32_t) i), CA_CMP_LOOPS, >);
        
        ca_landmine();
    }
    
    ca_panic();
    ca_panic();
    ca_panic();
}

//UNFINISHED
ca_return_t ca_compare_func_eq( ca_fptr_voidptr_array_t    get_value_func,
                             void *                     get_value_func_param,
                             uint8_t *                  get_value_func_return,
                             uint8_t *                  expected_value_array,
                             uint32_t                   expected_value_len,
                             ca_fptr_voidptr_t           equal_function,
                             void *                     equal_func_param,
                             ca_fptr_voidptr_t          unequal_function,
                             void *                     unequal_func_param)
{
    uintptr_t equal_function_mask = (uintptr_t)equal_function;
    uintptr_t equal_func_param_mask = (uintptr_t)equal_func_param;
    uintptr_t unequal_function_mask = (uintptr_t)unequal_function;
    uintptr_t unequal_func_param_mask = (uintptr_t)unequal_func_param;

    volatile ca_fptr_voidptr_array_t get_value_func_copy = get_value_func;
    void * volatile get_value_func_param_copy = get_value_func_param;
    uint8_t * volatile get_value_func_return_copy = get_value_func_return;

    volatile uint32_t expected_value_len_copy = expected_value_len;

    ca_landmine();

    FULL_IF_FAILIN (!get_value_func_copy | !get_value_func_param_copy | !get_value_func_return_copy) {
        return CA_BADARG;
    }
    
    get_value_func_copy(get_value_func_param_copy, get_value_func_return_copy);

    //Mask values we'll jump to, make later FI skips increase chance we jump
    //to some invalid value.
    equal_function_mask ^= (CA_CMP_LOOPS << 15);
    equal_func_param_mask ^= (CA_CMP_LOOPS << 15);
    ca_landmine();
    unequal_function_mask ^= (CA_CMP_LOOPS << 15);
    unequal_func_param_mask ^= (CA_CMP_LOOPS << 15);
    
    volatile uint32_t equal = 0;
    volatile uint32_t unequal = 0;
    
    volatile int i = -1000;
    ca_landmine();
    
    goto CA_DO_LOOP;
    
    ca_panic();
    ca_landmine();
    ca_panic();
    ca_landmine();
    ca_panic();
    
CA_DO_COMPARE:
    FULL_IF_FAILOUT (i == CA_CMP_LOOPS) {
        
        ca_atmine();
        ca_atwait();
        
        FULL_IF_FAILOUT (equal == CA_CMP_LOOPS) {
            ca_atmine();
            ca_atwait();
            FULL_IF_FAILOUT (equal == CA_CMP_LOOPS) {
                __cmp_and_panic(equal_function_mask, (uintptr_t)equal_function, !=);
                __cmp_and_panic(equal_func_param_mask, (uintptr_t)equal_func_param, !=);
                if(equal_function) {
                    equal_function(equal_func_param);
                }
                return CA_SUCCESS;
            } else {
                ca_panic();
            }
        }
        
        FULL_IF_FAILOUT (unequal == CA_CMP_LOOPS) {
            ca_atmine();
            ca_atwait();            
            FULL_IF_FAILOUT (unequal == CA_CMP_LOOPS) {
                __cmp_and_panic(unequal_function_mask, (uintptr_t)unequal_function, !=);
                __cmp_and_panic(unequal_func_param_mask, (uintptr_t)unequal_func_param, !=);
                if(unequal_function) {
                    unequal_function(unequal_func_param);
                }                
                return CA_FAIL;
            } else {
                ca_panic();
            }
        }

        ca_panic();
    } else {
        ca_panic();
    }
    
    ca_panic();

    return -1;
    
CA_DO_LOOP:
    i = 0;
    while(1)
    {
        volatile uint32_t j = 0;
        volatile int op_unequal = 0;
        FULL_FOR_FAILOUT (j = 0, j < expected_value_len_copy, j++) {
            volatile int a = expected_value_array[j];

            // Original code was vulnerable to a side-channel timing attack by means of an early break statement
            op_unequal |= (get_value_func_return[j] != a);   
        }
        FULL_IF_FAILIN (!op_unequal) {equal++;}
        else {unequal++;}
        
        ca_fastwait();        
        i++;
        
        ca_landmine();

        __cmp_and_panic(((uint32_t) i) != equal, ((uint32_t) i) != unequal, &&);
        
        FULL_IF_FAILOUT (i == CA_CMP_LOOPS) { 
            ca_landmine();
            FULL_IF_FAILOUT (((uint32_t) i) == equal) {
                equal_function_mask ^= (equal << 15);
                equal_func_param_mask ^= (equal << 15);
                goto CA_DO_COMPARE;
            } else FULL_IF_FAILOUT (((uint32_t) i) == unequal) {
                unequal_function_mask ^= (unequal << 15);
                unequal_func_param_mask ^= (unequal << 15);
                goto CA_DO_COMPARE;
            } else {
                ca_panic();
            }
        }
        
        __cmp_and_panic(((uint32_t) i), CA_CMP_LOOPS, >);
        
        ca_landmine();
    }
    
    ca_panic();
    ca_panic();
    ca_panic();
}

void ca_state_machine(int statenum)
{
    static int ca_stored_state;
    volatile int state_num = statenum;
    
    FULL_IF_FAILIN (state_num == CA_STATE_INIT) {
        ca_stored_state = 0;
        return;
    }
    
    ++ca_stored_state;
    __cmp_and_panic(ca_stored_state, statenum, !=);
    
    return;
}