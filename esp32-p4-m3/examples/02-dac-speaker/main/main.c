/*
this is simplified version (using IDF version 6) of
https://github.com/espressif/esp-adf/blob/af7b72fb2b73d4f513d3cede01c13518ab216735/components/esp_codec_dev/test_apps/codec_dev_test/main/test_board.c#L99
*/
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "esp_codec_dev_os.h"
#include "driver/i2c_master.h"
#include "driver/i2s_std.h"
#include "unity.h"
#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"
#include "esp_log.h"
static const char *TAG = "speaker";

#define I2C_PORT 0
#define MCLK_MULTIPLE 384
#define SAMPLERATE 16000
#define OUTPUT_VOLUME 5.0

static i2c_master_bus_handle_t i2c_bus_handle;
static i2s_chan_handle_t i2s_tx_handle;

static int ut_i2c_init(int16_t sda, int16_t scl)
{
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = scl,
        .sda_io_num = sda,
        .glitch_ignore_cnt = 7,
    };
    return i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle);
}

static int ut_i2c_deinit()
{
    if (i2c_bus_handle) {
        i2c_del_master_bus(i2c_bus_handle);
    }
    i2c_bus_handle = NULL;
    return 0;
}

static int ut_i2s_init(
    int16_t mclk,
    int16_t bclk,
    int16_t ws,
    int16_t ic_din
)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLERATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(16, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = mclk,
            .bclk = bclk,
            .din = I2S_GPIO_UNUSED,
            .ws = ws,
            .dout = ic_din,
        },
    };
    std_cfg.clk_cfg.mclk_multiple = MCLK_MULTIPLE;

    int ret = i2s_new_channel(&chan_cfg,
                              &i2s_tx_handle,
                              NULL);
    TEST_ESP_OK(ret);

    // init tx channel
    ret = i2s_channel_init_std_mode(i2s_tx_handle, &std_cfg);
    TEST_ESP_OK(ret);

    // enable tx channel
    i2s_channel_enable(i2s_tx_handle);
    return ret;
}

static int ut_i2s_deinit()
{
    // disable tx channel
    i2s_channel_disable(i2s_tx_handle);
    i2s_del_channel(i2s_tx_handle);

    return 0;
}

void app_main(void)
{
    // Need install driver (i2c and i2s) firstly
    int ret = ut_i2c_init(GPIO_NUM_7, GPIO_NUM_8);
    TEST_ESP_OK(ret);
    ESP_LOGI(TAG, "I2C Init");

    ret = ut_i2s_init(
        GPIO_NUM_13,
        GPIO_NUM_12, // SCLK in datasheet
        GPIO_NUM_10, // LRCK in datasheet
        GPIO_NUM_9
    );
    TEST_ESP_OK(ret);


    // Do initialize of related interface: data_if, ctrl_if and gpio_if
    audio_codec_i2s_cfg_t i2s_cfg = {
        .tx_handle = i2s_tx_handle,
    };

    const audio_codec_data_if_t *data_if = audio_codec_new_i2s_data(&i2s_cfg);
    TEST_ASSERT_NOT_NULL(data_if);

    audio_codec_i2c_cfg_t i2c_cfg = {
        .port = I2C_PORT,
        .addr = ES8311_CODEC_DEFAULT_ADDR,
        .bus_handle = i2c_bus_handle,
    };

    const audio_codec_ctrl_if_t *out_ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    TEST_ASSERT_NOT_NULL(out_ctrl_if);

    const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();
    TEST_ASSERT_NOT_NULL(gpio_if);

    // New output codec interface
    es8311_codec_cfg_t es8311_cfg = {
        .codec_mode = ESP_CODEC_DEV_WORK_MODE_DAC,
        .ctrl_if = out_ctrl_if,
        .gpio_if = gpio_if,
        .pa_pin = GPIO_NUM_11, //PA_CTRL or amplifier CTRL
        .master_mode = false,
        .use_mclk = true,
        .mclk_div = MCLK_MULTIPLE,
    };
    const audio_codec_if_t *out_codec_if = es8311_codec_new(&es8311_cfg);
    TEST_ASSERT_NOT_NULL(out_codec_if);

    // New output codec device
    esp_codec_dev_cfg_t dev_cfg = {
        .codec_if = out_codec_if,
        .data_if = data_if,
        .dev_type = ESP_CODEC_DEV_TYPE_OUT,
    };
    esp_codec_dev_handle_t play_dev = esp_codec_dev_new(&dev_cfg);
    TEST_ASSERT_NOT_NULL(play_dev);

    ret = esp_codec_dev_set_out_vol(play_dev, OUTPUT_VOLUME);
    TEST_ESP_OK(ret);

    esp_codec_dev_sample_info_t fs = {
        .sample_rate = SAMPLERATE,
        .channel = 1,
        .bits_per_sample = 16,
        .mclk_multiple = MCLK_MULTIPLE,
    };
    ret = esp_codec_dev_open(play_dev, &fs);
    TEST_ESP_OK(ret);
    ESP_LOGI(TAG, "sample_info Init");

    // play a sine wave for few seconds
    ESP_LOGI(TAG, "Creating sine wave to play");

    // write to play device
    double audio_phase;
    float frequency = 440.0f;
    double two_pi = 2.0 * M_PI;
    double phase_increment = 0;

    // store one second of data, so calculate the number of samples
    uint32_t sinewave_buffer_samples = fs.sample_rate * fs.channel;
    // the 2 is because int16_t is 2 bytes of length
    uint32_t sinewave_buffer_length = sinewave_buffer_samples * 2;
    int16_t *sinewave_buffer = (int16_t *)malloc(sinewave_buffer_length);
    if (sinewave_buffer == NULL) {
        ESP_LOGI(TAG, "Failed to allocate: %u", sinewave_buffer_length);
    }
    TEST_ASSERT_NOT_NULL(sinewave_buffer);

    // play 10 seconds of audio
    for (int j=0; j<10; j++) {
        audio_phase = 0.0;
        phase_increment = two_pi / fs.sample_rate;

        // generate sine wave
        for (int i = 0; i < sinewave_buffer_samples; i++) {
            // Calculate the sine sample value
            sinewave_buffer[i] = (int16_t)(32767 * sin(audio_phase * frequency));

            // Advance and wrap the phase to prevent precision loss over time
            audio_phase += phase_increment;
            if (audio_phase >= two_pi) {
                audio_phase -= two_pi;
            }
        }

        frequency += 300;
        ESP_LOGI(TAG, "[%d] created one second", j);

        // play to device
        ret = esp_codec_dev_write(play_dev, sinewave_buffer, sinewave_buffer_length);
        TEST_ESP_OK(ret);
        ESP_LOGI(TAG, "[%d] played one second", j);
    }

    ret = esp_codec_dev_close(play_dev);
    TEST_ESP_OK(ret);
    esp_codec_dev_delete(play_dev);

    // Delete codec interface
    audio_codec_delete_codec_if(out_codec_if);
    // Delete codec control interface
    audio_codec_delete_ctrl_if(out_ctrl_if);
    audio_codec_delete_gpio_if(gpio_if);
    // Delete codec data interface
    audio_codec_delete_data_if(data_if);

    // clean
    ut_i2c_deinit();
    ut_i2s_deinit();
}