//=================================================================
// Copyright 2026 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//=================================================================


#include "globals.h"

static const char *TAG = "example";
int alarm_count = 10000;

// TODO: Implement this function.
// Read all hardware inputs and populate the GameInputs struct.
// Hint: the buttons are active low
// Use the helper functions in Buttons.c and Nav_Switch.c
void read_inputs(GameInputs* inputs, Buttons* buttons, Nav_Switch* myNav)
{
    inputs->b0 = pb0(buttons); // TODO
    inputs->b1 = pb1(buttons);
    inputs->b2 = pb2(buttons);
    inputs->b3 = pb3(buttons);

    inputs->up = up(myNav); // TODO
    inputs->down = down(myNav);
    inputs->left = left(myNav);
    inputs->right = right(myNav);
    inputs->center = center(myNav);

    return;
}

led_strip_handle_t configure_led(gpio_num_t led_gpio_pin)
{
    gpio_reset_pin(led_gpio_pin); // Data pin for LED strip
    gpio_set_direction(led_gpio_pin, GPIO_MODE_OUTPUT);

    // LED strip general initialization, for the single LED (trivial "strip") on the ESP32-C6-DevKit board
    led_strip_config_t strip_config = {
        .strip_gpio_num = led_gpio_pin, // The GPIO that connected to the LED strip's data line
        .max_leds = 1,      // The number of LEDs in the strip,
        .led_model = LED_MODEL_WS2812,        // LED model
        // set the color order of the LED: GRB
        .color_component_format = {
            .format = {
                .r_pos = 1, // red is the second byte in the color data
                .g_pos = 0, // green is the first byte in the color data
                .b_pos = 2, // blue is the third byte in the color data
                .num_components = 3, // total 3 color components
            },
        },
        .flags = {
            .invert_out = false, // don't invert the output signal
        }
    };

    // LED strip backend configuration: SPI
    led_strip_spi_config_t spi_config = {
        .clk_src = SPI_CLK_SRC_DEFAULT, // different clock source can lead to different power consumption
        .spi_bus = SPI2_HOST,           // SPI bus ID
        .flags = {
            .with_dma = true, // Using DMA can improve performance and help drive more LEDs
        }
    };

    // LED Strip object handle
    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with SPI backend");
    return led_strip;
}


static gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT, // Select the default clock source
    .direction = GPTIMER_COUNT_UP,      // Counting direction is up
    .resolution_hz = 1 * 1000 * 1000,   // Resolution is 1 MHz, i.e., 1 tick equals 1 microsecond
};

static bool example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    // General process for handling event callbacks:
    // 1. Retrieve user context data from user_ctx (passed in from gptimer_register_event_callbacks)
    // 2. Get alarm event data from edata, such as edata->count_value
    // 3. Perform user-defined operations
    // 4. Return whether a high-priority task was awakened during the above operations to notify the scheduler to switch tasks
    static bool level = false;
    gptimer_set_raw_count(timer, MAX_TIMER_COUNT-alarm_count); // Reset the timer count to current freq.
    gpio_set_level(SPEAKER_GPIO_PIN, level);
    level = !level;
    return false;
}

static gptimer_alarm_config_t alarm_config = {
    .reload_count = 0,      // When the alarm event occurs, the timer will automatically reload to 0
    .alarm_count = MAX_TIMER_COUNT, // Set the actual alarm rate
    .flags.auto_reload_on_alarm = true, // Enable auto-reload function
};

static gptimer_event_callbacks_t cbs = {
    .on_alarm = example_timer_on_alarm_cb, // Call the user callback function when the alarm event occurs
};


// NOTE: If you want to drive the speaker from a dedicated background task rather than
// the timer interrupt alone, you can launch a FreeRTOS task like this:
//
//   void speaker_task(void* pvParameters) {
//       sound* s = (sound*) pvParameters;
//       while (1) {
//           // update tone, handle duration, etc.
//           vTaskDelay(pdMS_TO_TICKS(10));
//       }
//   }
//
//   // In app_main (or configure_sound), after setup:
//   xTaskCreate(speaker_task, "speaker_task", 2048, &speaker, 5, NULL);
//   // Parameters: task function, name, stack size (bytes), parameter, priority, handle (NULL = ignore)

int configure_sound(sound* my_sound, gptimer_handle_t* gptimer)
{
    my_sound->speaker_gpio = SPEAKER_GPIO_PIN;
    my_sound->resolution =  1000000;  // Frequency resolution in Hz (1 MHz, i.e., 1 tick equals 1 microsecond)
    my_sound->frequency = 20;         // Frequency of current tone (square wave), Hz
    my_sound->volume = 50;            // Default volume level (not implemented)
    my_sound->duration_ms = 0;        // Default duration (continuous)
    my_sound->enabled = 0;            // Default enabled flag (disabled)
    gpio_reset_pin(my_sound->speaker_gpio); // Pin for speaker output
    gpio_set_direction(my_sound->speaker_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(my_sound->speaker_gpio, 1);

    // Create a timer instance
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, gptimer));

    // Set the timer's alarm action
    ESP_ERROR_CHECK(gptimer_set_alarm_action(*gptimer, &alarm_config));

    // Register timer event callback functions, allowing user context to be carried
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(*gptimer, &cbs, NULL));

    return 0;
}

int sound_on(sound* my_sound, gptimer_handle_t* gptimer) {
    // enable timer (and its interrupt) to play sound
    ESP_ERROR_CHECK(gptimer_enable(*gptimer));
    // Start the timer
    ESP_ERROR_CHECK(gptimer_start(*gptimer));
    my_sound->enabled = 1;
    return 0;
}

int tone(sound* my_sound, gptimer_handle_t* gptimer_clock_source_t, int frequency ) {
    my_sound->frequency = frequency;
    alarm_count = (timer_config.resolution_hz/frequency)/2;
    return 0;
}
