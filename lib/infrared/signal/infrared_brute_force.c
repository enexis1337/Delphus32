#include "infrared_brute_force.h"

#include <stdlib.h>
#include <string.h>
#include <m-dict.h>
#include <m-array.h>
#include <flipper_format/flipper_format.h>

#include "infrared_signal.h"

ARRAY_DEF(SignalPositionArray, size_t, M_DEFAULT_OPLIST); //-V658

typedef struct {
    size_t index;
    SignalPositionArray_t signals;
} InfraredBruteForceRecord;

static inline void ir_bf_record_init(InfraredBruteForceRecord* record) {
    record->index = 0;
    SignalPositionArray_init(record->signals);
}
#define IR_BF_RECORD_INIT(r) (ir_bf_record_init(&(r)))

static inline void
    ir_bf_record_init_set(InfraredBruteForceRecord* dest, const InfraredBruteForceRecord* src) {
    dest->index = src->index;
    SignalPositionArray_init_set(dest->signals, src->signals);
}
#define IR_BF_RECORD_INIT_SET(d, s) (ir_bf_record_init_set(&(d), &(s)))

static inline void
    ir_bf_record_set(InfraredBruteForceRecord* dest, const InfraredBruteForceRecord* src) {
    dest->index = src->index;
    SignalPositionArray_set(dest->signals, src->signals);
}
#define IR_BF_RECORD_SET(d, s) (ir_bf_record_set(&(d), &(s)))

static inline void ir_bf_record_clear(InfraredBruteForceRecord* record) {
    SignalPositionArray_clear(record->signals);
}
#define IR_BF_RECORD_CLEAR(r) (ir_bf_record_clear(&(r)))

#define IR_BF_RECORD_OPLIST           \
    (INIT(IR_BF_RECORD_INIT),         \
     INIT_SET(IR_BF_RECORD_INIT_SET), \
     SET(IR_BF_RECORD_SET),           \
     CLEAR(IR_BF_RECORD_CLEAR))

DICT_DEF2(
    InfraredBruteForceRecordDict,
    FuriString*,
    FURI_STRING_OPLIST,
    InfraredBruteForceRecord,
    IR_BF_RECORD_OPLIST);

struct InfraredBruteForce {
    FlipperFormat* ff;
    const char* db_filename;
    FuriString* current_record_name;
    InfraredBruteForceRecord current_record;
    InfraredSignal* current_signal;
    InfraredBruteForceRecordDict_t records;
    bool is_started;
};

InfraredBruteForce* infrared_brute_force_alloc(void) {
    InfraredBruteForce* brute_force = malloc(sizeof(InfraredBruteForce));
    brute_force->ff = NULL;
    brute_force->db_filename = NULL;
    brute_force->current_signal = NULL;
    brute_force->is_started = false;
    brute_force->current_record_name = furi_string_alloc();
    InfraredBruteForceRecordDict_init(brute_force->records);
    return brute_force;
}

void infrared_brute_force_free(InfraredBruteForce* brute_force) {
    furi_check(brute_force);
    furi_assert(!brute_force->is_started);
    InfraredBruteForceRecordDict_clear(brute_force->records);
    furi_string_free(brute_force->current_record_name);
    free(brute_force);
}

void infrared_brute_force_set_db_filename(InfraredBruteForce* brute_force, const char* db_filename) {
    furi_check(brute_force);
    furi_assert(!brute_force->is_started);
    brute_force->db_filename = db_filename;
}

InfraredErrorCode infrared_brute_force_calculate_messages(InfraredBruteForce* brute_force) {
    furi_check(brute_force);
    furi_assert(!brute_force->is_started);
    furi_assert(brute_force->db_filename);
    InfraredErrorCode error = InfraredErrorCodeNone;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);
    FuriString* signal_name = furi_string_alloc();
    InfraredSignal* signal = infrared_signal_alloc();

    const char* db_path = brute_force->db_filename;
    bool file_opened = false;

    FURI_LOG_I("IR_BF", "Trying to open database: %s", db_path);

    /* First attempt: original path */
    if(flipper_format_buffered_file_open_existing(ff, db_path)) {
        file_opened = true;
        FURI_LOG_I("IR_BF", "Opened database: %s", db_path);
    } else {
        FURI_LOG_W("IR_BF", "Failed to open database: %s", db_path);

        /* Fallback: /ext/infrared/assets/X.ir -> /ext/infrared/X.ir */
        const char* assets_marker = strstr(db_path, "/assets/");
        if(assets_marker) {
            size_t prefix_len = (size_t)(assets_marker - db_path);
            const char* filename = assets_marker + 8;
            FuriString* fallback = furi_string_alloc();
            furi_string_set_strn(fallback, db_path, prefix_len);
            furi_string_cat_printf(fallback, "/%s", filename);
            const char* fallback_cstr = furi_string_get_cstr(fallback);

            FURI_LOG_I("IR_BF", "Trying fallback path: %s", fallback_cstr);
            flipper_format_free(ff);
            ff = flipper_format_buffered_file_alloc(storage);

            if(flipper_format_buffered_file_open_existing(ff, fallback_cstr)) {
                file_opened = true;
                FURI_LOG_I("IR_BF", "Opened fallback database: %s", fallback_cstr);
            }
            furi_string_free(fallback);
        }

        /* Reverse fallback: /ext/infrared/X.ir -> /ext/infrared/assets/X.ir */
        if(!file_opened && !assets_marker) {
            const char* last_slash = strrchr(db_path, '/');
            if(last_slash) {
                size_t dir_len = (size_t)(last_slash - db_path);
                const char* filename = last_slash + 1;
                FuriString* fallback = furi_string_alloc();
                furi_string_set_strn(fallback, db_path, dir_len);
                furi_string_cat_printf(fallback, "/assets/%s", filename);
                const char* fallback_cstr = furi_string_get_cstr(fallback);

                FURI_LOG_I("IR_BF", "Trying assets fallback: %s", fallback_cstr);
                flipper_format_free(ff);
                ff = flipper_format_buffered_file_alloc(storage);

                if(flipper_format_buffered_file_open_existing(ff, fallback_cstr)) {
                    file_opened = true;
                }
                furi_string_free(fallback);
            }
        }
    }

    if(!file_opened) {
        FURI_LOG_E("IR_BF", "All database paths failed for: %s", brute_force->db_filename);
        error = InfraredErrorCodeFileOperationFailed;
    }

    if(error == InfraredErrorCodeNone) {
        bool signal_valid = false;
        while(infrared_signal_read_name(ff, signal_name) == InfraredErrorCodeNone) {
            size_t signal_start = flipper_format_tell(ff);
            error = infrared_signal_read_body(signal, ff);
            signal_valid = (!INFRARED_ERROR_PRESENT(error)) && infrared_signal_is_valid(signal);
            if(!signal_valid) break;

            InfraredBruteForceRecord* record =
                InfraredBruteForceRecordDict_get(brute_force->records, signal_name);
            furi_assert(record);
            SignalPositionArray_push_back(record->signals, signal_start);
        }
    }

    infrared_signal_free(signal);
    furi_string_free(signal_name);
    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return error;
}

bool infrared_brute_force_start(
    InfraredBruteForce* brute_force,
    uint32_t index,
    uint32_t* record_count) {
    furi_check(brute_force);
    furi_assert(!brute_force->is_started);
    bool success = false;
    *record_count = 0;

    InfraredBruteForceRecordDict_it_t it;
    for(InfraredBruteForceRecordDict_it(it, brute_force->records);
        !InfraredBruteForceRecordDict_end_p(it);
        InfraredBruteForceRecordDict_next(it)) {
        const InfraredBruteForceRecordDict_itref_t* record = InfraredBruteForceRecordDict_cref(it);
        if(record->value.index == index) {
            *record_count = SignalPositionArray_size(record->value.signals);
            if(*record_count) {
                furi_string_set(brute_force->current_record_name, record->key);
                brute_force->current_record = record->value;
            }
            break;
        }
    }

    if(*record_count) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        brute_force->ff = flipper_format_buffered_file_alloc(storage);
        brute_force->current_signal = infrared_signal_alloc();
        brute_force->is_started = true;
        success =
            flipper_format_buffered_file_open_existing(brute_force->ff, brute_force->db_filename);
        if(!success) infrared_brute_force_stop(brute_force);
    }
    return success;
}

bool infrared_brute_force_is_started(const InfraredBruteForce* brute_force) {
    furi_check(brute_force);
    return brute_force->is_started;
}

void infrared_brute_force_stop(InfraredBruteForce* brute_force) {
    furi_check(brute_force);
    furi_assert(brute_force->is_started);
    furi_string_reset(brute_force->current_record_name);
    infrared_signal_free(brute_force->current_signal);
    flipper_format_free(brute_force->ff);
    brute_force->current_signal = NULL;
    brute_force->ff = NULL;
    brute_force->is_started = false;
    furi_record_close(RECORD_STORAGE);
}

bool infrared_brute_force_send(InfraredBruteForce* brute_force, uint32_t signal_index) {
    furi_check(brute_force);
    furi_assert(brute_force->is_started);

    if(signal_index >= SignalPositionArray_size(brute_force->current_record.signals)) return false;

    size_t signal_start =
        *SignalPositionArray_cget(brute_force->current_record.signals, signal_index);
    if(!flipper_format_seek(brute_force->ff, signal_start, FlipperFormatOffsetFromStart))
        return false;

    if(INFRARED_ERROR_PRESENT(
           infrared_signal_read_body(brute_force->current_signal, brute_force->ff)))
        return false;

    infrared_signal_transmit(brute_force->current_signal);
    return true;
}

void infrared_brute_force_add_record(
    InfraredBruteForce* brute_force,
    uint32_t index,
    const char* name) {
    InfraredBruteForceRecord value;
    ir_bf_record_init(&value);
    value.index = index;
    FuriString* key;
    key = furi_string_alloc_set(name);
    InfraredBruteForceRecordDict_set_at(brute_force->records, key, value);
    furi_string_free(key);
}

void infrared_brute_force_reset(InfraredBruteForce* brute_force) {
    furi_assert(!brute_force->is_started);
    InfraredBruteForceRecordDict_reset(brute_force->records);
}
