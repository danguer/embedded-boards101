
#include "bao.h"
#include "hardware/bio.h"

#define BIO_CORE 0x1
static const uint32_t bio_program_length = 15;
static const uint32_t bio_program[] = {
    0x010002b7,
    0x000f0313,
    0x00628463,
    0xff9ff06f,
    0x00000393,
    0x00080313,
    0x006383b3,
    0x00080313,
    0x006383b3,
    0x00080313,
    0x006383b3,
    0x00080313,
    0x006383b3,
    0x00038893,
    0xfcdff06f
};


int main(void)
{
    bao_init();

    mini_printf("\r\nBIO Adding with events\r\n");

    bio_init(FCLK_HZ);
    bio_load_code_words(0, bio_program, bio_program_length);
    bio_start_cores(BIO_CORE);

    // register event threshold for FIFO0,Threshold0 using == for event generation
    // there are 8 thresholds, 2 per fifo
    // [7:0]	FIFO_EVENT_LT_MASK	fifo_event_lt_mask read/write control register
    // [15:8]	FIFO_EVENT_EQ_MASK	fifo_event_eq_mask read/write control register
    // [23:16]	FIFO_EVENT_GT_MASK
    // so for this needs the bit 8 = 1 to enable FIFO0,Threshold0 == mask
    // and trigger Event24
    BIO_SFR_ETYPE = 0x100;

    /* Main CPU is free */
    uint32_t base = 0;
    while(1) {
        // put into fifo0
        bio_push_fifo0(base); // sdk
        bio_push_fifo0(base+1); // sdk
        bio_push_fifo0(base+2); // sdk
        bio_push_fifo0(base+3); // sdk

        // wait for result
        while(bio_fifo_empty(1));
        mini_printf(
            "%d + %d + %d + %d = %d\r\n",
            base,
            base+1,
            base+2,
            base+3,
            bio_pop_fifo1()
        );

        // add base for next iteration
        if (base < 0xFFFFFFFC) {
            base += 4;
        } else {
            base = 0;
        }
    }
}
