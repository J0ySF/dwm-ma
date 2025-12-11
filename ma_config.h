#ifndef MA_CONFIG_H
#define MA_CONFIG_H

typedef enum {
    /**
     * Monophonic output
     * @remark Output 0 at the center position
     */
    MA_CONFIG_MONO = 0,
    /**
     * Stereophonic output
     * @remark Output 0 at -75mm on the X-axis, output 1 at +75mm on the X-axis
     */
    MA_CONFIG_STEREO_75MM,
    /**
     * 6 points output at 1 junctions of distance
     * @remark Output 0 to 5 sorted in lexicographical order ([Z-, Y-, X-, X+, Y+, Z+]), sampled from junctions of
     * distance 1 from the center position
     */
    MA_CONFIG_6_POINTS_1J,
    /**
     * 6 points output at 3 junctions of distance
     * @remark Same as MA_CONFIG_6_POINTS_1J but with 3 junctions of distance
     */
    MA_CONFIG_6_POINTS_3J,
    /**
     * 30 points output at 3 junctions of distance
     * @remark Output 0 to 29 sorted in lexicographical order, the points are the integer coordinate points that lay
     * on a sphere of radius = 3
     */
    MA_CONFIG_30_POINTS_3J,
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
     * Microphone array metric unit radius
     */
    const float radius;
    /**
     * Microphone array channels count
     */
    const int channel_count;
    /**
     * Azimuth-elevation couples for each microphone, in radians (valid data dimensionality channel_count x 2)
     */
    const float mic_azi_elev[DWM_MA_MAX_OUTPUT_COUNT][2];
    /**
     * Relative metric X-Y-Z coordinates to the center of the array for each microphone (valid data dimensionality channel_count x 3)
     */
    const float mic_rel_xyz_m[DWM_MA_MAX_OUTPUT_COUNT][3];
} ma_layout;

const ma_layout *ma_config_layout(MA_CONFIG ma_config);

#endif
