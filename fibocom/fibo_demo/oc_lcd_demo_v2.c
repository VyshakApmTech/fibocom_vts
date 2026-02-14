/**
 * @file oc_lcd_demo_v2.c
 * @author Ethan Guo (Guo Junhuang) (ethan.guo@fibocom.com)
 * @brief LCD demo V2 of LCD full function test.
 * @version 0.1
 * @date 2025-07-10
 * 
 * @note 1. This demo is a full function test, Including tests for drawing dots, lines, filling rectangular 
 *          colors, transmitting images, entering sleep mode, and retiring to sleep mode, etc.
 *       2. This demo is including mornal LCD function test and multiple LCD device function test.
 *          * Mormal LCD function: Call fibo_lcd_init(), fibo_lcd_FrameTransfer(), etc, only to drive 
 *                                 only one LCD screen.
 *          * Multiple LCD device function: fibo_lcd_reg_multi_device(), fibo_lcd_unreg_multi_device() 
 *                                          and fibo_lcd_switch_mulit_device() are used for multiple LCD  
 *                                          devices function. Enable this function by calling these APIs.
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "osi_api.h"
#include "osi_log.h"
#include "app_image.h"
#include "fibo_opencpu.h"

static void sig_res_callback(GAPP_SIGNAL_ID_T sig, va_list arg)
{
    fibo_textTrace("sig_res_callback  sig = %d", sig);
}

static void at_res_callback(UINT8 *buf, UINT16 len)
{
    fibo_textTrace("FIBO <--%s", buf);
}

static FIBO_CALLBACK_T user_callback = {
    .fibo_signal = sig_res_callback,
    .at_resp = at_res_callback
};

#define _THREAD_SLEEP_TIME_SHORT            (30)
#define _THREAD_SLEEP_TIME_MID              (100)
#define _THREAD_SLEEP_TIME_LONG             (500)

#define _PIXEL_TEST_TIME                    (100)
#define _RECT_TEST_TIME                     (10)
#define _RECT_BORDER_SIZE                   (10)
#define _UNIT_TEST_LOOP_TIME                (10)

#ifdef CONFIG_FIBOCOM_MULTI_LCD_SUPPORT // FIBOCOM multiple LCD device function
#define _LCD_REG_MULTI_DEVICE(name, cfg)    fibo_lcd_reg_multi_device(name, cfg)
#define _LCD_UNREG_MULTI_DEVICE(name)       fibo_lcd_unreg_multi_device(name)
#define _LCD_SWITCH_MULTI_DEVICE(name)      fibo_lcd_switch_multi_device(name)

#define _MULTI_LCD_DEVICE_0                 "multi_lcd_0"
#define _MULTI_LCD_DEVICE_1                 "multi_lcd_1"

typedef struct
{
    char name[20];
    multi_lcd_cfg_t cfg;
} app_lcd_device_t;

app_lcd_device_t g_lcd_device_0 = {
    .name = _MULTI_LCD_DEVICE_0,
    .cfg = {
        .csx_pin = 68,
        .csx_pol = 1,
        .rst_pin = 49, // allow used same rstb_pin
        .rst_pol = 1,
    },
};

app_lcd_device_t g_lcd_device_1 = {
    .name = _MULTI_LCD_DEVICE_1,
    .cfg = {
        .csx_pin = 69,
        .csx_pol = 1,
        .rst_pin = 49, // allow used same rstb_pin
        .rst_pol = 1,
    },
};
#else // mornal function, call nothing
#define _LCD_REG_MULTI_DEVICE(name, cfg)
#define _LCD_UNREG_MULTI_DEVICE(name)
#define _LCD_SWITCH_MULTI_DEVICE(name)
#endif

static void prv_thread_lcdtest(void *param)
{
    int32_t ret = -1;
    uint32_t lcd_devid = 0;
    uint32_t lcd_width = 0;
    uint32_t lcd_height = 0;
    lcdDisplay_t startPoint = {0};
    lcdFrameBuffer_t dataBufferWin = {0};
    uint16_t *pBuff = NULL;

    fibo_textTrace("GJH: application thread enter, param: 0x%p", param);

#ifdef CONFIG_FIBOCOM_MULTI_LCD_SUPPORT // FIBOCOM multiple LCD device function
    char *lcd_dev_name = ((app_lcd_device_t *)param)->name; // get lcd device name
    multi_lcd_cfg_t lcd_dev_cfg = ((app_lcd_device_t *)param)->cfg; // get lcd device configuration

    fibo_textTrace("GJH: lcd dev_name=%s, csx=%d, rst=%d", lcd_dev_name, lcd_dev_cfg.csx_pin, lcd_dev_cfg.rst_pin);
#else // mornal function, call nothing
    __attribute__((unused)) char *lcd_dev_name = NULL; // un-used
    __attribute__((unused)) multi_lcd_cfg_t lcd_dev_cfg = {0}; // un-used
#endif

    for (;;)
    {
        _LCD_REG_MULTI_DEVICE(lcd_dev_name, lcd_dev_cfg);
        // lcd init
        do
        {
            fibo_textTrace("GJH: [lcd init] start...");
            _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
            ret = fibo_lcd_init();
            if ((ret != DRV_LCD_SUCCESS) && (ret != DRV_LCD_HAS_INITED))
            {
                fibo_textTrace("GJH: err: fibo_lcd_init=%d.", ret);
                goto error;
            }

            _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
            ret = fibo_lcd_Getinfo(&lcd_devid, &lcd_width, &lcd_height);
            if (ret != 0)
            {
                fibo_textTrace("GJH: err: fibo_lcd_Getinfo=%d.", ret);
                goto error;
            }

            fibo_textTrace("GJH: lcd_init success.");
            fibo_textTrace("GJH: lcd devid: %d, lcd width: %d, lcd heigth: %d", lcd_devid, lcd_width, lcd_height);

            _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
            ret = fibo_lcd_SetBrushDirection(LCD_DIRECT_NORMAL);
            if (ret != 0)
            {
                fibo_textTrace("GJH: err: fibo_lcd_Getinfo=%d.", ret);
                goto error;
            }

            startPoint.x = 0;
            startPoint.y = 0;
            startPoint.height = lcd_height;
            startPoint.width = lcd_width;
            _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
            fibo_lcd_FillRect(&startPoint, BLACK);
            fibo_taskSleep(_THREAD_SLEEP_TIME_SHORT);

            if (pBuff != NULL)
            {
                free(pBuff);
                pBuff = NULL;
            }

            pBuff = malloc(lcd_width * lcd_height * sizeof(uint16_t));
            if (pBuff == NULL)
            {
                fibo_textTrace("GJH: err: fibo_lcd_Getinfo=%d.", ret);
                goto error;
            }
            fibo_textTrace("GJH: [lcd init] end");
        } while (0);

        // test sending buffer
        do
        {
            fibo_textTrace("GJH: [test sending buffer] start...");
            startPoint.x = 0;
            startPoint.y = 0;
            startPoint.height = lcd_height;
            startPoint.width = lcd_width;

            dataBufferWin.buffer = pBuff;
            dataBufferWin.colorFormat = LCD_RESOLUTION_RGB565;
            dataBufferWin.keyMaskEnable = false;
            dataBufferWin.region_x = 0;
            dataBufferWin.region_y = 0;
            dataBufferWin.height = lcd_height;
            dataBufferWin.width = lcd_width;
            dataBufferWin.widthOriginal = lcd_width;
            dataBufferWin.rotation = 0;

            for (uint32_t i = 0; i < _UNIT_TEST_LOOP_TIME; i++)
            {
                for (uint32_t w = 0; w < lcd_width; w++)
                {
                    for (uint32_t h = 0; h < lcd_height; h++)
                    {
                        pBuff[w + h * lcd_width] = RED;
                    }
                }
                _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                fibo_lcd_FrameTransfer(&dataBufferWin, &startPoint);
                fibo_taskSleep(_THREAD_SLEEP_TIME_SHORT);

                for (uint32_t w = 0; w < lcd_width; w++)
                {
                    for (uint32_t h = 0; h < lcd_height; h++)
                    {
                        pBuff[w + h * lcd_width] = GREEN;
                    }
                }
                _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                fibo_lcd_FrameTransfer(&dataBufferWin, &startPoint);
                fibo_taskSleep(_THREAD_SLEEP_TIME_SHORT);

                for (uint32_t w = 0; w < lcd_width; w++)
                {
                    for (uint32_t h = 0; h < lcd_height; h++)
                    {
                        pBuff[w + h * lcd_width] = BLUE;
                    }
                }
                _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                fibo_lcd_FrameTransfer(&dataBufferWin, &startPoint);
                fibo_taskSleep(_THREAD_SLEEP_TIME_SHORT);
            }
            fibo_textTrace("GJH: [test sending buffer] end");
        } while (0);

        // test drawing square and observing burst direction
        do
        {
            fibo_textTrace("GJH: [test drawing square and observing burst direction] start...");
            // clear background as black
            do
            {
                startPoint.x = 0;
                startPoint.y = 0;
                startPoint.width = lcd_width;
                startPoint.height = lcd_height;
                _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                fibo_lcd_FillRect(&startPoint, BLACK);
                fibo_taskSleep(_THREAD_SLEEP_TIME_SHORT);
            } while (0);

            startPoint.x = 0;
            startPoint.y = 0;
            startPoint.width = _RECT_BORDER_SIZE;
            startPoint.height = _RECT_BORDER_SIZE;

            for (uint32_t i = 0; i < _RECT_TEST_TIME; i += 3)
            {
                startPoint.x = _RECT_BORDER_SIZE * (i + 0);
                startPoint.y = _RECT_BORDER_SIZE * (i + 0);
                _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                fibo_lcd_FillRect(&startPoint, YELLOW);
                fibo_taskSleep(_THREAD_SLEEP_TIME_MID);

                startPoint.x = _RECT_BORDER_SIZE * (i + 1);
                startPoint.y = _RECT_BORDER_SIZE * (i + 1);
                _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                fibo_lcd_FillRect(&startPoint, BLUE);
                fibo_taskSleep(_THREAD_SLEEP_TIME_MID);

                startPoint.x = _RECT_BORDER_SIZE * (i + 2);
                startPoint.y = _RECT_BORDER_SIZE * (i + 2);
                _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                fibo_lcd_FillRect(&startPoint, RED);
                fibo_taskSleep(_THREAD_SLEEP_TIME_MID);
            }
            fibo_textTrace("GJH: [test drawing square and observing burst direction] end");
        } while (0);

        // testing drawing pixel and line
        do
        {
            fibo_textTrace("GJH: [testing drawing pixel and line] start...");
            startPoint.x = 0;
            startPoint.y = 0;
            startPoint.width = lcd_width;
            startPoint.height = lcd_height;
            _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
            fibo_lcd_FillRect(&startPoint, BLACK);

            for (uint32_t i = 0; i < _PIXEL_TEST_TIME; i++)
            {
                _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                fibo_lcd_SetPixel(i, i, GREEN);
                fibo_taskSleep(_THREAD_SLEEP_TIME_SHORT);
            }

            _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
            fibo_lcd_DrawLine(0, 0, lcd_width - 1, lcd_height - 1, WHITE);
            fibo_taskSleep(_THREAD_SLEEP_TIME_LONG);

            _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
            fibo_lcd_DrawLine(0, lcd_height - 1, lcd_width - 1, 0, YELLOW);
            fibo_taskSleep(_THREAD_SLEEP_TIME_LONG);
            fibo_textTrace("GJH: [testing drawing pixel and line] end");
        } while (0);

        // sleep test
        do
        {
            fibo_textTrace("GJH: [sleep test] start...");
            
            // Rotate the screen by 90 degrees
            _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
            fibo_lcd_SetBrushDirection(LCD_DIRECT_ROT_90);  

            // sleep test
            for (uint32_t i = 0; i < _UNIT_TEST_LOOP_TIME; i++)
            {
                _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                fibo_lcd_Sleep(true);
                fibo_taskSleep(_THREAD_SLEEP_TIME_LONG);

                _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                fibo_lcd_Sleep(false);
                fibo_taskSleep(_THREAD_SLEEP_TIME_LONG);

                // clear background as black
                do
                {
                    startPoint.x = 0;
                    startPoint.y = 0;
                    startPoint.width = lcd_height; // rotate 90 degrees
                    startPoint.height = lcd_width;
                    _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                    fibo_lcd_FillRect(&startPoint, BLACK);
                } while (0);

                // test drawing square and observing burst direction
                do
                {
                    startPoint.x = 0;
                    startPoint.y = 0;
                    startPoint.width = _RECT_BORDER_SIZE;
                    startPoint.height = _RECT_BORDER_SIZE;

                    for (uint32_t i = 0; i < _RECT_TEST_TIME; i += 3)
                    {
                        startPoint.x = _RECT_BORDER_SIZE * (i + 0);
                        startPoint.y = _RECT_BORDER_SIZE * (i + 0);
                        _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                        fibo_lcd_FillRect(&startPoint, YELLOW);
                        fibo_taskSleep(_THREAD_SLEEP_TIME_MID);

                        startPoint.x = _RECT_BORDER_SIZE * (i + 1);
                        startPoint.y = _RECT_BORDER_SIZE * (i + 1);
                        _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                        fibo_lcd_FillRect(&startPoint, BLUE);
                        fibo_taskSleep(_THREAD_SLEEP_TIME_MID);

                        startPoint.x = _RECT_BORDER_SIZE * (i + 2);
                        startPoint.y = _RECT_BORDER_SIZE * (i + 2);
                        _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
                        fibo_lcd_FillRect(&startPoint, RED);
                        fibo_taskSleep(_THREAD_SLEEP_TIME_MID);
                    }
                } while (0);
            }
            fibo_textTrace("GJH: [sleep test] end");
        } while (0);

        // lcd deinit
        do
        {
            fibo_textTrace("GJH: [lcd deinit] start...");
            _LCD_SWITCH_MULTI_DEVICE(lcd_dev_name);
            fibo_lcd_deinit();
            fibo_textTrace("GJH: [lcd deinit] end");
        } while (0);
        fibo_taskSleep(_THREAD_SLEEP_TIME_LONG);

        _LCD_UNREG_MULTI_DEVICE(lcd_dev_name);
    }

error:
    _LCD_UNREG_MULTI_DEVICE(lcd_dev_name);
    fibo_lcd_deinit();
    fibo_thread_delete();
}

static void prv_lcd_hardware_prepare_in_adp(void)
{
// all below are only for ADP test
#ifdef CONFIG_SOC_8850
#ifdef CONFIG_FIBOCOM_MC669
    fibo_gpio_driving_set(62, 6);   // lcd te pin set driving strength level
    fibo_gpio_driving_set(63, 6);   // lcd sdc pin set driving strength level
    fibo_gpio_driving_set(64, 6);   // lcd rstb pin set driving strength level
    fibo_gpio_driving_set(65, 6);   // lcd cs pin set driving strength level
    fibo_gpio_driving_set(66, 6);   // lcd sio pin set driving strength level
    fibo_gpio_driving_set(67, 6);   // lcd clk pin set driving strength level
#elif defined(CONFIG_FIBOCOM_MC665)
    // fibo_gpio_driving_set(-1, 8); // has no te pin
    fibo_gpio_driving_set(62, 6); // lcd sdc pin set driving strength level
    fibo_gpio_driving_set(74, 6); // lcd rstb pin set driving strength level
    fibo_gpio_driving_set(69, 6); // lcd cs pin set driving strength level
    fibo_gpio_driving_set(58, 6); // lcd sio pin set driving strength level
    fibo_gpio_driving_set(54, 6); // lcd clk pin set driving strength level
#elif defined(CONFIG_FIBOCOM_MC661)
    fibo_gpio_driving_set(78, 6); // lcd te pin set driving strength level
    fibo_gpio_driving_set(51, 6); // lcd sdc pin set driving strength level
    fibo_gpio_driving_set(49, 6); // lcd rstb pin set driving strength level
    fibo_gpio_driving_set(52, 6); // lcd cs pin set driving strength level
    fibo_gpio_driving_set(50, 6); // lcd sio pin set driving strength level
    fibo_gpio_driving_set(53, 6); // lcd clk pin set driving strength level
#endif

    pmu_cfg_t cfg = {
        .enable = true,
        .lp_enable = true,
        .level = 2800,
    };
    fibo_pmu_set_cfg(HAL_POWER_KEYLED, cfg); // ADP LCD IC power supply.

    // It is recommended to turn on backlight after the LCD is initialized and the first frame of the image has been transfer.
    // The approach used here is only for testing.
    fibo_sink_OnOff(1, true);
    fibo_sinkLevel_Set(1, 200);
#endif
}

static void prv_thread_lcd_app_init()
{
    // Only for use on the ADP board. Users need to re-adapt the hardware environment according to their own board.
    prv_lcd_hardware_prepare_in_adp();

#ifdef CONFIG_FIBOCOM_MULTI_LCD_SUPPORT
    fibo_thread_create(prv_thread_lcdtest, "lcdtest0", 100 * 1024, &g_lcd_device_0, OSI_PRIORITY_NORMAL);
    fibo_thread_create(prv_thread_lcdtest, "lcdtest1", 100 * 1024, &g_lcd_device_1, OSI_PRIORITY_NORMAL);
#else
    fibo_thread_create(prv_thread_lcdtest, "lcdtest", 100 * 1024, NULL, OSI_PRIORITY_NORMAL);
#endif

    fibo_thread_delete();
}

void *appimg_enter(void *param)
{
    fibo_textTrace("GJH: application image enter, param 0x%x", param);

    fibo_thread_create(prv_thread_lcd_app_init, "lcdinit", 10 * 1024, NULL, OSI_PRIORITY_NORMAL);
    return (void *)&user_callback;
}

void appimg_exit(void)
{
    fibo_textTrace("GJH: application image exit");
}
