#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wasm_export.h"
#include "bh_platform.h"
#include "test_wasm.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define LOG_TAG "wamr"
#define LED_GPIO 8  // On-board RGB LED is connected to GPIO 8

static void wasm_gpio_set_level(wasm_exec_env_t exec_env, int32_t pin, int32_t level) {
    gpio_set_level((gpio_num_t)pin, level);
}

static void wasm_sleep_ms(wasm_exec_env_t exec_env, int32_t milliseconds) {
    vTaskDelay(milliseconds / portTICK_PERIOD_MS);
}

static void wasm_print_debug(wasm_exec_env_t exec_env, const char *message) {
    ESP_LOGI("WASM", "WASM Debug: %s", message);
}

static void *app_instance_main(wasm_module_inst_t module_inst) {
    const char *exception;
    wasm_application_execute_main(module_inst, 0, NULL);
    if ((exception = wasm_runtime_get_exception(module_inst)))
        ESP_LOGE(LOG_TAG, "WASM Exception: %s", exception);
    return NULL;
}

void *iwasm_main(void *arg) {
    (void)arg;
    uint8_t *wasm_file_buf = (uint8_t *)wasm_test_file_interp;
    unsigned wasm_file_buf_size = sizeof(wasm_test_file_interp);
    wasm_module_t wasm_module = NULL;
    wasm_module_inst_t wasm_module_inst = NULL;
    char error_buf[128];
    void *ret;
    RuntimeInitArgs init_args;

    memset(&init_args, 0, sizeof(RuntimeInitArgs));
    init_args.mem_alloc_type = Alloc_With_Allocator;
    init_args.mem_alloc_option.allocator.malloc_func = (void *)os_malloc;
    init_args.mem_alloc_option.allocator.realloc_func = (void *)os_realloc;
    init_args.mem_alloc_option.allocator.free_func = (void *)os_free;

    ESP_LOGI(LOG_TAG, "Initialize WASM runtime");
    if (!wasm_runtime_full_init(&init_args)) {
        ESP_LOGE(LOG_TAG, "Init runtime failed.");
        return NULL;
    }

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    ESP_LOGI(LOG_TAG, "Registering native functions");
    NativeSymbol native_symbols[] = {
        {"gpio_set_level", (void*)wasm_gpio_set_level, "(ii)", NULL},
        {"sleep_ms", (void*)wasm_sleep_ms, "(i)", NULL},
        {"print_debug", (void*)wasm_print_debug, "($)", NULL}
    };
    wasm_runtime_register_natives("env", native_symbols, sizeof(native_symbols) / sizeof(NativeSymbol));

    ESP_LOGI(LOG_TAG, "Run wamr with interpreter");
    if (!(wasm_module = wasm_runtime_load(wasm_file_buf, wasm_file_buf_size, error_buf, sizeof(error_buf)))) {
        ESP_LOGE(LOG_TAG, "Error in wasm_runtime_load: %s", error_buf);
        goto fail1interp;
    }

    ESP_LOGI(LOG_TAG, "Instantiate WASM runtime");
    if (!(wasm_module_inst = wasm_runtime_instantiate(wasm_module, 32 * 1024, 32 * 1024, error_buf, sizeof(error_buf)))) {
        ESP_LOGE(LOG_TAG, "Error while instantiating: %s", error_buf);
        goto fail2interp;
    }

    ESP_LOGI(LOG_TAG, "Run main() of the application");
    ret = app_instance_main(wasm_module_inst);
    assert(!ret);

    ESP_LOGI(LOG_TAG, "Deinstantiate WASM runtime");
    wasm_runtime_deinstantiate(wasm_module_inst);

fail2interp:
    ESP_LOGI(LOG_TAG, "Unload WASM module");
    wasm_runtime_unload(wasm_module);

fail1interp:
    ESP_LOGI(LOG_TAG, "Destroy WASM runtime");
    wasm_runtime_destroy();

    return NULL;
}

void app_main(void) {
    pthread_t t;
    int res;

    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setstacksize(&tattr, 4096);

    res = pthread_create(&t, &tattr, iwasm_main, (void *)NULL);
    assert(res == 0);

    res = pthread_join(t, NULL);
    assert(res == 0);

    ESP_LOGI(LOG_TAG, "Exiting...");
}
