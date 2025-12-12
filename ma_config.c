#include "ma_config.h"

#include "dwm_ma.h"

static const ma_layout MA_CONFIG_MONO_layout = {
        0.0f,
        1,
        {{0, 0, 0}},
};

static const ma_layout MA_CONFIG_STEREO_layout = {
        DWM_MA_SIZE_JUNCTION_M,
        2,
        {{-1, 0, 0}, {1, 0, 0}},
};

static const ma_layout MA_CONFIG_6_POINTS_SQRT_1_layout = {
        DWM_MA_SIZE_JUNCTION_M,
        6,
        {
                {0, 0, -1},
                {0, -1, 0},
                {-1, 0, 0},
                {1, 0, 0},
                {0, 1, 0},
                {0, 0, 1},
        },
};

static const ma_layout MA_CONFIG_8_POINTS_SQRT_3_layout = {
        DWM_MA_SIZE_JUNCTION_M * 1.73205080757f, // sqrt(3)
        8,
        {
                {-1, -1, -1},
                {1, -1, -1},
                {-1, 1, -1},
                {1, 1, -1},
                {-1, -1, 1},
                {1, -1, 1},
                {-1, 1, 1},
                {1, 1, 1},
        },
};

static const ma_layout MA_CONFIG_12_POINTS_SQRT_2_layout = {
        DWM_MA_SIZE_JUNCTION_M * 1.41421356237f, // sqrt(2)
        12,
        {
                {0, -1, -1},
                {-1, 0, -1},
                {1, 0, -1},
                {0, 1, -1},
                {-1, -1, 0},
                {1, -1, 0},
                {-1, 1, 0},
                {1, 1, 0},
                {0, -1, 1},
                {-1, 0, 1},
                {1, 0, 1},
                {0, 1, 1},
        },
};

static const ma_layout MA_CONFIG_24_POINTS_SQRT_5_layout = {
        DWM_MA_SIZE_JUNCTION_M * 2.2360679775f, // sqrt(5)
        24,
        {
                {0, -1, -2}, {-1, 0, -2}, {1, 0, -2},  {0, 1, -2}, {0, -2, -1}, {-2, 0, -1}, {2, 0, -1}, {0, 2, -1},
                {-1, -2, 0}, {1, -2, 0},  {-2, -1, 0}, {2, -1, 0}, {-2, 1, 0},  {2, 1, 0},   {-1, 2, 0}, {1, 2, 0},
                {0, -2, 1},  {-2, 0, 1},  {2, 0, 1},   {0, 2, 1},  {0, -1, 2},  {-1, 0, 2},  {1, 0, 2},  {0, 1, 2},
        },
};

static const ma_layout MA_CONFIG_24_POINTS_SQRT_6_layout = {
        DWM_MA_SIZE_JUNCTION_M * 2.44948974278f, // sqrt(6)
        24,
        {
                {-1, -1, -2}, {1, -1, -2}, {-1, 1, -2}, {1, 1, -2}, {-1, -2, -1}, {1, -2, -1},
                {-2, -1, -1}, {2, -1, -1}, {-2, 1, -1}, {2, 1, -1}, {-1, 2, -1},  {1, 2, -1},
                {-1, -2, 1},  {1, -2, 1},  {-2, -1, 1}, {2, -1, 1}, {-2, 1, 1},   {2, 1, 1},
                {-1, 2, 1},   {1, 2, 1},   {-1, -1, 2}, {1, -1, 2}, {-1, 1, 2},   {1, 1, 2},
        },
};

static const ma_layout MA_CONFIG_24_POINTS_SQRT_10_layout = {
        DWM_MA_SIZE_JUNCTION_M * 3.16227766017f, // sqrt(10)
        24,
        {
                {0, -1, -3}, {-1, 0, -3}, {1, 0, -3},  {0, 1, -3}, {0, -3, -1}, {-3, 0, -1}, {3, 0, -1}, {0, 3, -1},
                {-1, -3, 0}, {1, -3, 0},  {-3, -1, 0}, {3, -1, 0}, {-3, 1, 0},  {3, 1, 0},   {-1, 3, 0}, {1, 3, 0},
                {0, -3, 1},  {-3, 0, 1},  {3, 0, 1},   {0, 3, 1},  {0, -1, 3},  {-1, 0, 3},  {1, 0, 3},  {0, 1, 3},
        },
};

static const ma_layout MA_CONFIG_24_POINTS_SQRT_11_layout = {
        DWM_MA_SIZE_JUNCTION_M * 3.31662479036f, // sqrt(11)
        24,
        {
                {-1, -1, -3}, {1, -1, -3}, {-1, 1, -3}, {1, 1, -3}, {-1, -3, -1}, {1, -3, -1},
                {-3, -1, -1}, {3, -1, -1}, {-3, 1, -1}, {3, 1, -1}, {-1, 3, -1},  {1, 3, -1},
                {-1, -3, 1},  {1, -3, 1},  {-3, -1, 1}, {3, -1, 1}, {-3, 1, 1},   {3, 1, 1},
                {-1, 3, 1},   {1, 3, 1},   {-1, -1, 3}, {1, -1, 3}, {-1, 1, 3},   {1, 1, 3},
        },
};

static const ma_layout MA_CONFIG_24_POINTS_SQRT_13_layout = {
        DWM_MA_SIZE_JUNCTION_M * 3.60555127546f, // sqrt(13)
        24,
        {
                {0, -2, -3}, {-2, 0, -3}, {2, 0, -3},  {0, 2, -3}, {0, -3, -2}, {-3, 0, -2}, {3, 0, -2}, {0, 3, -2},
                {-2, -3, 0}, {2, -3, 0},  {-3, -2, 0}, {3, -2, 0}, {-3, 2, 0},  {3, 2, 0},   {-2, 3, 0}, {2, 3, 0},
                {0, -3, 2},  {-3, 0, 2},  {3, 0, 2},   {0, 3, 2},  {0, -2, 3},  {-2, 0, 3},  {2, 0, 3},  {0, 2, 3},
        },
};

static const ma_layout MA_CONFIG_30_POINTS_SQRT_9_layout = {
        DWM_MA_SIZE_JUNCTION_M * 3,
        30,
        {
                {0, 0, -3}, {-1, -2, -2}, {1, -2, -2}, {-2, -1, -2}, {2, -1, -2}, {-2, 1, -2}, {2, 1, -2}, {-1, 2, -2},
                {1, 2, -2}, {-2, -2, -1}, {2, -2, -1}, {-2, 2, -1},  {2, 2, -1},  {0, -3, 0},  {-3, 0, 0}, {3, 0, 0},
                {0, 3, 0},  {-2, -2, 1},  {2, -2, 1},  {-2, 2, 1},   {2, 2, 1},   {-1, -2, 2}, {1, -2, 2}, {-2, -1, 2},
                {2, -1, 2}, {-2, 1, 2},   {2, 1, 2},   {-1, 2, 2},   {1, 2, 2},   {0, 0, 3},
        },
};

static const ma_layout MA_CONFIG_48_POINTS_SQRT_14_layout = {
        DWM_MA_SIZE_JUNCTION_M * 3.74165738677f, // sqrt(14)
        48,
        {
                {-1, -2, -3}, {1, -2, -3}, {-2, -1, -3}, {2, -1, -3}, {-2, 1, -3}, {2, 1, -3}, {-1, 2, -3}, {1, 2, -3},
                {-1, -3, -2}, {1, -3, -2}, {-3, -1, -2}, {3, -1, -2}, {-3, 1, -2}, {3, 1, -2}, {-1, 3, -2}, {1, 3, -2},
                {-2, -3, -1}, {2, -3, -1}, {-3, -2, -1}, {3, -2, -1}, {-3, 2, -1}, {3, 2, -1}, {-2, 3, -1}, {2, 3, -1},
                {-2, -3, 1},  {2, -3, 1},  {-3, -2, 1},  {3, -2, 1},  {-3, 2, 1},  {3, 2, 1},  {-2, 3, 1},  {2, 3, 1},
                {-1, -3, 2},  {1, -3, 2},  {-3, -1, 2},  {3, -1, 2},  {-3, 1, 2},  {3, 1, 2},  {-1, 3, 2},  {1, 3, 2},
                {-1, -2, 3},  {1, -2, 3},  {-2, -1, 3},  {2, -1, 3},  {-2, 1, 3},  {2, 1, 3},  {-1, 2, 3},  {1, 2, 3},
        },
};

const ma_layout *ma_config_layout(const MA_CONFIG ma_config) {
    switch (ma_config) {
        case MA_CONFIG_6_POINTS_SQRT_1:
            return &MA_CONFIG_6_POINTS_SQRT_1_layout;
        case MA_CONFIG_8_POINTS_SQRT_3:
            return &MA_CONFIG_8_POINTS_SQRT_3_layout;
        case MA_CONFIG_12_POINTS_SQRT_2:
            return &MA_CONFIG_12_POINTS_SQRT_2_layout;
        case MA_CONFIG_24_POINTS_SQRT_5:
            return &MA_CONFIG_24_POINTS_SQRT_5_layout;
        case MA_CONFIG_24_POINTS_SQRT_6:
            return &MA_CONFIG_24_POINTS_SQRT_6_layout;
        case MA_CONFIG_24_POINTS_SQRT_10:
            return &MA_CONFIG_24_POINTS_SQRT_10_layout;
        case MA_CONFIG_24_POINTS_SQRT_11:
            return &MA_CONFIG_24_POINTS_SQRT_11_layout;
        case MA_CONFIG_24_POINTS_SQRT_13:
            return &MA_CONFIG_24_POINTS_SQRT_13_layout;
        case MA_CONFIG_30_POINTS_SQRT_9:
            return &MA_CONFIG_30_POINTS_SQRT_9_layout;
        case MA_CONFIG_48_POINTS_SQRT_14:
            return &MA_CONFIG_48_POINTS_SQRT_14_layout;
        case MA_CONFIG_STEREO:
            return &MA_CONFIG_STEREO_layout;
        case MA_CONFIG_MONO:
        default:
            return &MA_CONFIG_MONO_layout;
    }
}
