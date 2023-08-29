/*
 * Copyright  2017-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "FreeRTOS.h"
#include "task.h"

#include "fsl_common.h"
#include "fsl_elcdif.h"
#include "fsl_debug_console.h"

#include "fsl_gpio.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#include <math.h>
#include "../helper/cube.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_ELCDIF LCDIF


#define APP_HSW        41
#define APP_HFP        4
#define APP_HBP        8
#define APP_VSW        10
#define APP_VFP        4
#define APP_VBP        2
#define APP_POL_FLAGS \
    (kELCDIF_DataEnableActiveHigh | kELCDIF_VsyncActiveLow | kELCDIF_HsyncActiveLow | kELCDIF_DriveDataOnRisingClkEdge)
/* additional definitions ie. colors */
#define BLUE		   0x000000FFU
#define RED			   0x00f44336U
#define PINK		   0x000fd01a4
#define YELLOW		   0x000fef400

/* Axis differentiate */
#define DIFF_X         110
#define DIFF_Y		   250
#define DIFF_Z		   0.5

/* Display. */
#define LCD_DISP_GPIO     GPIO1
#define LCD_DISP_GPIO_PIN 2
/* Back light. */
#define LCD_BL_GPIO     GPIO2
#define LCD_BL_GPIO_PIN 31

/* Frame buffer data alignment, for better performance, the LCDIF frame buffer should be 64B align. */
#define FRAME_BUFFER_ALIGN 64

#ifndef APP_LCDIF_DATA_BUS
#define APP_LCDIF_DATA_BUS kELCDIF_DataBus24Bit
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_EnableLcdInterrupt(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static volatile bool s_frameDone = false;

AT_NONCACHEABLE_SECTION_ALIGN(static uint32_t s_frameBuffer[2][APP_IMG_HEIGHT][APP_IMG_WIDTH], FRAME_BUFFER_ALIGN);

/*******************************************************************************
 * Code
 ******************************************************************************/
extern void APP_LCDIF_IRQHandler(void);

void LCDIF_IRQHandler(void)
{
    APP_LCDIF_IRQHandler();
    __DSB();
}

/* Enable interrupt. */
void BOARD_EnableLcdInterrupt(void)
{
    EnableIRQ(LCDIF_IRQn);
}

/* Initialize the LCD_DISP. */
void BOARD_InitLcd(void)
{
    gpio_pin_config_t config = {
        kGPIO_DigitalOutput,
        0,
        kGPIO_NoIntmode,
    };

    /* Backlight. */
    config.outputLogic = 1;
    GPIO_PinInit(LCD_BL_GPIO, LCD_BL_GPIO_PIN, &config);
}

void BOARD_InitLcdifPixelClock(void)
{
    /*
     * The desired output frame rate is 60Hz. So the pixel clock frequency is:
     * (480 + 41 + 4 + 18) * (272 + 10 + 4 + 2) * 60 = 9.2M.
     * Here set the LCDIF pixel clock to 9.3M.
     */

    /*
     * Initialize the Video PLL.
     * Video PLL output clock is OSC24M * (loopDivider + (denominator / numerator)) / postDivider = 93MHz.
     */
    clock_video_pll_config_t config = {
        .loopDivider = 31,
        .postDivider = 8,
        .numerator   = 0,
        .denominator = 0,
    };

    CLOCK_InitVideoPll(&config);

    /*
     * 000 derive clock from PLL2
     * 001 derive clock from PLL3 PFD3
     * 010 derive clock from PLL5
     * 011 derive clock from PLL2 PFD0
     * 100 derive clock from PLL2 PFD1
     * 101 derive clock from PLL3 PFD1
     */
    CLOCK_SetMux(kCLOCK_LcdifPreMux, 2);

    CLOCK_SetDiv(kCLOCK_LcdifPreDiv, 4);

    CLOCK_SetDiv(kCLOCK_LcdifDiv, 1);
}


void APP_LCDIF_IRQHandler(void)
{
    uint32_t intStatus;

    intStatus = ELCDIF_GetInterruptStatus(APP_ELCDIF);

    ELCDIF_ClearInterruptStatus(APP_ELCDIF, intStatus);

    if (intStatus & kELCDIF_CurFrameDone)
    {
        s_frameDone = true;
    }
    SDK_ISR_EXIT_BARRIER;
}

void APP_ELCDIF_Init(void)
{
    const elcdif_rgb_mode_config_t config = {
        .panelWidth    = APP_IMG_WIDTH,
        .panelHeight   = APP_IMG_HEIGHT,
        .hsw           = APP_HSW,
        .hfp           = APP_HFP,
        .hbp           = APP_HBP,
        .vsw           = APP_VSW,
        .vfp           = APP_VFP,
        .vbp           = APP_VBP,
        .polarityFlags = APP_POL_FLAGS,
        .bufferAddr    = (uint32_t)s_frameBuffer[0],
        .pixelFormat   = kELCDIF_PixelFormatXRGB8888,
        .dataBus       = APP_LCDIF_DATA_BUS,
    };

#if (defined(APP_ELCDIF_HAS_DISPLAY_INTERFACE) && APP_ELCDIF_HAS_DISPLAY_INTERFACE)
    BOARD_InitDisplayInterface();
#endif
    ELCDIF_RgbModeInit(APP_ELCDIF, &config);
}

uint32_t round(float num){
    return num < 0 ? (int)(num - 0.5) : (int)(num + 0.5);
}

void draw_line(float x1, float y1, float x2, float y2, uint32_t frameBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH], uint32_t pointBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH], uint32_t color){
	float dx = x2 - x1;
	float dy = y2 - y1;

	// Determine the number of steps to put pixel
	// old version: float steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    float distance = sqrt(dx*dx + dy*dy);
    float steps = distance * 10;

	// Calculate increment in x & y for each steps
	float Xinc = dx / steps;
	float Yinc = dy / steps;

	// Put pixel for each step
	float X = x1;
	float Y = y1;
	for (int i = 0; i <= steps; i++) {
		// frameBuffer[round(Y)][round(X)] = color;
        pointBuffer[round(Y)][round(X)] = color;
	    X += Xinc;           // increment in x at each step
	    Y += Yinc;           // increment in y at each step
	}

}

void APP_FillFrameBuffer(uint32_t frameBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH], float try)
{
    /* Background color. */
    static const uint32_t bgColor = 0U;
    /* Foreground color. */
    static uint8_t fgColorIndex          = 0U;
    static const uint32_t fgColorTable[] = {0x000000FFU, 0x0000FF00U, 0x0000FFFFU, 0x00FF0000U,
                                            0x00FF00FFU, 0x00FFFF00U, 0x00FFFFFFU};
    uint32_t fgColor                     = fgColorTable[fgColorIndex];
    uint32_t i, j;

    /* Background color. */
    for (i = 0; i < APP_IMG_HEIGHT; i++)
    {
        for (j = 0; j < APP_IMG_WIDTH; j++)
        {
            frameBuffer[i][j] = bgColor;
        }
    }

    /* #1 Create points */
    float pointTable[8][4] = {
        {0.0, 1.0, 0.0, 1.0}, {1.0, 1.0, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0}, {0.0, 0.0, 0.0, 1.0},
        {0.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 0.0, 1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}
    };

    /* #2 Create view matrix */
    Matrix4 myMat;
    identity(&myMat);

    Vector3 cameraAngle = initVector3(0.0, 0.0, 0.0);
    Vector3 cameraPosition = initVector3(-0.0, -0.0, 15.0);
    
    rotate(&myMat, cameraAngle.z, 0.0, 0.0, 1.0);      // roll
    rotate(&myMat, -cameraAngle.y, 0.0, 1.0, 0.0);     // heading
    rotate(&myMat, cameraAngle.x, 1.0, 0.0, 0.0);      // pitch
    translate(&myMat, -cameraPosition.x, -cameraPosition.y, -cameraPosition.z);

    /* #3 Create model matrix */
    Vector3 modelAngle = initVector3(0.0, 0.0, 0.0);
    Vector3 modelPosition = initVector3(-1.0 + try * 5, 0.0, 0.0);

    translate(&myMat, modelPosition.x, modelPosition.y, modelPosition.z);
    scaleMatrix_set(&myMat, 3.0, 3.0, 3.0);
    rotate(&myMat, modelAngle.x, 1.0, 0.0, 0.0);
    rotate(&myMat, -modelAngle.y, 0.0, 1.0, 0.0);
    rotate(&myMat, modelAngle.z, 0.0, 0.0, 1.0);

    /* #4 For all points, multi modelview matrix, pointTable->eye */
    for (int i = 0; i < 8; i++){
        multiplyVector4(myMat, pointTable[i]);
    }

    /* #5 Create perspective projection matrix */
    Matrix4 projectionMatrix;
    identity(&projectionMatrix);
    perspective(&projectionMatrix, -0.9, 0.9, -0.5, 0.5, 1.0, 10.0);
    
    /* #6 For all points, multi projectionMatrix, pointTable->clip */
    for (int i = 0; i < 8; i++){
        multiplyVector4(projectionMatrix, pointTable[i]);
    }

    /* #7 Normalize Device Coordinates (NDC) to Screen Coordinates */
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 3; j++){
            pointTable[i][j] = pointTable[i][j] / pointTable[i][3];
        }
    }

    /* #7 Window Coordinates */
    window_coordinates(pointTable, 0.0, 0.0, APP_IMG_WIDTH, APP_IMG_HEIGHT, 1.0, 10.0);

    /* TRY: copy of all points*/
	uint32_t pointBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH];
    for (int i = 0; i < APP_IMG_HEIGHT; i++) {
        for (int j = 0; j < APP_IMG_WIDTH; j++) {
            pointBuffer[i][j] = 0;
        }
    }

    for (int i = 0; i < 8; i++){
        // was pointBuffer[(int)pointTable[i][1]][(int)pointTable[i][0]] = RED;
        pointBuffer[round(pointTable[i][1])][round(pointTable[i][0])] = RED;
    }

    /* #8 DrawLine */
    draw_line(pointTable[0][0], pointTable[0][1], pointTable[1][0], pointTable[1][1], frameBuffer, pointBuffer, RED);    // back face
    draw_line(pointTable[1][0], pointTable[1][1], pointTable[2][0], pointTable[2][1], frameBuffer, pointBuffer, RED);
    draw_line(pointTable[2][0], pointTable[2][1], pointTable[3][0], pointTable[3][1], frameBuffer, pointBuffer, RED);
    draw_line(pointTable[3][0], pointTable[3][1], pointTable[0][0], pointTable[0][1], frameBuffer, pointBuffer, RED);

    draw_line(pointTable[4][0], pointTable[4][1], pointTable[5][0], pointTable[5][1], frameBuffer, pointBuffer, RED);    // front face
    draw_line(pointTable[5][0], pointTable[5][1], pointTable[6][0], pointTable[6][1], frameBuffer, pointBuffer, RED);
    draw_line(pointTable[6][0], pointTable[6][1], pointTable[7][0], pointTable[7][1], frameBuffer, pointBuffer, RED);
    draw_line(pointTable[7][0], pointTable[7][1], pointTable[4][0], pointTable[4][1], frameBuffer, pointBuffer, RED);

    draw_line(pointTable[0][0], pointTable[0][1], pointTable[4][0], pointTable[4][1], frameBuffer, pointBuffer, RED);    // connect edges
    draw_line(pointTable[1][0], pointTable[1][1], pointTable[5][0], pointTable[5][1], frameBuffer, pointBuffer, RED);
    draw_line(pointTable[3][0], pointTable[3][1], pointTable[7][0], pointTable[7][1], frameBuffer, pointBuffer, RED);
    draw_line(pointTable[2][0], pointTable[2][1], pointTable[6][0], pointTable[6][1], frameBuffer, pointBuffer, RED);

    /* #9 Render */
    for (int i = 0; i < APP_IMG_HEIGHT; i++){
        for (int j = 0; j < APP_IMG_WIDTH; j++){
            if (frameBuffer[i][j] != pointBuffer[i][j]){
                frameBuffer[i][j] = pointBuffer[i][j];
            }
        }
    }

}

void delay(){
    volatile int delayCount;

    for(delayCount = 0; delayCount < 500; delayCount++)
    {
        __asm volatile("nop");
    }
}

void refresh(uint32_t frameBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH]){
    /* Background color. */
    for (int i = 0; i < APP_IMG_HEIGHT; i++)
    {
        for (int j = 0; j < APP_IMG_WIDTH; j++)
        {
            frameBuffer[i][j] = 0U;
        }
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t frameBufferIndex = 0;

    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitSemcPins();
    BOARD_InitBootClocks();
    BOARD_InitLcdifPixelClock();
    BOARD_InitDebugConsole();
    BOARD_InitLcd();

    PRINTF("LCDIF RGB example start...\r\n");

    APP_ELCDIF_Init();

    BOARD_EnableLcdInterrupt();

    /* Clear the frame buffer. */
    memset(s_frameBuffer, 0, sizeof(s_frameBuffer));

    ELCDIF_EnableInterrupts(APP_ELCDIF, kELCDIF_CurFrameDoneInterruptEnable);
    ELCDIF_RgbModeStart(APP_ELCDIF);

    for (int i = 0; i < 7200; i+=6){
        frameBufferIndex ^= 1U;
        APP_FillFrameBuffer(s_frameBuffer[frameBufferIndex], cos(i * PI / 180.0f));
        // vTaskDelay(pdMS_TO_TICKS(500));
        // delay();
        ELCDIF_SetNextBufferAddr(APP_ELCDIF, (uint32_t)s_frameBuffer[frameBufferIndex]);
        
        /* Wait for previous frame complete. */
        s_frameDone = false;
        while (!s_frameDone)
        {
        }
        
    }

    // APP_FillFrameBuffer(s_frameBuffer[frameBufferIndex], 0.0);
    // ELCDIF_EnableInterrupts(APP_ELCDIF, kELCDIF_CurFrameDoneInterruptEnable);
    // ELCDIF_RgbModeStart(APP_ELCDIF);
    return 0;

}
