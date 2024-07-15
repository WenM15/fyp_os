#include "typedef.h"
#include "riscv_register.h"
#include "qemu_memlayout.h"
#define MAX_CPU 8

extern void func_init();
extern void timer_vector();

static void timer_init();

uint64 timer_scratch_storage[MAX_CPU][5];

void main()
{	
	// keep each CPU's hart id in its tp register
	int hart_id = r_mhartid();
	w_tp(hart_id);

	// CPU starts in Machine mode
	// mret will transition to the mode specified in the M Previous Privilege (MPP) bit field in mstatus register
	// set MPP to Supervisor so that CPU can transition to it
	uint64 mstatus = r_mstatus();
	mstatus &= ~MSTATUS_MPP_MASK;
	mstatus |= MSTATUS_MPP_S;
	w_mstatus(mstatus);

	// mret will jump to the code where its address is specified in M Exception Program Counter (MEPC) register
	// set MEPC to main
	// requires gcc -mcmodel=medany
	w_mepc((uint64)func_init);

	// disable paging
	w_satp(0);

	// by default, all interrupts and exceptions are handled in Machine mode
	// delegate all interrupts and exceptions to Supervisor mode
	w_medeleg(0xffff);
	w_mideleg(0xffff);
	w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

	// configure Physical Memory Protection (PMP) to give Supervisor mode
	// access to all physical memory
	
	// 0x3fffffffffffffull is a 64-bit value with the lower 54 bits set to 1
	// writes to PMP's address registers are automatically left-shifted by 2 bits
	// after the shift, the value becomes 0xFFFFFFFFFFFFFFC
	// the address range written covers up to the maximum physical memory addressable in the RISC-V architecture, minus the alignment
	w_pmpaddr0(0x3fffffffffffffull);
	w_pmpcfg0(0xf);

	// ask for clock interrupts from CLINT (Core Local Interruptor)
	timer_init();


	// switch to Supervisor mode
	asm volatile("mret");
}

static void timer_init()
{
	// write timer interrupt handler's address to Machine mode's trap vector
	w_mtvec((uint64)timer_vector);

	// one million CPU cycles for every timer interrupt interval
	int interval_cycles = 1000000;
	
	int hart_id = r_tp();

	// there are N mtimecmp (64-bit) register for N hart and 1 mtime (64-bit) register in CLINT
	// mtimecmp holds the value that the timer (mtime) will be compared against to trigger an interrupt
	// i.e. when the value of mtimecmp = mtime, a timer interrupt will be generated
	*(uint64*)CLINT_MTIMECMP(hart_id) = *(uint64*)CLINT_MTIME + interval_cycles;
	
	// prepare information for timer interrupt handler
	// scratch[0 to 2] : space for timer_vector to save registers
	// scratch[3] : address of MTIMECMP register
	// scratch[4] : interval (in cycles) between timer interrupts 
	uint64 *scratch = &timer_scratch_storage[hart_id][0];
	scratch[3] = CLINT_MTIMECMP(hart_id);
	scratch[4] = interval_cycles;
	w_mscratch((uint64)scratch);

	// enable Machine mode interrupts
	w_mstatus(r_mstatus() | MSTATUS_MIE);

	// enable Machine mode timer interrupts
	w_mie(r_mie() | MIE_MTIE);
}
