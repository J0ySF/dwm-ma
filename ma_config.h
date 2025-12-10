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

#endif
