#include "ma_config.h"

static const ma_layout layouts[5] = {
    // MA_CONFIG_MONO
    {
        0.0f,
        1,
        {{0, 0}},
        {{0, 0, 0}},
    },
    // MA_CONFIG_STEREO_75MM
    {
        0.75f,
        2,
        {{1.57079632679f, 0}, {-1.57079632679f, 0}},
        {{-0.75f, 0, 0}, {0.75f, 0, 0}},
    },
    // TODO: update the rest with actual data
    // MA_CONFIG_6_POINTS_1J
    {
        0.0f,
        1,
        {{0, 0}},
        {{0, 0, 0}},
    },
    // MA_CONFIG_6_POINTS_3J
    {
        0.0f,
        1,
        {{0, 0}},
        {{0, 0, 0}},
    },
    // MA_CONFIG_30_POINTS_3J
    {
        0.0f,
        1,
        {{0, 0}},
        {{0, 0, 0}},
    }
};

const ma_layout *ma_config_layout(MA_CONFIG ma_config) {
    // Set non-valid ma_config values to MA_CONFIG_MONO
    switch (ma_config) {
        case MA_CONFIG_MONO:
        case MA_CONFIG_STEREO_75MM:
        case MA_CONFIG_6_POINTS_1J:
        case MA_CONFIG_6_POINTS_3J:
        case MA_CONFIG_30_POINTS_3J:
        default:
            ma_config = MA_CONFIG_MONO;
            break;
    }
    return &layouts[ma_config];
}
