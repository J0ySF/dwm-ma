#ifndef MA_CONFIG_H
#define MA_CONFIG_H

/**
 * The following layouts employ the following naming scheme (bar for MONO and STEREO):
 * {N_POINTS}_SQRT_{SQUARE}, where: \n
 * N_POINTS refers to the number of points in the layout, \n
 * SQUARE refers to the square of the layout sphere's radius
 */
typedef enum {
    /**
     * Monophonic output
     * @remark Output 0 at the center position
     */
    MA_CONFIG_MONO = 0,
    /**
     * Stereophonic output
     * @remark Output 0 at -1 junction distance on the X-axis, output 1 at +1 junction distance on the X-axis
     */
    MA_CONFIG_STEREO,

    //
    MA_CONFIG_6_POINTS_SQRT_1,
    MA_CONFIG_8_POINTS_SQRT_3,
    MA_CONFIG_12_POINTS_SQRT_2,
    MA_CONFIG_24_POINTS_SQRT_5,
    MA_CONFIG_24_POINTS_SQRT_6,
    MA_CONFIG_24_POINTS_SQRT_10,
    MA_CONFIG_24_POINTS_SQRT_11,
    MA_CONFIG_24_POINTS_SQRT_13,
    MA_CONFIG_30_POINTS_SQRT_9,
} MA_CONFIG;

/**
 * Maximum possible output count used by any microphone array layout
 */
#define DWM_MA_MAX_OUTPUT_COUNT 30

/**
 * Microphone array layout
 */
typedef struct {
    /**
     * Microphone array metric units radius
     */
    const float radius_m;
    /**
     * Microphone array channels count
     */
    const int channel_count;
    /**
     * Azimuth-elevation couples for each microphone, in radians (valid data dimensionality channel_count x 2)
     */
    const float mic_azi_elev[DWM_MA_MAX_OUTPUT_COUNT][2];
    /**
     * Relative junction X-Y-Z coordinates to the center of the array for each microphone (valid data dimensionality
     * channel_count x 3)
     * @remark All layouts' channels are ordered lexicographically in order of relative coordinates to the center of the
     */
    const int mic_rel_xyz_j[DWM_MA_MAX_OUTPUT_COUNT][3];
} ma_layout;

const ma_layout *ma_config_layout(MA_CONFIG ma_config);

#endif
