
#include "bao.h"
#include "hardware/bio.h"

#define BIO_CORE 0x1
static const uint32_t bio_program_length = 5;
static const uint32_t bio_program[] = {
    0x00200313,
    0x00080293,
    0x026288b3,
    0x00028913,
    0xff5ff06f
};


int main(void)
{
    bao_init();

    mini_printf("\r\nBIO Multiplication with signal\r\n");

    bio_init(FCLK_HZ);
    bio_load_code_words(0, bio_program, bio_program_length);
    bio_start_cores(BIO_CORE);

    /* Main CPU is free */
    uint32_t multiplicand = 1;
    uint32_t result;
    while(1) {
        // put into fifo0
        bio_push_fifo0(multiplicand); // sdk
        // BIO_SFR_TXF0 = multiplicand; // direct register

        // wait until is multiplied and fifo2 is signaled
        while(bio_pop_fifo2() != multiplicand);
        //while (BIO_SFR_RXF1 != multiplicand); // direct register

        result = bio_pop_fifo1(); // sdk
        // result = BIO_SFR_RXF1; // direct register

        mini_printf("%u * 2 = %u\r\n", multiplicand, result);

        if (multiplicand == 0xFFFFFFFF) {
            multiplicand = 1;
        } else {
            multiplicand++;
        }
    }
}
