.section .text
.global _start
.type _start, @function

# this will use the halt to event and a threshold when FIFO0 is level equal to 4
# then it will add the values and set to FIFO1
# the setup of the threshold is going to be handled in the host side
# so the FIFO0 Treshold0 or event[24] is going to happen
_start:
    li t0, 0x1000000 # mask for event[24]
wait_for_event:
    mv t1, x30      # halt for event
    beq t0, t1, do_sum
    j wait_for_event
do_sum:
    # load the four values from FIFO0
    # add without a loop
    li t2, 0 # sum
    mv t1, x16 # load first value from FIFO0
    add t2, t2, t1 # sum
    mv t1, x16 # load second value from FIFO0
    add t2, t2, t1 # sum
    mv t1, x16 # load third value from FIFO0
    add t2, t2, t1 # sum
    mv t1, x16 # load fourth value from FIFO0
    add t2, t2, t1 # sum
    mv x17, t2 # save the sum in the FIFO1
    j wait_for_event
