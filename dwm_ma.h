#ifndef DWM_MA_H
#define DWM_MA_H

// User-redefinable definitions

#ifndef DWM_MA_SAMPLE_RATE
/**
 * DSP sampling rate used by the dwm-ma implementation
 */
#define DWM_MA_SAMPLE_RATE 16000
#endif

#ifndef DWM_MA_BUFFER_SIZE
/**
 * DSP buffer size used by the dwm-ma implementation
 */
#define DWM_MA_BUFFER_SIZE 128
#endif

#ifndef DWM_MA_SIZE_X_J
/**
 * Junctions size on the X-axis used by the dwm-ma implementation
 */
#define DWM_MA_SIZE_X_J 32
#endif

#ifndef DWM_MA_SIZE_Y_J
/**
 * Junctions size on the Y-axis used by the dwm-ma implementation
 */
#define DWM_MA_SIZE_Y_J 32
#endif

#ifndef DWM_MA_SIZE_Z_J
/**
 * Junctions size on the Z-axis used by the dwm-ma implementation
 */
#define DWM_MA_SIZE_Z_J 32
#endif

#ifndef DWM_MA_SOUND_PROPAGATION_SPEED
/**
 * Sound propagation speed used by the dwm-ma implementation
 */
#define DWM_MA_SOUND_PROPAGATION_SPEED 343.0f
#endif

#ifndef DWM_MA_MAX_INPUT_COUNT
/**
 * Maximum input count used by the dwm-ma implementation
 */
#define DWM_MA_MAX_INPUT_COUNT 16
#endif

// Non user-redefinable definitions

#define _DWM_MA_SQRT_3F 1.73205080757f
#define _DWM_MA_METRIC_2_JUNCTION (DWM_MA_SAMPLE_RATE / (_DWM_MA_SQRT_3F * DWM_MA_SOUND_PROPAGATION_SPEED))
#define _DWM_MA_JUNCTION_2_METRIC (_DWM_MA_SQRT_3F * DWM_MA_SOUND_PROPAGATION_SPEED / DWM_MA_SAMPLE_RATE)

/**
 * Metric size on the X-axis used by the dwm-ma implementation
 */
#define DWM_MA_SIZE_X_M (DWM_MA_SIZE_X_J * _DWM_MA_JUNCTION_2_METRIC)

/**
 * Metric size on the Y-axis used by the dwm-ma implementation
 */
#define DWM_MA_SIZE_Y_M (DWM_MA_SIZE_Y_J * _DWM_MA_JUNCTION_2_METRIC)

/**
 * Metric size on the Z-axis used by the dwm-ma implementation
 */
#define DWM_MA_SIZE_Z_M (DWM_MA_SIZE_Z_J * _DWM_MA_JUNCTION_2_METRIC)

/**
 * Metric size on a single junction used by the dwm-ma implementation
 */
#define DWM_MA_SIZE_JUNCTION_M _DWM_MA_JUNCTION_2_METRIC

// dwm-ma functions

#include "ma_config.h"

/**
 * Creates a new dwm-ma instance
 * @param dwm_ma address of dwm-ma handle
 * @note The dwm dimensions are provided by the DWM_MA_SIZE_?_J and DWM_MA_SIZE_?_M definitions
 * (J for junction count, M for metric)
 */
void dwm_ma_create(void **dwm_ma);

/**
 * Destroys a dwm-ma instance
 * @param dwm_ma address of a valid dwm-ma handle
 */
void dwm_ma_destroy(void **dwm_ma);

/**
 * Initializes a dwm-ma instance to the initial state
 * @param dwm_ma address of a valid dwm-ma handle
 * @param dwm_bound_params dwm boundary parameters, in order [Z-,Y-,X-,X+,Y+,Z+]. The way these parameters are
 * interpreted is controlled by dwm_bound_params_normalized
 * @param dwm_bound_params_normalized \n
 * dwm_bound_params_normalized = 0 -> \n
 * + dwm_bound_params[AXIS][0] = R1, \n
 * + dwm_bound_params[AXIS][1] = R2 \n
 * dwm_bound_params_normalized != 0 -> \n
 * + dwm_bound_params[AXIS][0] = normalized admittance (in [0, 1] range), \n
 * + dwm_bound_params[AXIS][1] = normalized low-pass cutoff (in [0, 1] range)
 * @details R1 and R2 refer to the filter parameters mentioned in Kelloniemi, Antti. "Frequency-dependent boundary
 * condition for the 3-D digital waveguide mesh." Proc. Int. Conf. Digital Audio Effects (DAFx’06). 2006.
 */
void dwm_ma_init(void *dwm_ma, const float dwm_bound_params[6][2], int dwm_bound_params_normalized);

/**
 * Processes DWM_MA_BUFFER_SIZE samples inside a dwm-ma, with dwm coordinates expressed in metric units
 * @param dwm_ma address of a valid dwm-ma handle
 * @param in_buffers samples introduced by each input (dimensionality in_count x DWM_MA_BUFFER_SIZE)
 * @param in_positions_m metric positions of each input (dimensionality in_count x 3)
 * @param in_count amount of inputs processed (no more than DWM_MA_MAX_INPUT_COUNT)
 * @param ma_config microphone array configuration used
 * @param ma_scale microphone array scale
 * @param ma_buffers samples outputted by each microphone (dimensionality ma_config->channel_count x DWM_MA_BUFFER_SIZE)
 * @param ma_position_m microphone array's center position (dimensionality 1 x 3)
 * @note Non-valid coordinates are clamped to valid mesh coordinates, the microphone array's center is also moved to the
 * nearest valid position inside the mesh to avoid sampling of non-valid coordinates
 * @note Positions between discrete junctions are read/written with trilinear interpolation
 * @note Non-valid ma_config values result in MA_CONFIG_MONO being used
 */
void dwm_ma_process_interpolated(void *dwm_ma, const float *const *in_buffers, const float *const *in_positions_m,
                                 int in_count, MA_CONFIG ma_config, float ma_scale, float *const *ma_buffers,
                                 const float *ma_position_m);

#endif
