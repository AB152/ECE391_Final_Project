#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL")

//static int idt_test(void);
//static int test_divzero_exception(void);

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

/* MP3.1: Test cases for exceptions */

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
	
 	int i, j;
	j = 1 - 1; 			// Dodge divide by zero warning
 	i = 1 / j;

 	return FAIL;		// If exception wasn't thrown and we aren't looping, we failed (at least for MP3.1)
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
 	asm volatile("movl %eax, %cr6");

 	return FAIL;		// If exception wasn't thrown and we aren't looping, we failed (at least for MP3.1)	
}


/*
 * test_page_fault (MP3.1)
 *    DESCRIPTION: Test accessing a part of the physical mem that's supposed to be restricted such as first 4MB
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUES: none
 *    SIDE EFFECTS: Should throw page fault as non-present mem shouldn't be accessible
 */
int test_page_fault(){
	TEST_HEADER;
	int * test_ptr = (int *) 0x90000;
	(void)(*test_ptr);
	return FAIL;			// If exception wasn't thrown and we aren't looping, we failed (at least for MP3.1)
}


/*
 * test_no_page_fault (MP3.1)
 *    DESCRIPTION: Test accessing a part of the physical mem that's not restricted such as xb8001
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUES: none
 *    SIDE EFFECTS: Should throw page fault as non-present mem shouldn't be accessible
 */
int test_no_page_fault(){
	TEST_HEADER;
	int * test_ptr = (int *) 0xb8001;
	(void)(*test_ptr);
	return PASS;		// If exception wasn't thrown and we aren't looping, we passed (at least for MP3.1)
}



/* Checkpoint 2 (MP3.2) tests */
/* Checkpoint 3 (MP3.3) tests */
/* Checkpoint 4 (MP3.4) tests */
/* Checkpoint 5 (MP3.5) tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());							// Checks descriptor offset field for NULL
	// launch your tests here
	//TEST_OUTPUT("test_opcode_exception", test_opcode_exception());	// Test opcode exception 
	//TEST_OUTPUT("test_divzero_exception", test_divzero_exception());  // Test divzero
	TEST_OUTPUT("test_no_page_fault", test_no_page_fault());		// Test no page fault
	TEST_OUTPUT("test_page_fault", test_page_fault());				// Test page fault
}
