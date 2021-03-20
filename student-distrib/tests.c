#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 (MP3.1) tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}
	return result;
}

// MP3.1: Test cases for exceptions

/*
 * test_divzero_exception (MP3.1)
 *    DESCRIPTION: Tests divide by 0 exception (INT 0)
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUES: none
 *    SIDE EFFECTS: Should force an infinite loop because our exception handlers just infinite loop
 */
int test_divzero_exception() {
	TEST_HEADER;
	
	int i;
	i = 1 / 0;

	return FAIL;		// Assuming fail as the system shouldn't be able to reach this line (at least for MP3.1)
}


/*
 * test_opcode_exception (MP3.1)
 *    DESCRIPTION: Tests invalid opcode exception (INT 6)
 *    INPUTS: none
 *    OUTPUT: none
 *    RETURN VALUE: none
 * 	  SIDE EFFECTS: Should force an infinite loop because our exception handlers just infinite loop
 */
int test_opcode_exception() {
	TEST_HEADER;

	// The processor should throw invalid opcode as register CR6 is reserved
	asm volatile("mov cr6, eax");

	return FAIL;		// Assuming fail as the system shouldn't be able to reach this line (at least for MP3.1)	
}

/* Checkpoint 2 (MP3.2) tests */
/* Checkpoint 3 (MP3.3) tests */
/* Checkpoint 4 (MP3.4) tests */
/* Checkpoint 5 (MP3.5) tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());							// Checks descriptor offset field for NULL
	// launch your tests here
	TEST_OUTPUT("test_divzero_exception", test_divzero_exception());	// Test DIV0 exception
}
