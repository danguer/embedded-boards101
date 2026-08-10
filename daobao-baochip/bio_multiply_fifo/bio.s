.section .text
.global _start
.type _start, @function

# very simple program, just wait for a number and multiply by 2
# it will only show the lower bits of multiplication
# uses a single FIFO for input / output

_start:
    mv t0, x16      # wait for FIFO0
    li t1, 2        # load multiplier
    mul x16, t0, t1 # multiply and store back in fifo
