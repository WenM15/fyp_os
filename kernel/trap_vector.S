#
# Machine mode timer interrupt handler
#

.global timer_vector
.align 4
timer_vector:
	
	# main.c has set up a memory for timer information (timer_scratch_storage)
	# mscratch points to a hart-local space in this storage
	
	# scratch[0,1,2] : register save area
	# scratch[3] : address of CLINT'S MTIMECMP register
	# scratch[4] : interval between timer interrupt
	
	# scratch is a uint64 pointer
	# therefore, the space between, e.g. scratch[0] and scratch[1] is 8 bytes (64 bits)
 
	# swap mscratch and a0 register value, basically saving a0 register state, and loading mscratch for subsequent operations
	csrr t0, mscratch # load mscratch value to a temporary register
	csrw mscratch, a0 # now mscratch has backup, write a0 to mscratch
	addi a0, t0, 0 # write backup value to a0

	# save register value
	sd a1, 0(a0)
	sd a2, 8(a0)
	sd a3, 16(a0)

	# schedule next timer interrupt
	# by adding interval to mtimecmp
	ld a1, 24(a0) # MTIMECMP register's address (unique to each hart)
	ld a2, 32(a0) # interval
	ld a3, 0(a1) # MTIMECMP register's value
	add a3, a3, a2 # add MTIMECMP register's value with interval
	sd a3, 0(a1) # store the new value to MTIMECMP register

	# arrange for a supervisor software interrupt
	# after this handler calls mret
	li a1, 2 # set sip's SSIP bit field
	csrw sip, a1

	# restore register value
	ld a3, 16(a0)
	ld a2, 8(a0)
	ld a1, 0(a0)
	
	addi t0, a0, 0 # load a0 value to a temporary register
	csrr a0, mscratch # now a0 has backup, load mscratch to a0
	csrw mscratch, t0 # write backup value to mscratch

	mret
