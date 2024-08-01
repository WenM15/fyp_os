#ifndef RISCV_REGISTER_H
#define RISCV_REGISTER_H

#include "typedef.h"
// thread pointer (tp) register, typically points to a thread local storage (TLS)
// but it is used to hold the core's hartid only in this implementation
static inline uint64 r_tp()
{
	uint64 x;
	asm volatile("addi %0, tp, 0" : "=r" (x) );
	return x;
}

static inline void w_tp(uint64 x)
{
	asm volatile("addi tp, %0, 0" : : "r" (x) );
}

// <<<<<--------[ Machine mode ]-------->>>>>

// read the hart (core)'s id
static inline uint64 r_mhartid()
{
	uint64 x;
	asm volatile("csrr %0, mhartid" : "=r" (x) );
	return x;
}

// mstatus (Machine Status) register

// MPP (Machine Previous Privilege) bit field (2-bit)
// possible values (in binary) are:
// 00: User mode
// 01: Supervisor mode
// 10: Reserved
// 11: Machine mode
#define MSTATUS_MPP_MASK (3L << 11) // mask for MPP bit field, used for resetting it
#define MSTATUS_MPP_M (3L << 11)
#define MSTATUS_MPP_S (1L << 11)
#define MSTATUS_MPP_U (0L << 11)

// MIE (Machine Interrupt Enable) bit field (1-bit)
#define MSTATUS_MIE (1L << 3)

// read from mstatus register
static inline uint64 r_mstatus()
{
	uint64 x;
	asm volatile("csrr %0, mstatus" : "=r" (x) );
	return x;
}

// write to mstatus register
static inline void w_mstatus(uint64 x)
{
	asm volatile("csrw mstatus, %0" : : "r" (x) );
}

// mie (Machine interrupt-enable) register
// enable or disable External, Timer, or Software interrupts for Machine mode
#define MIE_MEIE (1L << 11) // External
#define MIE_MTIE (1L << 7) // Timer
#define MIE_MSIE (1L << 3) // Software

static inline uint64 r_mie()
{
	uint64 x;
	asm volatile("csrr %0, mie" : "=r" (x) );
	return x;
}

static inline void w_mie(uint64 x)
{
	asm volatile("csrw mie, %0" : : "r" (x) );
}

// mepc (Machine Exception Program Counter) register
// holds the instruction address which a return from a trap will go
static inline void w_mepc(uint64 x)
{
	asm volatile("csrw mepc, %0" : : "r" (x) );
}

// medeleg (Machine Exception Delegation) register
// delegate exception (represented by the position of bits) to Supervisor mode
static inline uint64 r_medeleg()
{
	uint64 x;
	asm volatile("csrr %0, medeleg" : "=r" (x) );
	return x;
}

static inline void w_medeleg(uint64 x)
{
	asm volatile("csrw medeleg, %0" : : "r" (x) );
}

// mideleg (Machine Interrupt Delegation) register
// delegate interrupt (represented by the position of bits) to Supervisor mode
static inline uint64 r_mideleg()
{
	uint64 x;
	asm volatile("csrr %0, mideleg" : "=r" (x) );
	return x;
}

static inline void w_mideleg(uint64 x)
{
	asm volatile("csrw mideleg, %0" : : "r" (x) );
}

// Physical Memory Protection
static inline void w_pmpcfg0(uint64 x)
{
	asm volatile("csrw pmpcfg0, %0" : : "r" (x) );
}

static inline void w_pmpaddr0(uint64 x)
{
	asm volatile("csrw pmpaddr0, %0" : : "r" (x) );
}

// mtvec (Machine Trap Vector) register
// holds the address of trap handler
static inline void w_mtvec(uint64 x)
{
	asm volatile("csrw mtvec, %0" : : "r" (x) );
}

// mscratch (Machine Scratch) register
// holds the address of hart-local space for trap handler's information
// in this implementation, it is used for the timer interrupt handler
static inline void w_mscratch(uint64 x)
{
	asm volatile("csrw mscratch, %0" : : "r" (x) );
}


// <<<<<--------[ Supervisor mode]-------->>>>>
#define SSTATUS_SIE (1L << 1) // Supervisor Interrupt Enable

static inline uint64 r_sstatus()
{
	uint64 x;
	asm volatile("csrr %0, sstatus" : "=r" (x) );
	return x;
}

static inline void w_sstatus(uint64 x)
{
	asm volatile("csrw sstatus, %0" : : "r" (x) );
}

// device interrupts
//
// get the state of whether interrupts are enabled or not
static inline uint8 intr_get()
{
	uint8 x = r_sstatus();
	return (x & SSTATUS_SIE) != 0;
}

// enable device interrupts
static inline void intr_on()
{
	w_sstatus(r_sstatus() | SSTATUS_SIE);
}

// disable device interrupts
static inline void intr_off()
{
	w_sstatus(r_sstatus() | ~SSTATUS_SIE);
}

// satp (Supervisor Address Translation and Protection) register
// holds the address of the root page table entry
static inline uint64 r_satp()
{
	uint64 x;
	asm volatile("csrr %0, satp" : "=r" (x) );
	return x;
}

static inline void w_satp(uint64 x)
{
	asm volatile("csrw satp, %0" : : "r" (x) );
}

// sie (Supervisor Interrupt Enable) register
// enable or disable External, Timer, or Software interrupts for Supervisor mode

#define SIE_SEIE (1L << 9) // External
#define SIE_STIE (1L << 5) // Timer
#define SIE_SSIE (1L << 1) // Software

static inline uint64 r_sie()
{
	uint64 x;
	asm volatile("csrr %0, sie" : "=r" (x) );
	return x;
}

static inline void w_sie(uint64 x)
{
	asm volatile("csrw sie, %0" : : "r" (x) );
}

// <<<<<--------[ Locks ]-------->>>>>

// swaps the lock's value with the value provided
// returns the lock's old value
// this swap is an atomic operation,
// which means that the lock's value can be accessed by
// one CPU at a time
inline uint32 atomic_swap(uint32 *p_lock, uint32 value)
{
	asm volatile("amoswap.w %0, %0, (%1)"
			: "+r" (value)
			: "r" (p_lock)
			: "memory");
	return value;
}

#endif
