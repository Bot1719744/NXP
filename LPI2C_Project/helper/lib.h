#include "MIMXRT1062.h"

/*! @brief LPI2C pin configuration. */
typedef enum _lpi2c_master_pin_config
{
    kLPI2C_2PinOpenDrain  = 0x0U, /*!< LPI2C Configured for 2-pin open drain mode */
    kLPI2C_2PinOutputOnly = 0x1U, /*!< LPI2C Configured for 2-pin output only mode (ultra-fast mode) */
    kLPI2C_2PinPushPull   = 0x2U, /*!< LPI2C Configured for 2-pin push-pull mode */
    kLPI2C_4PinPushPull   = 0x3U, /*!< LPI2C Configured for 4-pin push-pull mode */
    kLPI2C_2PinOpenDrainWithSeparateSlave =
        0x4U, /*!< LPI2C Configured for 2-pin open drain mode with separate LPI2C slave */
    kLPI2C_2PinOutputOnlyWithSeparateSlave =
        0x5U, /*!< LPI2C Configured for 2-pin output only mode(ultra-fast mode) with separate LPI2C slave */
    kLPI2C_2PinPushPullWithSeparateSlave =
        0x6U, /*!< LPI2C Configured for 2-pin push-pull mode with separate LPI2C slave */
    kLPI2C_4PinPushPullWithInvertedOutput = 0x7U /*!< LPI2C Configured for 4-pin push-pull mode(inverted outputs) */
} lpi2c_master_pin_config_t;

/*! @brief LPI2C master host request selection. */
typedef enum _lpi2c_host_request_source
{
    kLPI2C_HostRequestExternalPin  = 0x0U, /*!< Select the LPI2C_HREQ pin as the host request input */
    kLPI2C_HostRequestInputTrigger = 0x1U, /*!< Select the input trigger as the host request input */
} lpi2c_host_request_source_t;

typedef enum _lpi2c_host_request_polarity
{
    kLPI2C_HostRequestPinActiveLow  = 0x0U, /*!< Configure the LPI2C_HREQ pin active low */
    kLPI2C_HostRequestPinActiveHigh = 0x1U  /*!< Configure the LPI2C_HREQ pin active high */
} lpi2c_host_request_polarity_t;

typedef struct _lpi2c_master_config
{
    bool enableMaster;                   /*!< Whether to enable master mode. */
    bool enableDoze;                     /*!< Whether master is enabled in doze mode. */
    bool debugEnable;                    /*!< Enable transfers to continue when halted in debug mode. */
    bool ignoreAck;                      /*!< Whether to ignore ACK/NACK. */
    lpi2c_master_pin_config_t pinConfig; /*!< The pin configuration option. */
    uint32_t baudRate_Hz;                /*!< Desired baud rate in Hertz. */
    uint32_t busIdleTimeout_ns;          /*!< Bus idle timeout in nanoseconds. Set to 0 to disable. */
    uint32_t pinLowTimeout_ns;           /*!< Pin low timeout in nanoseconds. Set to 0 to disable. */
    uint8_t sdaGlitchFilterWidth_ns;     /*!< Width in nanoseconds of glitch filter on SDA pin. Set to 0 to disable. */
    uint8_t sclGlitchFilterWidth_ns;     /*!< Width in nanoseconds of glitch filter on SCL pin. Set to 0 to disable. */
    struct
    {
        bool enable;                            /*!< Enable host request. */
        lpi2c_host_request_source_t source;     /*!< Host request source. */
        lpi2c_host_request_polarity_t polarity; /*!< Host request pin polarity. */
    } hostRequest;                              /*!< Host request options. */
} lpi2c_master_config_t;


/*******************************************************************************
 * Functions
 ******************************************************************************/

void LPI2C_MasterGetDefaultConfig(lpi2c_master_config_t *masterConfig){

}

void LPI2C_MasterInit(context->base, &config, I2C_MASTER_CLOCK_FREQUENCY){

}

static UINT _tx_tranfer_i2c(tx_i2c_master_context_t *context, tx_i2c_request_t *request, lpi2c_direction_t dir){
    
}
