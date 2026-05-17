/**
 * @file stm32f1xx_algo.c
 * @brief STM32F1xx Flash algorithm for offline programmer
 *
 * Flash algorithm blob ported from ARM DAPLink project:
 *   https://github.com/ARMmbed/DAPLink/tree/main/source/family/st/stm32f103rb
 *
 * Supports:
 *   - STM32F103 MD (medium density): C8/CB, 64/128KB Flash
 *   - STM32F103 HD (high density): RB/RD/RE, 256/512KB Flash
 *
 * License: Apache-2.0 (from DAPLink)
 */

#include "components/flash_algo/flash_algo.h"

/* ---- STM32F1xx Flash Algorithm Blob (Cortex-M3 Thumb) ---- */

static const uint32_t stm32f1_algo_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x4603b510, 0x4c442000, 0x48446020, 0x48446060, 0x46206060, 0xf01069c0, 0xd1080f04, 0x5055f245,
    0x60204c40, 0x60602006, 0x70fff640, 0x200060a0, 0x4601bd10, 0x69004838, 0x0080f040, 0x61104a36,
    0x47702000, 0x69004834, 0x0004f040, 0x61084932, 0x69004608, 0x0040f040, 0xe0036108, 0x20aaf64a,
    0x60084930, 0x68c0482c, 0x0f01f010, 0x482ad1f6, 0xf0206900, 0x49280004, 0x20006108, 0x46014770,
    0x69004825, 0x0002f040, 0x61104a23, 0x61414610, 0xf0406900, 0x61100040, 0xf64ae003, 0x4a2120aa,
    0x481d6010, 0xf01068c0, 0xd1f60f01, 0x6900481a, 0x0002f020, 0x61104a18, 0x47702000, 0x4603b510,
    0xf0201c48, 0xe0220101, 0x69004813, 0x0001f040, 0x61204c11, 0x80188810, 0x480fbf00, 0xf01068c0,
    0xd1fa0f01, 0x6900480c, 0x0001f020, 0x61204c0a, 0x68c04620, 0x0f14f010, 0x4620d006, 0xf04068c0,
    0x60e00014, 0xbd102001, 0x1c921c9b, 0x29001e89, 0x2000d1da, 0x0000e7f7, 0x40022000, 0x45670123,
    0xcdef89ab, 0x40003000, 0x00000000
};

/*
 * algo_blob entry points (relative offsets from start of blob):
 *   0x20000000 + 0x00 = algo_start
 *   0x20000000 + 0x21 = init
 *   0x20000000 + 0x53 = uninit
 *   0x20000000 + 0x65 = erase_chip
 *   0x20000000 + 0x9F = erase_sector
 *   0x20000000 + 0xDD = program_page
 *
 * Stack: 0x20000800  (2KB above algo + buffer)
 * Static base: 0x20000148 (r9 — used for literal pool base)
 */

/* ---- Sector Layout ---- */
/*
 * STM32F103 Flash is organized as uniform 1KB pages.
 * Medium density: 64 or 128 pages (64KB / 128KB)
 * High density: 256 or 512 pages (256KB / 512KB)
 */

static const sector_info_t stm32f1_md_sectors[] = {
    {0x08000000, 0x400},  /* 1KB per sector, 64/128 sectors */
};

static const sector_info_t stm32f1_hd_sectors[] = {
    {0x08000000, 0x400},  /* 1KB per sector, 256/512 sectors */
};

static const sector_info_t gd32f1_xd_sectors[] = {
    {0x08000000, 0x800},  /* GD32F1 high-capacity parts typically use 2KB pages */
};

static const sector_info_t stm32f4_sectors[] = {
    {0x08000000, 0x4000},   /* sectors 0-3: 16KB */
    {0x08010000, 0x10000},  /* sector 4: 64KB */
    {0x08020000, 0x20000},  /* sector 5+: 128KB */
};

/* ---- Flash Algorithm (shared between MD and HD) ---- */

static const program_target_t stm32f1_flash_algo = {
    .init               = 0x20000021,
    .uninit             = 0x20000053,
    .erase_chip         = 0x20000065,
    .erase_sector       = 0x2000009F,
    .program_page       = 0x200000DD,
    .verify             = 0x0,  /* no verify function in blob */
    {
        .breakpoint     = 0x20000001,
        .static_base    = 0x20000148,  /* R9 — literal pool base */
        .stack_pointer  = 0x20000800,
    },
    .program_buffer     = 0x20000A00,   /* 0x20000000 + 0x00000A00 */
    .algo_start         = 0x20000000,
    .algo_size          = sizeof(stm32f1_algo_blob),
    .algo_blob          = stm32f1_algo_blob,
    .program_buffer_size = 0x00000400,  /* 1KB page buffer */
    .algo_flags         = 0,            /* no special flags */
};


/* ---- STM32F4xx Flash Algorithm Blobs (Cortex-M4 Thumb) ---- */
/*
 * Flash algorithm blobs ported from ARM DAPLink project:
 *   source/family/st/stm32f401re/flash_blob.c
 *   source/family/st/stm32f407ve/flash_blob.c
 *   source/family/st/stm32f439zi/flash_blob.c
 *
 * License: Apache-2.0 (from DAPLink)
 */

static const uint32_t STM32F401RE_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x03004601, 0x28200e00, 0x0940d302, 0xe0051d00, 0xd3022810, 0x1cc00900, 0x0880e000, 0xd50102c9,
    0x43082110, 0x48424770, 0x60414940, 0x60414941, 0x60012100, 0x22f068c1, 0x60c14311, 0x06806940,
    0x483ed406, 0x6001493c, 0x60412106, 0x6081493c, 0x47702000, 0x69014836, 0x43110542, 0x20006101,
    0xb5104770, 0x69014832, 0x43212404, 0x69016101, 0x431103a2, 0x49336101, 0xe0004a30, 0x68c36011,
    0xd4fb03db, 0x43a16901, 0x20006101, 0xb530bd10, 0xffb6f7ff, 0x68ca4926, 0x431a23f0, 0x240260ca,
    0x690a610c, 0x0e0006c0, 0x610a4302, 0x03e26908, 0x61084310, 0x4a214823, 0x6010e000, 0x03ed68cd,
    0x6908d4fb, 0x610843a0, 0x060068c8, 0xd0030f00, 0x431868c8, 0x200160c8, 0xb570bd30, 0x1cc94d14,
    0x68eb0889, 0x26f00089, 0x60eb4333, 0x612b2300, 0xe0174b15, 0x431c692c, 0x6814612c, 0x68ec6004,
    0xd4fc03e4, 0x0864692c, 0x612c0064, 0x062468ec, 0xd0040f24, 0x433068e8, 0x200160e8, 0x1d00bd70,
    0x1f091d12, 0xd1e52900, 0xbd702000, 0x45670123, 0x40023c00, 0xcdef89ab, 0x00005555, 0x40003000,
    0x00000fff, 0x0000aaaa, 0x00000201, 0x00000000
};

static const uint32_t stm32f4xx_512_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0xf3c04601, 0x28203007, 0x2204bf24, 0x1050eb02, 0x2810d205, 0x2203bf26, 0x1010eb02, 0xf4110880,
    0xbf181f80, 0x0010f040, 0x486b4770, 0x60014969, 0x6001496a, 0x6801486a, 0x01f0f041, 0x48696001,
    0xf0106800, 0xd1080f20, 0xf2454867, 0x60015155, 0x60412106, 0x71fff640, 0x20006081, 0x49634770,
    0xf4206808, 0x600a52f8, 0x48616008, 0xf0416801, 0x60014100, 0x47702000, 0xc174f8df, 0x0000f8dc,
    0x0004f040, 0x0000f8cc, 0x0000f8dc, 0x3080f440, 0x0000f8cc, 0x0004f1ac, 0xf4116801, 0xbf1c3f80,
    0x21aaf64a, 0xd0044a50, 0x68036011, 0x3f80f413, 0xf8dcd1fa, 0xf0200000, 0xf8cc0004, 0x20000000,
    0xf3c04770, 0x29203107, 0x2204bf24, 0x1151eb02, 0x2910d205, 0x2203bf26, 0x1111eb02, 0xf4100889,
    0xbf181f80, 0x0110f041, 0x6802483d, 0x02f0f042, 0xf1006002, 0x22020c04, 0x2000f8cc, 0x2000f8dc,
    0xea0323f8, 0x431101c1, 0x1000f8cc, 0x1000f8dc, 0x3180f441, 0x1000f8cc, 0xf4116801, 0xbf1c3f80,
    0x21aaf64a, 0xd0044a30, 0x68036011, 0x3f80f413, 0xf8dcd1fa, 0xf0211000, 0xf8cc0102, 0x68011000,
    0x0ff0f011, 0x2000bf04, 0x68014770, 0x01f0f041, 0x20016001, 0x4b224770, 0x1cc9b430, 0xc000f8d3,
    0x0103f031, 0x0cf0f04c, 0xc000f8c3, 0x0404f103, 0x0c00f04f, 0xc000f8c4, 0xf240bf18, 0xd0252501,
    0xc000f8d4, 0x0c05ea4c, 0xc000f8c4, 0xc000f8d2, 0xc000f8c0, 0xc000f8d3, 0x3f80f41c, 0xf8d4d1fa,
    0xf02cc000, 0xf8c40c01, 0xf8d3c000, 0xf01cc000, 0xd0060ff0, 0xf0406818, 0x601800f0, 0x2001bc30,
    0x1d004770, 0xf1021f09, 0xd1d90204, 0x2000bc30, 0x00004770, 0x45670123, 0x40023c04, 0xcdef89ab,
    0x40023c0c, 0x40023c14, 0x40003000, 0x40023c00, 0x40023c10, 0x00000000
};

static const uint32_t STM32F439ZI_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x03004601, 0x28200e00, 0x0940d302, 0xe0051d00, 0xd3022810, 0x1cc00900, 0x0880e000, 0xd50102c9,
    0x43082110, 0x48464770, 0x60414944, 0x60414945, 0x60012100, 0x22f068c1, 0x60c14311, 0x06806940,
    0x4842d406, 0x60014940, 0x60412106, 0x60814940, 0x47702000, 0x6901483a, 0x43110542, 0x20006101,
    0xb5304770, 0x69014836, 0x43212404, 0x69016101, 0x43290365, 0x69016101, 0x431103a2, 0x49356101,
    0xe0004a32, 0x68c36011, 0xd4fb03db, 0x43a16901, 0x69016101, 0x610143a9, 0xbd302000, 0xf7ffb530,
    0x4927ffaf, 0x23f068ca, 0x60ca431a, 0x610c2402, 0x06c0690a, 0x43020e00, 0x6908610a, 0x431003e2,
    0x48246108, 0xe0004a21, 0x68cd6010, 0xd4fb03ed, 0x43a06908, 0x68c86108, 0x0f000600, 0x68c8d003,
    0x60c84318, 0xbd302001, 0x4d15b570, 0x08891cc9, 0x008968eb, 0x433326f0, 0x230060eb, 0x4b16612b,
    0x692ce017, 0x612c431c, 0x60046814, 0x03e468ec, 0x692cd4fc, 0x00640864, 0x68ec612c, 0x0f240624,
    0x68e8d004, 0x60e84330, 0xbd702001, 0x1d121d00, 0x29001f09, 0x2000d1e5, 0x0000bd70, 0x45670123,
    0x40023c00, 0xcdef89ab, 0x00005555, 0x40003000, 0x00000fff, 0x0000aaaa, 0x00000201, 0x00000000
};

static const program_target_t stm32f4_small_flash_algo = {
    .init                = 0x20000047,
    .uninit              = 0x20000075,
    .erase_chip          = 0x20000083,
    .erase_sector        = 0x200000AF,
    .program_page        = 0x200000FB,
    .verify              = 0x0,
    {
        .breakpoint      = 0x20000001,
        .static_base     = 0x2000016C,
        .stack_pointer   = 0x20000800,
    },
    .program_buffer      = 0x20000A00,
    .algo_start          = 0x20000000,
    .algo_size           = sizeof(STM32F401RE_flash_prog_blob),
    .algo_blob           = STM32F401RE_flash_prog_blob,
    .program_buffer_size = 0x00000400,
    .algo_flags          = 0,
};

static const program_target_t stm32f407_flash_algo = {
    .init                = 0x2000004B,
    .uninit              = 0x2000007F,
    .erase_chip          = 0x20000099,
    .erase_sector        = 0x200000E3,
    .program_page        = 0x20000177,
    .verify              = 0x0,
    {
        .breakpoint      = 0x20000001,
        .static_base     = 0x20000214,
        .stack_pointer   = 0x20002000,
    },
    .program_buffer      = 0x20000A00,
    .algo_start          = 0x20000000,
    .algo_size           = sizeof(stm32f4xx_512_flash_prog_blob),
    .algo_blob           = stm32f4xx_512_flash_prog_blob,
    .program_buffer_size = 0x00000400,
    .algo_flags          = 0,
};

static const program_target_t stm32f439_flash_algo = {
    .init                = 0x20000047,
    .uninit              = 0x20000075,
    .erase_chip          = 0x20000083,
    .erase_sector        = 0x200000BD,
    .program_page        = 0x20000109,
    .verify              = 0x0,
    {
        .breakpoint      = 0x20000001,
        .static_base     = 0x2000017C,
        .stack_pointer   = 0x20000800,
    },
    .program_buffer      = 0x20000A00,
    .algo_start          = 0x20000000,
    .algo_size           = sizeof(STM32F439ZI_flash_prog_blob),
    .algo_blob           = STM32F439ZI_flash_prog_blob,
    .program_buffer_size = 0x00000400,
    .algo_flags          = 0,
};
/* ---- Target Configurations ---- */

static target_cfg_t stm32f1_md_target = {
    .version            = 1,
    .sectors_info       = stm32f1_md_sectors,
    .sector_info_length = 1,
    .flash_regions      = {
        { .start = 0x08000000, .end = 0x08010000, .flags = 1, .flash_algo = (program_target_t *)&stm32f1_flash_algo },
    },
    .ram_regions        = {
        { .start = 0x20000000, .end = 0x20005000 },  /* 20KB SRAM */
    },
    .target_vendor      = "STMicroelectronics",
    .target_part_number = "STM32F103C8/CB",
    .erase_reset        = 1,
};

static target_cfg_t stm32f1_hd_target = {
    .version            = 1,
    .sectors_info       = stm32f1_hd_sectors,
    .sector_info_length = 1,
    .flash_regions      = {
        { .start = 0x08000000, .end = 0x08020000, .flags = 1, .flash_algo = (program_target_t *)&stm32f1_flash_algo },
    },
    .ram_regions        = {
        { .start = 0x20000000, .end = 0x20005000 },
    },
    .target_vendor      = "STMicroelectronics",
    .target_part_number = "STM32F103RB/RD/RE",
    .erase_reset        = 1,
};

static target_cfg_t gd32f1_xd_target = {
    .version            = 1,
    .sectors_info       = gd32f1_xd_sectors,
    .sector_info_length = 1,
    .flash_regions      = {
        { .start = 0x08000000, .end = 0x08300000, .flags = 1, .flash_algo = (program_target_t *)&stm32f1_flash_algo },
    },
    .ram_regions        = {
        { .start = 0x20000000, .end = 0x20018000 },
    },
    .target_vendor      = "GigaDevice",
    .target_part_number = "GD32F1 large-density compatible",
    .erase_reset        = 1,
};

static target_cfg_t stm32f4_256k_target = {
    .version            = 1,
    .sectors_info       = stm32f4_sectors,
    .sector_info_length = 3,
    .flash_regions      = {
        { .start = 0x08000000, .end = 0x08040000, .flags = 1, .flash_algo = (program_target_t *)&stm32f4_small_flash_algo },
    },
    .ram_regions        = {
        { .start = 0x20000000, .end = 0x20010000 },
    },
    .target_vendor      = "STMicroelectronics/GigaDevice",
    .target_part_number = "STM32F4/GD32F4 256KB class",
    .erase_reset        = 1,
};

static target_cfg_t stm32f4_512k_target = {
    .version            = 1,
    .sectors_info       = stm32f4_sectors,
    .sector_info_length = 3,
    .flash_regions      = {
        { .start = 0x08000000, .end = 0x08080000, .flags = 1, .flash_algo = (program_target_t *)&stm32f407_flash_algo },
    },
    .ram_regions        = {
        { .start = 0x20000000, .end = 0x20020000 },
    },
    .target_vendor      = "STMicroelectronics/GigaDevice",
    .target_part_number = "STM32F4/GD32F4 512KB class",
    .erase_reset        = 1,
};

static target_cfg_t stm32f4_1024k_target = {
    .version            = 1,
    .sectors_info       = stm32f4_sectors,
    .sector_info_length = 3,
    .flash_regions      = {
        { .start = 0x08000000, .end = 0x08100000, .flags = 1, .flash_algo = (program_target_t *)&stm32f407_flash_algo },
    },
    .ram_regions        = {
        { .start = 0x20000000, .end = 0x20020000 },
    },
    .target_vendor      = "STMicroelectronics/GigaDevice",
    .target_part_number = "STM32F4/GD32F4 1MB class",
    .erase_reset        = 1,
};

static target_cfg_t stm32f4_2048k_target = {
    .version            = 1,
    .sectors_info       = stm32f4_sectors,
    .sector_info_length = 3,
    .flash_regions      = {
        { .start = 0x08000000, .end = 0x08200000, .flags = 1, .flash_algo = (program_target_t *)&stm32f439_flash_algo },
    },
    .ram_regions        = {
        { .start = 0x20000000, .end = 0x20030000 },
    },
    .target_vendor      = "STMicroelectronics/GigaDevice",
    .target_part_number = "STM32F4/GD32F4 2MB class",
    .erase_reset        = 1,
};

/* ---- Public API ---- */

target_chip_t flash_algo_detect(uint32_t idcode) {
    /*
     * Cortex-M3 IDCODE format:
     *   Bits [31:16] = 0x3BA0 (STMicroelectronics JEDEC code for STM32F1)
     *   Bits [15:0]  = device specific
     *
     * STM32F103 variants:
     *   0x3BA00477 = STM32F103 medium density (C8/CB)
     *   0x3BA00414 = STM32F103 high density (RB/RD)
     *   0x3BA00410 = STM32F103 XL density
     *
     * We also check DBGMCU_IDCODE register at 0xE0042000 to get
     * exact device ID, but that requires more advanced detection.
     */
    if ((idcode & 0xFFF00000) == 0x3BA00000) {
        uint32_t device_id = idcode & 0x00000FFF;
        switch (device_id) {
            case 0x412:  /* STM32F103 low density */
            case 0x410:  /* STM32F103 medium density */
                return TARGET_STM32F1_MD;
            case 0x414:  /* STM32F103 high density */
                return TARGET_STM32F1_HD;
            case 0x430:  /* GD32F103 extra-large density variants */
                return TARGET_GD32F1_XD;
            case 0x423:  /* STM32F401 low/medium density */
            case 0x433:  /* STM32F401 */
                return TARGET_STM32F4_256K;
            case 0x421:  /* STM32F446 */
            case 0x431:  /* STM32F411 */
                return TARGET_STM32F4_512K;
            case 0x413:  /* STM32F405/407/415/417 and compatible GD32F4 */
                return TARGET_STM32F4_1024K;
            case 0x419:  /* STM32F42x/43x */
            case 0x434:  /* STM32F469/479 */
                return TARGET_STM32F4_2048K;
            default:
                /* Try as medium density for any other STM32F1 variant */
                return TARGET_STM32F1_MD;
        }
    }
    return TARGET_UNKNOWN;
}

const program_target_t *flash_algo_get(target_chip_t chip) {
    switch (chip) {
        case TARGET_STM32F1_MD:
        case TARGET_STM32F1_HD:
        case TARGET_GD32F1_XD:
            return &stm32f1_flash_algo;
        case TARGET_STM32F4_256K:
            return &stm32f4_small_flash_algo;
        case TARGET_STM32F4_512K:
        case TARGET_STM32F4_1024K:
            return &stm32f407_flash_algo;
        case TARGET_STM32F4_2048K:
            return &stm32f439_flash_algo;
        default:
            return NULL;
    }
}

const target_cfg_t *flash_algo_get_target_config(target_chip_t chip) {
    switch (chip) {
        case TARGET_STM32F1_MD:
            return &stm32f1_md_target;
        case TARGET_STM32F1_HD:
            return &stm32f1_hd_target;
        case TARGET_GD32F1_XD:
            return &gd32f1_xd_target;
        case TARGET_STM32F4_256K:
            return &stm32f4_256k_target;
        case TARGET_STM32F4_512K:
            return &stm32f4_512k_target;
        case TARGET_STM32F4_1024K:
            return &stm32f4_1024k_target;
        case TARGET_STM32F4_2048K:
            return &stm32f4_2048k_target;
        default:
            return NULL;
    }
}

const char *flash_algo_get_name(target_chip_t chip) {
    switch (chip) {
        case TARGET_STM32F1_MD:
            return "STM32F103C8/CB (Medium Density)";
        case TARGET_STM32F1_HD:
            return "STM32F103RB/RD/RE (High Density)";
        case TARGET_GD32F1_XD:
            return "GD32F1 large-density compatible";
        case TARGET_STM32F4_256K:
            return "STM32F4/GD32F4 256KB class";
        case TARGET_STM32F4_512K:
            return "STM32F4/GD32F4 512KB class";
        case TARGET_STM32F4_1024K:
            return "STM32F4/GD32F4 1MB class";
        case TARGET_STM32F4_2048K:
            return "STM32F4/GD32F4 2MB class";
        default:
            return "Unknown";
    }
}
