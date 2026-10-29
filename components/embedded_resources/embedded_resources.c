/**
 * @file embedded_resources.c
 * @brief Auto-deploys IR database files from firmware to SD card on boot.
 */

#include "embedded_resources.h"
#include <storage/storage.h>
#include <furi.h>
#include <esp_log.h>
#include <string.h>

static const char* TAG = "EmbeddedRes";

extern const uint8_t tv_ir_start[]         asm("_binary_tv_ir_start");
extern const uint8_t tv_ir_end[]           asm("_binary_tv_ir_end");
extern const uint8_t ac_ir_start[]         asm("_binary_ac_ir_start");
extern const uint8_t ac_ir_end[]           asm("_binary_ac_ir_end");
extern const uint8_t audio_ir_start[]      asm("_binary_audio_ir_start");
extern const uint8_t audio_ir_end[]        asm("_binary_audio_ir_end");
extern const uint8_t fans_ir_start[]       asm("_binary_fans_ir_start");
extern const uint8_t fans_ir_end[]         asm("_binary_fans_ir_end");
extern const uint8_t leds_ir_start[]       asm("_binary_leds_ir_start");
extern const uint8_t leds_ir_end[]         asm("_binary_leds_ir_end");
extern const uint8_t projectors_ir_start[] asm("_binary_projectors_ir_start");
extern const uint8_t projectors_ir_end[]   asm("_binary_projectors_ir_end");

typedef struct {
    const char* target_path;
    const uint8_t* data_start;
    const uint8_t* data_end;
} EmbeddedResource;

static const EmbeddedResource resources[] = {
    {"/ext/infrared/assets/tv.ir",         tv_ir_start,         tv_ir_end},
    {"/ext/infrared/assets/ac.ir",         ac_ir_start,         ac_ir_end},
    {"/ext/infrared/assets/audio.ir",      audio_ir_start,      audio_ir_end},
    {"/ext/infrared/assets/fans.ir",       fans_ir_start,       fans_ir_end},
    {"/ext/infrared/assets/leds.ir",       leds_ir_start,       leds_ir_end},
    {"/ext/infrared/assets/projectors.ir", projectors_ir_start, projectors_ir_end},
    {"/ext/infrared/tv.ir",                tv_ir_start,         tv_ir_end},
    {"/ext/infrared/ac.ir",                ac_ir_start,         ac_ir_end},
    {"/ext/infrared/audio.ir",             audio_ir_start,      audio_ir_end},
    {"/ext/infrared/fans.ir",              fans_ir_start,       fans_ir_end},
    {"/ext/infrared/leds.ir",              leds_ir_start,       leds_ir_end},
    {"/ext/infrared/projectors.ir",        projectors_ir_start, projectors_ir_end},
};

static void ensure_parent_dirs(Storage* storage, const char* path) {
    FuriString* dir = furi_string_alloc();
    const char* p = path;
    if(*p == '/') p++;
    while(*p) {
        const char* slash = strchr(p, '/');
        if(!slash) break;
        furi_string_set_strn(dir, path, (size_t)(slash - path));
        const char* dir_cstr = furi_string_get_cstr(dir);
        if(!storage_dir_exists(storage, dir_cstr)) {
            FS_Error err = storage_common_mkdir(storage, dir_cstr);
            if(err != FSE_OK && err != FSE_EXIST) {
                ESP_LOGW(TAG, "mkdir '%s' err=%d", dir_cstr, err);
            }
        }
        p = slash + 1;
    }
    furi_string_free(dir);
}

static bool file_exists_with_correct_size(Storage* storage, const char* path, size_t expected_size) {
    FileInfo fileinfo;
    FS_Error err = storage_common_stat(storage, path, &fileinfo);
    if(err != FSE_OK) return false;
    if(file_info_is_dir(&fileinfo)) return false;
    if((size_t)fileinfo.size != expected_size) {
        ESP_LOGW(TAG, "'%s' size mismatch: on_sd=%u expected=%u",
                 path, (unsigned)fileinfo.size, (unsigned)expected_size);
        return false;
    }
    return true;
}

static bool deploy_single_resource(Storage* storage, const EmbeddedResource* res) {
    const size_t total = (size_t)(res->data_end - res->data_start);
    if(file_exists_with_correct_size(storage, res->target_path, total)) {
        ESP_LOGI(TAG, "OK (exists): %s (%u bytes)", res->target_path, (unsigned)total);
        return true;
    }
    ESP_LOGI(TAG, "Deploying %s (%u bytes)", res->target_path, (unsigned)total);
    ensure_parent_dirs(storage, res->target_path);
    storage_common_remove(storage, res->target_path);

    File* file = storage_file_alloc(storage);
    bool success = false;
    if(storage_file_open(file, res->target_path, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        size_t written = 0;
        const size_t chunk = 4096;
        while(written < total) {
            size_t to_write = total - written;
            if(to_write > chunk) to_write = chunk;
            size_t result = storage_file_write(file, res->data_start + written, to_write);
            if(result != to_write) {
                ESP_LOGE(TAG, "Write error at offset %u for %s", (unsigned)written, res->target_path);
                break;
            }
            written += result;
        }
        success = (written == total);
        if(success) ESP_LOGI(TAG, "OK: %s (%u bytes written)", res->target_path, (unsigned)total);
        else ESP_LOGE(TAG, "FAIL: %s (%u of %u)", res->target_path, (unsigned)written, (unsigned)total);
    } else {
        ESP_LOGE(TAG, "Cannot open '%s' for writing (err=%d)", res->target_path, storage_file_get_error(file));
    }
    storage_file_close(file);
    storage_file_free(file);
    return success;
}

void embedded_resources_deploy(void) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    const int max_wait_cycles = 30;
    bool sd_ready = false;
    for(int i = 0; i < max_wait_cycles; i++) {
        FS_Error sd_status = storage_sd_status(storage);
        if(sd_status == FSE_OK) { sd_ready = true; break; }
        furi_delay_ms(500);
    }
    if(!sd_ready) {
        ESP_LOGE(TAG, "SD card not ready, aborting deployment");
        furi_record_close(RECORD_STORAGE);
        return;
    }
    const size_t count = sizeof(resources) / sizeof(resources[0]);
    size_t deployed = 0;
    ESP_LOGI(TAG, "Starting resource deployment: %u files", (unsigned)count);
    for(size_t i = 0; i < count; i++) {
        if(deploy_single_resource(storage, &resources[i])) deployed++;
    }
    ESP_LOGI(TAG, "Resource deployment complete: %u/%u OK", (unsigned)deployed, (unsigned)count);
    furi_record_close(RECORD_STORAGE);
}

static void embedded_resources_state_cb(FuriThread* thread, FuriThreadState state, void* context) {
    UNUSED(context);
    if(state == FuriThreadStateStopped) furi_thread_free(thread);
}

static int32_t embedded_resources_deploy_thread(void* context) {
    UNUSED(context);
    furi_delay_ms(2000);
    embedded_resources_deploy();
    return 0;
}

void embedded_resources_start_async(void) {
    FuriThread* thread = furi_thread_alloc_ex("ResDeploy", 8192, embedded_resources_deploy_thread, NULL);
    furi_thread_set_state_callback(thread, embedded_resources_state_cb);
    furi_thread_set_priority(thread, FuriThreadPriorityLowest);
    furi_thread_start(thread);
}
