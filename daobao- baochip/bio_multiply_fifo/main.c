
#include "bao.h"
#include "hardware/bio.h"

/*
 * BIO program: toggles BIO pin 2 (PB2) with quantum timing.
 *
 *   addi  x5, x0, 4       ; bit mask for pin 2
 *   addi  x26, x5, 0      ; GPIO mask = pin 2
 *   addi  x24, x5, 0      ; pin 2 = output
 * loop:
 *   addi  x22, x5, 0      ; set pin 2 HIGH
 *   addi  x20, x0, 0      ; wait quantum
 *   addi  x23, x0, 0      ; clear pin 2 LOW
 *   addi  x20, x0, 0      ; wait quantum
 *   jal   x0, -16          ; loop
 */
#define BIO_CORE 0x1
static const uint32_t bio_program_length = 3;
static const uint32_t bio_program[] = {
    0x00080293,
    0x00200313,
    0x02628833
};

int main(void)
{
    bao_init();

    mini_printf("\r\nBIO Multiplication\r\n");

    bio_init(FCLK_HZ);
    bio_load_code_words(0, bio_program, bio_program_length);

    /* Main CPU is free */
    uint32_t multiplicand = 1;
    uint32_t result;
    while(1) {
        // put into fifo0
        bio_event_clear(0xFFFFFFFF); // sdk
        // BIO_SFR_EVENT_CLR = 0xFFFFFFFF; // direct register
        bio_start_cores(BIO_CORE);
        bio_push_fifo0(multiplicand); // sdk
        // BIO_SFR_TXF0 = multiplicand; // direct register
        // wait until is multiplied
        do {
            result = bio_pop_fifo0(); // sdk
            // result = BIO_SFR_RXF0; // direct register
        } while(result == multiplicand);
        bio_stop_cores(BIO_CORE);
        mini_printf("%u * 2 = %u\r\n", multiplicand, result);
        if (multiplicand == 0xFFFFFFFF) {
            multiplicand = 1;
        } else {
            multiplicand++;
        }
    }
}
