.section .text
.global _start
.type _start, @function

# very simple program, just wait for a number and multiply by 2
# it will only show the lower bits of multiplication
# uses three fifos
# FIFO0 (x16) for input
# FIFO1 (x17) for output
# FIFO2 (x18) for signal (set to the multiplier once finished)

_start:
    li t1, 2        # load multiplier
cycle:
    mv t0, x16      # wait for FIFO0
    mul x17, t0, t1 # multiply and store back in FIFO1
    mv x18, t0      # signal that multiply has finished in FIFO2
    j cycle
