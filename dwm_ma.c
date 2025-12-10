#include "dwm_ma.h"

#include <assert.h>

typedef struct {

} dwm_ma_t;

void dwm_ma_create(void **dwm_ma) {
    // Assert that the user-redefinable definitions have legal values
    static_assert(DWM_MA_SAMPLE_RATE >= 1, "dwm-ma DSP sample rate must be greater or equal than 1");
    static_assert(DWM_MA_BUFFER_SIZE >= 1, "dwm-ma DSP buffer size must be greater or equal than 1");
    static_assert(DWM_MA_SIZE_X_J >= 1, "dwm-ma junctions size on the X-axis must be greater or equal than 1");
    static_assert(DWM_MA_SIZE_Y_J >= 1, "dwm-ma junctions size on the Y-axis must be greater or equal than 1");
    static_assert(DWM_MA_SIZE_Z_J >= 1, "dwm-ma junctions size on the Z-axis must be greater or equal than 1");
    static_assert(DWM_MA_SOUND_PROPAGATION_SPEED >= 1, "dwm-ma sound propagation speed must be greater than 0");
    static_assert(DWM_MA_MAX_INPUT_COUNT >= 1, "dwm-ma sample rate must be greater or equal than 1");

    // TODO: implement
}
