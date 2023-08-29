/*
 * Copyright  2017-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_elcdif.h"
#include "fsl_debug_console.h"

#include "fsl_gpio.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_ELCDIF LCDIF

#define APP_IMG_HEIGHT 272
#define APP_IMG_WIDTH  480
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
#define DIFF_Z		   0.4

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

/* Global Variables for movements and axis difference*/
int movement_X = 0;
int movement_Y = 0;
int movement_Z = 0;

int axisDIFF = 1;

uint32_t pointBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH] = {0};

/*******************************************************************************
 * Structs
 ******************************************************************************/
struct Cube {
	uint32_t pointTable[8][3];
	uint32_t edges[12][2];
	uint32_t faces[6][4];
};

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

void draw_line(float x1, float y1, float x2, float y2, uint32_t frameBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH], uint32_t color, char type[]){
	float dx = x2 - x1;
	float dy = y2 - y1;

	// Determine the number of steps to put pixel
	float steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

	// Calculate increment in x & y for each steps
	float Xinc = dx / steps;
	float Yinc = dy / steps;

	// Temp positions
	uint32_t pointBuffer_temp[APP_IMG_HEIGHT][APP_IMG_WIDTH] = {0};

	// Put pixel for each step
	float X = x1;
	float Y = y1;
	for (int i = 0; i <= steps; i++) {
		if (type == "axis"){
			frameBuffer[round(X)][round(Y)] = color;
		}
		if (type == "cube"){
			pointBuffer[round(X)][round(Y)] = color;
			frameBuffer[round(X)][round(Y)] = color;
		}
		if (type == "move"){
			pointBuffer_temp[round(X)][round(Y)] = color;
		}
	    X += Xinc;           // increment in x at each step
	    Y += Yinc;           // increment in y at each step
	}

}


/* GOAL #1: Axis */
void axis_setup(uint32_t frameBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH], uint32_t color){
	draw_line(110.0, 250.0, 110.0, 50.0, frameBuffer, color, "axis");	/* X axis */
	draw_line(110.0, 50.0, 115.0, 55.0, frameBuffer, color, "axis");	/* X axis upper arrow */
	draw_line(110.0, 50.0, 105.0, 55.0, frameBuffer, color, "axis");	/* X axis lower arrow */
	draw_line(110.0, 250.0, 110.0, 430.0, frameBuffer, color, "axis");	/* X axis negative */
	draw_line(110.0, 430.0, 115.0, 425.0, frameBuffer, color, "axis");	/* X axis negative upper arrow */
	draw_line(110.0, 430.0, 105.0, 425.0, frameBuffer, color, "axis");	/* X axis negative lower arrow */
	draw_line(115.0, 45.0, 105.0, 35.0, frameBuffer, color, "axis");	/* X left */
	draw_line(115.0, 35.0, 105.0, 45.0, frameBuffer, color, "axis");	/* X right */

	draw_line(110.0, 250.0, 260.0, 250.0, frameBuffer, color, "axis");	/* Y axis */
	draw_line(260.0, 250.0, 255.0, 245.0, frameBuffer, color, "axis");	/* Y axis right arrow */
	draw_line(260.0, 250.0, 255.0, 255.0, frameBuffer, color, "axis");	/* Y axis left arrow */
	draw_line(110.0, 250.0, 10.0, 250.0, frameBuffer, color, "axis");	/* Y axis negative */
	draw_line(10.0, 250.0, 15.0, 245.0, frameBuffer, color, "axis");	/* Y axis negative right arrow */
	draw_line(10.0, 250.0, 15.0, 255.0, frameBuffer, color, "axis");	/* Y axis negative left arrow */
	draw_line(260.0, 240.0, 255.0, 235.0, frameBuffer, color, "axis");	/* Y axis left */
	draw_line(260.0, 230.0, 250.0, 240.0, frameBuffer, color, "axis");	/* Y axis right */

	draw_line(110.0, 250.0, 20.0, 340.0, frameBuffer, color, "axis");	/* Z axis */
	draw_line(20.0, 340.0, 22.0, 332.0, frameBuffer, color, "axis");	/* Z axis right arrow */
	draw_line(20.0, 340.0, 28.0, 338.0, frameBuffer, color, "axis");	/* Z axis left arrow */
	draw_line(110.0, 250.0, 190.0, 170.0, frameBuffer, color, "axis");	/* Z axis negative */
	draw_line(190.0, 170.0, 183.0, 171.0, frameBuffer, color, "axis");	/* Z axis negative right arrow */
	draw_line(190.0, 170.0, 188.0, 180.0, frameBuffer, color, "axis");	/* Z axis negative left arrow */
	draw_line(20.0, 350.0, 10.0, 360.0, frameBuffer, color, "axis");	/* Z middle */
	draw_line(20.0, 350.0, 20.0, 360.0, frameBuffer, color, "axis");	/* Z up */
	draw_line(10.0, 350.0, 10.0, 360.0, frameBuffer, color, "axis");	/* Z down */
}

/* helper function: transform 3D coordinates to 2D pixel coordinates */
void Object3D_to_2D(int length, uint32_t source[length][3], uint32_t (*resultTable)[2]){

	for (int i = 0; i < length; i+=1){
		int a = ( source[i][0] + movement_X ) * axisDIFF;
		int b = ( source[i][1] + movement_Y ) * axisDIFF;
		int c = ( source[i][2] + movement_Z ) * axisDIFF;

		resultTable[i][0] = round(DIFF_X+b-c*DIFF_Z);
		resultTable[i][1] = round(DIFF_Y-a+c*DIFF_Z);
		// PRINTF("X: %d, Y: %d\n", resultTable[i][0], resultTable[i][1]);
	}
	return;
}

void fill_face(struct Cube *cube, char side[], uint32_t color, uint32_t frameBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH]){
	uint32_t resultTable[8][2] = {0};

	/* #1 Convert coordinates */
	Object3D_to_2D(8, cube->pointTable, resultTable);
	// PRINTF("40: %d, 60: %d, 41: %d, 71: %d\n", resultTable[4][0], resultTable[6][0], resultTable[4][1], resultTable[7][1]);

	/* #2 Fill front face */
	for (int i = resultTable[6][0]; i < resultTable[4][0]; i++){
		for (int j = resultTable[7][1]; j < resultTable[6][1]; j++){
			// PRINTF("x: %d, y: %d\n", i, j);
			frameBuffer[i][j] = PINK;
		}
	}

	/* Try fill top face */
	PRINTF("01: %d, 11: %d, 01: %d, 41: %d\n", resultTable[0][1], resultTable[1][1], resultTable[0][0], resultTable[4][0]);
	for (int i = 0; i < resultTable[0][1] - resultTable[1][1]; i++){
		PRINTF("x1: %d, y1: %d, x2: %d, y2: %d\n", resultTable[4][0], resultTable[4][1] - i, resultTable[0][0], resultTable[0][1] - i);
		draw_line(resultTable[4][0], resultTable[4][1] - i, resultTable[0][0], resultTable[0][1] - i, frameBuffer, YELLOW, "cube");
	}

	/* Try fill right face */
	PRINTF("70: %d, 50: %d, 01: %d, 41: %d\n", resultTable[7][0], resultTable[5][0], resultTable[0][0], resultTable[4][0]);
	for (int i = 0; i < resultTable[5][0] - resultTable[7][0]; i++){
		PRINTF("x1: %d, y1: %d, x2: %d, y2: %d\n", resultTable[7][0], resultTable[7][1] - i, resultTable[5][0], resultTable[5][1] - i);
		draw_line(resultTable[5][0] - i, resultTable[5][1], resultTable[1][0] - i, resultTable[1][1], frameBuffer, color, "cube");
	}


}


void draw_cube(struct Cube* cube, uint32_t frameBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH]){
	uint32_t resultTable[8][2] = {0};

	/* #1 Convert coordinates */
	Object3D_to_2D(8, cube->pointTable, resultTable);

	/* #2 Draw all edges */
	for (int i = 0; i < 12; i++){
		uint32_t x1 = resultTable[cube->edges[i][0]][0];
		uint32_t y1 = resultTable[cube->edges[i][0]][1];
		uint32_t x2 = resultTable[cube->edges[i][1]][0];
		uint32_t y2 = resultTable[cube->edges[i][1]][1];
		// PRINTF("x1: %d, y1: %d, x2: %d, y2: %d\n", x1, y1, x2, y2);
		draw_line(x1, y1, x2, y2, frameBuffer, RED, "cube");
	}

}

void move_cube(struct Cube* cube, uint32_t length[3], uint32_t frameBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH]){

//	/* erase the old one */
//	for (int i = 0; i < APP_IMG_HEIGHT; i++){
//		for (int j = 0; j < APP_IMG_WIDTH; j++){
//			frameBuffer[i][j] =0U;
//		}
//	}

	/* setup axis again */
	// axis_setup(frameBuffer, BLUE);

	/* #1 Move back first */
	for (int i = 0; i < 4; i++){
		cube->pointTable[i][0] = cube->pointTable[i][0] + length[0];
		cube->pointTable[i][1] = cube->pointTable[i][1] + length[1];
		cube->pointTable[i][2] = cube->pointTable[i][2] + length[2];
	}

	/* #2 Move front then, move more */
	for (int i = 4; i < 8; i++){
		length[0] *= 1.2;
		cube->pointTable[i][0] = cube->pointTable[i][0] + length[0];
		cube->pointTable[i][1] = cube->pointTable[i][1] + length[1]*1.2;
		cube->pointTable[i][2] = cube->pointTable[i][2] + length[2];
	}

	draw_cube(cube, frameBuffer);
}


void APP_FillFrameBuffer(uint32_t frameBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH])
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

    /* #1 Setup XYZ axis */
    axis_setup(frameBuffer, BLUE);

    /* #2 Get coordinates */
    uint32_t pointTable[8][3] = {{0, 50, 0}, {50, 50, 0}, {0, 0, 0}, {50, 0, 0}, {0, 50, 50}, {50, 50, 50}, {0, 0, 50}, {50, 0, 50}};
    uint32_t edges[12][2] = {{0, 1}, {1, 3}, {3, 2}, {2, 0}, {0, 4}, {1, 5}, {3, 7}, {2, 6}, {4, 5}, {5, 7}, {7, 6}, {6, 4}};
    uint32_t faces[6][4] = {{0, 1, 3, 2}, {2, 3, 7, 6}, {4, 0, 2, 6}, {5, 1, 3, 7}, {0, 1, 3, 4}, {4, 5, 7, 6}};

    /* #3 Declare Cube struct */
    struct Cube cube;
    memcpy(cube.pointTable, pointTable, sizeof(cube.pointTable));
    memcpy(cube.edges, edges, sizeof(cube.edges));

    /* #4 draw cube */
    draw_cube(&cube, frameBuffer);
    fill_face(&cube, NULL, RED, frameBuffer);

    /* #5 move cube */
    uint32_t length[3] = {-90, 0, 0};
    move_cube(&cube, length, frameBuffer);
    fill_face(&cube, NULL, RED, frameBuffer);

//    uint32_t length1[3] = {-90, 0, 0};
//    move_cube(&cube, length1, frameBuffer);
//    fill_face(&cube, NULL, RED, frameBuffer);

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

    APP_FillFrameBuffer(s_frameBuffer[frameBufferIndex]);

    ELCDIF_EnableInterrupts(APP_ELCDIF, kELCDIF_CurFrameDoneInterruptEnable);
    ELCDIF_RgbModeStart(APP_ELCDIF);

    return 0;

}
