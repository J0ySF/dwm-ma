#include "dwm_ma.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Internal structs and functions declarations

/**
 * Internal dwm-ma boundary filter implementation, based on the solution described in
 * Kelloniemi, Antti. "Frequency-dependent boundary condition for the 3-D digital waveguide mesh." Proc. Int. Conf. Digital Audio Effects (DAFx’06). 2006.
 */
typedef struct {
    float t1, t2, t3;
} dwm_boundary_t;

/**
 * Internal dwm-ma implementation, based on a rectilinear junction scheme with 1-D boundaries
 */
typedef struct {
    float *p, *p_aux;
    dwm_boundary_t *b_xp, *b_xn, *b_yp, *b_yn, *b_zp, *b_zn;
    float b_params[6][2];
} dwm_ma_t;

/**
 * Computes the linearized junction index inside the 3D volume, given each axis' junction coordinate
 */
static int linearized_index_xyz(int x_j, int y_j, int z_j);

/**
 * Computes the interpolation parameters for a metric units coordinate
 * @param pos_m metric units XYZ position
 * @param pos_m_offset metric units XYZ position offset, ignored if NULL
 * @param interp_percents resulting XYZ interpolation percentages
 * @param interp_indices resulting X[0,1]-Y[0,1]-Z[0,1] interpolation coordinate
 * @note coordinates outside the mesh are clamped inside to valid coordinates
 */
static void compute_interpolation_parameters(const float *pos_m, const float *pos_m_offset,
                                             float interp_percents[3], int interp_indices[2][2][2]);

/**
 * Writes a value using pre-computed interpolation parameters
 * @param handle dwm-ma handle
 * @param value value to be written
 * @param interp_percents pre-computed XYZ interpolation percentages
 * @param interp_indices pre-computed X[0,1]-Y[0,1]-Z[0,1] interpolation coordinate
 */
static void write_value_interp_params(const dwm_ma_t *handle, float value,
                                      const float interp_percents[3], const int interp_indices[2][2][2]);

/**
 * Reads a value using pre-computed interpolation parameters
 * @param handle dwm-ma handle
 * @param interp_percents pre-computed XYZ interpolation percentages
 * @param interp_indices pre-computed X[0,1]-Y[0,1]-Z[0,1] interpolation coordinate
 */
static float read_value_interp_params(const dwm_ma_t *handle,
                                      const float interp_percents[3], const int interp_indices[2][2][2]);

/**
 * Progress the simulation state by one step on the whole mesh
 * @param handle dwm-ma handle
 */
static void process_iteration(dwm_ma_t *handle);

/**
 * Progress a boundary's simulation state by one step and filter an input sample
 * @param b boundary position handle
 * @param in input sample
 * @param r R1 and R2 filter values
 * @return the filtered in value
 */
static float process_boundary(dwm_boundary_t *b, float in, const float r[2]);

/**
 * Minimum of two integers
 */
static int mini(int a, int b);

/**
 * Maximum of two integers
 */
static int maxi(int a, int b);

/**
 * First value limited to be between the second and third arguments
 */
static int clampi(int v, int min, int max);

/**
 * First value limited to be between the second and third arguments
 * @note min must be less or equal than max
 */
static float fclampf(float v, float min, float max);

/**
 * Floating point unclamped linear interpolation between a and b controlled by percentage f
 */
float flerpf(float a, float b, float f);

// Function definitions

void dwm_ma_create(void **dwm_ma) {
    // Assert at compile time that the user-redefinable definitions have legal values
    static_assert(DWM_MA_SAMPLE_RATE >= 1, "dwm-ma DSP sample rate must be greater or equal than 1");
    static_assert(DWM_MA_BUFFER_SIZE >= 1, "dwm-ma DSP buffer size must be greater or equal than 1");
    static_assert(DWM_MA_SIZE_X_J >= 3, "dwm-ma junctions size on the X-axis must be greater or equal than 3");
    static_assert(DWM_MA_SIZE_Y_J >= 3, "dwm-ma junctions size on the Y-axis must be greater or equal than 3");
    static_assert(DWM_MA_SIZE_Z_J >= 3, "dwm-ma junctions size on the Z-axis must be greater or equal than 3");
    static_assert(DWM_MA_SOUND_PROPAGATION_SPEED >= 1, "dwm-ma sound propagation speed must be greater than 0");
    static_assert(DWM_MA_MAX_INPUT_COUNT >= 1, "dwm-ma sample rate must be greater or equal than 1");

    // Allocate all resources
    dwm_ma_t *handle = malloc(sizeof(dwm_ma_t));
    handle->p = (float *) malloc(sizeof(float) * DWM_MA_SIZE_X_J * DWM_MA_SIZE_Y_J * DWM_MA_SIZE_Z_J);
    handle->p_aux = (float *) malloc(sizeof(float) * DWM_MA_SIZE_X_J * DWM_MA_SIZE_Y_J * DWM_MA_SIZE_Z_J);
    handle->b_xp = (dwm_boundary_t *) malloc(sizeof(dwm_boundary_t) * DWM_MA_SIZE_Y_J * DWM_MA_SIZE_Z_J);
    handle->b_xn = (dwm_boundary_t *) malloc(sizeof(dwm_boundary_t) * DWM_MA_SIZE_Y_J * DWM_MA_SIZE_Z_J);
    handle->b_yp = (dwm_boundary_t *) malloc(sizeof(dwm_boundary_t) * DWM_MA_SIZE_X_J * DWM_MA_SIZE_Z_J);
    handle->b_yn = (dwm_boundary_t *) malloc(sizeof(dwm_boundary_t) * DWM_MA_SIZE_X_J * DWM_MA_SIZE_Z_J);
    handle->b_zp = (dwm_boundary_t *) malloc(sizeof(dwm_boundary_t) * DWM_MA_SIZE_X_J * DWM_MA_SIZE_Y_J);
    handle->b_zn = (dwm_boundary_t *) malloc(sizeof(dwm_boundary_t) * DWM_MA_SIZE_X_J * DWM_MA_SIZE_Y_J);
    *dwm_ma = handle;
}

void dwm_ma_destroy(void **dwm_ma) {
    dwm_ma_t *handle = *dwm_ma;

    // Free all resources
    free(handle->p);
    free(handle->p_aux);
    free(handle->b_xp);
    free(handle->b_xn);
    free(handle->b_yp);
    free(handle->b_yn);
    free(handle->b_zp);
    free(handle->b_zn);
    free(handle);
    *dwm_ma = NULL;
}

void dwm_ma_init(void *dwm_ma, float dwm_bound_params[6][2], const int dwm_bound_params_normalized) {
    dwm_ma_t *handle = dwm_ma;

    // Set initial memory state, assumes IEEE 754 float representation where 0-ed out bits correspond to 0.0f
    memset(handle->p, 0, sizeof(float) * DWM_MA_SIZE_X_J * DWM_MA_SIZE_Y_J * DWM_MA_SIZE_Z_J);
    memset(handle->p_aux, 0, sizeof(float) * DWM_MA_SIZE_X_J * DWM_MA_SIZE_Y_J * DWM_MA_SIZE_Z_J);
    memset(handle->b_xp, 0, sizeof(dwm_boundary_t) * DWM_MA_SIZE_Y_J * DWM_MA_SIZE_Z_J);
    memset(handle->b_xn, 0, sizeof(dwm_boundary_t) * DWM_MA_SIZE_Y_J * DWM_MA_SIZE_Z_J);
    memset(handle->b_yp, 0, sizeof(dwm_boundary_t) * DWM_MA_SIZE_X_J * DWM_MA_SIZE_Z_J);
    memset(handle->b_yn, 0, sizeof(dwm_boundary_t) * DWM_MA_SIZE_X_J * DWM_MA_SIZE_Z_J);
    memset(handle->b_zp, 0, sizeof(dwm_boundary_t) * DWM_MA_SIZE_X_J * DWM_MA_SIZE_Y_J);
    memset(handle->b_zn, 0, sizeof(dwm_boundary_t) * DWM_MA_SIZE_X_J * DWM_MA_SIZE_Y_J);

    // Handle the boundary parameters
    if (dwm_bound_params_normalized != 0) {
        // If the parameters are given as normalized admittance & normalized low-pass cutoff, convert to R1 and R2
        for (int i = 0; i < 6; i++) {
            handle->b_params[i][0] = (1 - dwm_bound_params[i][1]) * 0.25f * dwm_bound_params[i][0];
            handle->b_params[i][1] = dwm_bound_params[i][0] * (1 - (1 - dwm_bound_params[i][1]) * 0.5f);
        }
    } else {
        // Otherwise copy R1, R2
        memcpy(handle->b_params, dwm_bound_params, sizeof(float) * 6 * 2);
    }
}

void dwm_ma_process_m(void *dwm_ma,
                      float **in_buffers, float **in_positions_m, int in_count,
                      const MA_CONFIG ma_config, float **ma_buffers, float *ma_position_m) {
    dwm_ma_t *handle = dwm_ma;

    // Protect against non-valid in_count values
    in_count = clampi(in_count, 0, DWM_MA_MAX_INPUT_COUNT);

    // Preprocess each input coordinate's interpolation parameters, since they are the same during the entire buffer
    float input_int_percents[DWM_MA_MAX_INPUT_COUNT][3];
    int input_int_indices[DWM_MA_MAX_INPUT_COUNT][2][2][2];
    for (int i = 0; i < in_count; i++) {
        compute_interpolation_parameters(in_positions_m[i], NULL, input_int_percents[i], input_int_indices[i]);
    }

    // Preprocess the microphone array position such that the entire radius is inside the mesh bounds
    const ma_layout *ma = ma_config_layout(ma_config);
    float ma_position_m_restricted[3];
    ma_position_m_restricted[0] = fclampf(ma_position_m[0], ma->radius, DWM_MA_SIZE_X_M - ma->radius);
    ma_position_m_restricted[1] = fclampf(ma_position_m[1], ma->radius, DWM_MA_SIZE_Y_M - ma->radius);
    ma_position_m_restricted[2] = fclampf(ma_position_m[2], ma->radius, DWM_MA_SIZE_Z_M - ma->radius);

    // Preprocess the output microphone array's interpolation parameters, since they are the same during the entire buffer
    float output_int_percents[DWM_MA_MAX_OUTPUT_COUNT][3];
    int output_int_indices[DWM_MA_MAX_OUTPUT_COUNT][2][2][2];
    for (int i = 0; i < ma->channel_count; i++) {
        compute_interpolation_parameters(ma->mic_rel_xyz_m[i], ma_position_m_restricted,
                                         output_int_percents[i], output_int_indices[i]);
    }

    // Run DWM_MA_BUFFER_SIZE simulation iterations
    for (int n = 0; n < DWM_MA_BUFFER_SIZE; n++) {
        // Write all sources
        for (int i = 0; i < in_count; i++) {
            write_value_interp_params(handle, in_buffers[i][n], input_int_percents[i], input_int_indices[i]);
        }
        process_iteration(handle); // Single simulation interation
        // Read all mic outputs
        for (int i = 0; i < ma->channel_count; i++) {
            ma_buffers[i][n] = read_value_interp_params(handle, output_int_percents[i], output_int_indices[i]);
        }
        {
            float *aux = handle->p; // Post iteration buffer swapping
            handle->p = handle->p_aux;
            handle->p_aux = aux;
        }
    }
}

static int linearized_index_xyz(const int x_j, const int y_j, const int z_j) {
    return (z_j * DWM_MA_SIZE_Y_J + y_j) * DWM_MA_SIZE_X_J + x_j;
}

void compute_interpolation_parameters(const float *pos_m, const float *pos_m_offset,
                                      float interp_percents[3], int interp_indices[2][2][2]) {
    // Translate the metric coordinates to valid floating point junction coordinates
    const float x_j = fclampf(
        (pos_m[0] + (pos_m_offset != NULL ? pos_m_offset[0] : 0.0f)) * _DWM_MA_METRIC_2_JUNCTION - 0.5f
        , 0.0f, DWM_MA_SIZE_X_J - 1.0f);
    const float y_j = fclampf(
        (pos_m[1] + (pos_m_offset != NULL ? pos_m_offset[1] : 0.0f)) * _DWM_MA_METRIC_2_JUNCTION - 0.5f
        , 0.0f, DWM_MA_SIZE_Y_J - 1.0f);
    const float z_j = fclampf(
        (pos_m[2] + (pos_m_offset != NULL ? pos_m_offset[2] : 0.0f)) * _DWM_MA_METRIC_2_JUNCTION - 0.5f
        , 0.0f, DWM_MA_SIZE_Z_J - 1.0f);

    // Get the next and previous junction coordinates for each dimension
    const int x_j_0 = (int) floorf(x_j);
    const int x_j_1 = (int) ceilf(x_j);
    const int y_j_0 = (int) floorf(y_j);
    const int y_j_1 = (int) ceilf(y_j);
    const int z_j_0 = (int) floorf(z_j);
    const int z_j_1 = (int) ceilf(z_j);

    // Return the linearized junction sampling indices
    interp_indices[0][0][0] = linearized_index_xyz(x_j_0, y_j_0, z_j_0);
    interp_indices[1][0][0] = linearized_index_xyz(x_j_1, y_j_0, z_j_0);
    interp_indices[0][1][0] = linearized_index_xyz(x_j_0, y_j_1, z_j_0);
    interp_indices[1][1][0] = linearized_index_xyz(x_j_1, y_j_1, z_j_0);
    interp_indices[0][0][1] = linearized_index_xyz(x_j_0, y_j_0, z_j_1);
    interp_indices[1][0][1] = linearized_index_xyz(x_j_1, y_j_0, z_j_1);
    interp_indices[0][1][1] = linearized_index_xyz(x_j_0, y_j_1, z_j_1);
    interp_indices[1][1][1] = linearized_index_xyz(x_j_1, y_j_1, z_j_1);

    float _; // Return each axis' interpolation percentages
    interp_percents[0] = modff(x_j, &_);
    interp_percents[1] = modff(y_j, &_);
    interp_percents[2] = modff(z_j, &_);
}

void write_value_interp_params(const dwm_ma_t *handle, const float value,
                               const float interp_percents[3], const int interp_indices[2][2][2]) {
    handle->p[interp_indices[0][0][0]] = flerpf(handle->p[interp_indices[0][0][0]], value,
                                                (1 - interp_percents[0]) *
                                                (1 - interp_percents[1]) *
                                                (1 - interp_percents[2]));
    handle->p[interp_indices[1][0][0]] = flerpf(handle->p[interp_indices[1][0][0]], value,
                                                interp_percents[0] *
                                                (1 - interp_percents[1]) *
                                                (1 - interp_percents[2]));
    handle->p[interp_indices[0][1][0]] = flerpf(handle->p[interp_indices[0][1][0]], value,
                                                (1 - interp_percents[0]) *
                                                interp_percents[1] *
                                                (1 - interp_percents[2]));
    handle->p[interp_indices[1][1][0]] = flerpf(handle->p[interp_indices[1][1][0]], value,
                                                interp_percents[0] *
                                                interp_percents[1] *
                                                (1 - interp_percents[2]));
    handle->p[interp_indices[0][0][1]] = flerpf(handle->p[interp_indices[0][0][1]], value,
                                                (1 - interp_percents[0]) *
                                                (1 - interp_percents[1]) *
                                                interp_percents[2]);
    handle->p[interp_indices[1][0][1]] = flerpf(handle->p[interp_indices[1][0][1]], value,
                                                interp_percents[0] *
                                                (1 - interp_percents[1]) *
                                                interp_percents[2]);
    handle->p[interp_indices[0][1][1]] = flerpf(handle->p[interp_indices[0][1][1]], value,
                                                (1 - interp_percents[0]) *
                                                interp_percents[1] *
                                                interp_percents[2]);
    handle->p[interp_indices[1][1][1]] = flerpf(handle->p[interp_indices[1][1][1]], value,
                                                interp_percents[0] *
                                                interp_percents[1] *
                                                interp_percents[2]);
}

float read_value_interp_params(const dwm_ma_t *handle,
                               const float interp_percents[3], const int interp_indices[2][2][2]) {
    return flerpf(
        flerpf(
            flerpf(handle->p_aux[interp_indices[0][0][0]], handle->p_aux[interp_indices[1][0][0]],
                   interp_percents[0]),
            flerpf(handle->p_aux[interp_indices[0][1][0]], handle->p_aux[interp_indices[1][1][0]],
                   interp_percents[0]),
            interp_percents[1]),
        flerpf(
            flerpf(handle->p_aux[interp_indices[0][0][1]], handle->p_aux[interp_indices[1][0][1]],
                   interp_percents[0]),
            flerpf(handle->p_aux[interp_indices[0][1][1]], handle->p_aux[interp_indices[1][1][1]],
                   interp_percents[0]),
            interp_percents[1]),
        interp_percents[2]
    );
}

void process_iteration(dwm_ma_t *handle) {
    // Iterate trough each unwrapped boundary/internal case along each axis:
    // this implementation method is quite simple and results in code which is easily optimizable by a compiler with the
    // right options, since the macro expansion results in all conditional branches being removed and for loops which
    // iterate a predeterminate amount of times
    int i = 0, i_xp = 0, i_xn = 0, i_yp = 0, i_yn = 0, i_zp = 0, i_zn = 0;

#define UPDATE(ZN, ZP, YN, YP, XN, XP)                                                                                 \
    handle->p_aux[i] = (ZN + YN + XN + XP + YP + ZP) / 3.0f - handle->p_aux[i];                                        \
    i++;

#define XN_INTERNAL handle->p[i - 1]
#define XP_INTERNAL handle->p[i + 1]
#define XN_BOUNDARY process_boundary(&handle->b_xn[i_xn++], handle->p[i], handle->b_params[2])
#define XP_BOUNDARY process_boundary(&handle->b_xp[i_xp++], handle->p[i], handle->b_params[3])
#define AXIS_X(ZN, ZP, YN, YP)                                                                                         \
    UPDATE(ZN, ZP, YN, YP, XN_BOUNDARY, XP_INTERNAL)                                                                   \
    for (int x = 1; x < DWM_MA_SIZE_X_J - 1; x++) {                                                                    \
        UPDATE(ZN, ZP, YN, YP, XN_INTERNAL, XP_INTERNAL)                                                               \
    }                                                                                                                  \
    UPDATE(ZN, ZP, YN, YP, XN_INTERNAL, XP_BOUNDARY)

#define YN_INTERNAL handle->p[i - DWM_MA_SIZE_X_J]
#define YP_INTERNAL handle->p[i + DWM_MA_SIZE_X_J]
#define YN_BOUNDARY process_boundary(&handle->b_yn[i_yn++], handle->p[i], handle->b_params[1])
#define YP_BOUNDARY process_boundary(&handle->b_yp[i_yp++], handle->p[i], handle->b_params[4])
#define AXIS_Y(ZN, ZP)                                                                                                 \
    AXIS_X(ZN, ZP, YN_BOUNDARY, YP_INTERNAL)                                                                           \
    for (int y = 1; y < DWM_MA_SIZE_Y_J - 1; y++) {                                                                    \
        AXIS_X(ZN, ZP, YN_INTERNAL, YP_INTERNAL)                                                                       \
    }                                                                                                                  \
    AXIS_X(ZN, ZP, YN_INTERNAL, YP_BOUNDARY)

#define ZN_INTERNAL handle->p[i - DWM_MA_SIZE_X_J * DWM_MA_SIZE_Y_J]
#define ZP_INTERNAL handle->p[i + DWM_MA_SIZE_X_J * DWM_MA_SIZE_Y_J]
#define ZN_BOUNDARY process_boundary(&handle->b_zn[i_zn++], handle->p[i], handle->b_params[0])
#define ZP_BOUNDARY process_boundary(&handle->b_zp[i_zp++], handle->p[i], handle->b_params[5])
    AXIS_Y(ZN_BOUNDARY, ZP_INTERNAL)
    for (int z = 1; z < DWM_MA_SIZE_Z_J - 1; z++) {
        AXIS_Y(ZN_INTERNAL, ZP_INTERNAL)
    }
    AXIS_Y(ZN_INTERNAL, ZP_BOUNDARY)

    // Undef all macros
#undef UPDATE
#undef XN_INTERNAL
#undef XP_INTERNAL
#undef XN_BOUNDARY
#undef XP_BOUNDARY
#undef AXIS_X
#undef YN_INTERNAL
#undef YP_INTERNAL
#undef YN_BOUNDARY
#undef YP_BOUNDARY
#undef AXIS_Y
#undef ZN_INTERNAL
#undef ZP_INTERNAL
#undef ZN_BOUNDARY
#undef ZP_BOUNDARY
}

float process_boundary(dwm_boundary_t *b, const float in, const float r[2]) {
    const float aux = in - b->t1;
    const float out = r[0] * (aux + b->t3) + (1 + r[1]) * b->t2;
    b->t3 = b->t2;
    b->t1 = out - b->t2;
    b->t2 = aux;
    return out;
}

static int mini(const int a, const int b) {
    return a < b ? a : b;
}

static int maxi(const int a, const int b) {
    return a > b ? a : b;
}

static int clampi(const int v, const int min, const int max) {
    return mini(maxi(v, min), max);
}

float fclampf(const float v, const float min, const float max) {
    return fminf(fmaxf(v, min), max);
}

float flerpf(const float a, const float b, const float f) {
    return a * (1.0f - f) + (b * f);
}
