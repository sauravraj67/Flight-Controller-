
/*
 * main.c
 * STM32F401CBU6 Flight Controller Starter
 * IDE: STM32CubeIDE + STM32 HAL
 *
 * Features:
 * - MPU6050 raw sensor readings
 * - Four-channel PWM receiver capture
 * - Basic roll/pitch angle estimation
 * - Receiver-loss detection
 * - Motors intentionally disabled
 *
 * BEFORE BUILDING:
 * Configure I2C1 and TIM2 input capture in CubeMX.
 * TIM2 must run at 1 MHz for this example.
 * Verify all pins against your PCB schematic.
 */

#include "main.h"
#include <stdint.h>
#include <string.h>
#include <math.h>

/* CubeMX-generated peripheral handles */
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim2;

/* MPU6050, AD0 low */
#define MPU_ADDR       (0x68U << 1)
#define MPU_WHO_AM_I   0x75U
#define MPU_PWR_MGMT_1 0x6BU
#define MPU_DATA_REG   0x3BU

#define RC_CHANNELS    4
#define RC_MIN_US      900U
#define RC_MAX_US      2100U
#define RC_TIMEOUT_MS  100U

typedef struct {
    int16_t ax, ay, az;
    int16_t temperature;
    int16_t gx, gy, gz;
} IMU_Data;

typedef struct {
    volatile uint16_t rise;
    volatile uint16_t pulse;
    volatile uint32_t last_ms;
    volatile uint8_t falling;
    volatile uint8_t valid;
} RC_Input;

static IMU_Data imu;
static RC_Input rc[RC_CHANNELS];

static uint8_t imu_ok = 0;
static uint8_t armed = 0; /* Intentionally never armed */

static float roll_deg = 0.0f;
static float pitch_deg = 0.0f;
static uint32_t previous_loop_ms = 0;

/* Timer channel mapping: requires TIM2 CH1-CH4 configuration. */
static const uint32_t rc_timer_channels[4] = {
    TIM_CHANNEL_1, TIM_CHANNEL_2,
    TIM_CHANNEL_3, TIM_CHANNEL_4
};

/* ---------------- MPU6050 ---------------- */

static HAL_StatusTypeDef MPU_Init(void)
{
    uint8_t id = 0;
    uint8_t value = 0;

    if (HAL_I2C_Mem_Read(&hi2c1, MPU_ADDR, MPU_WHO_AM_I,
        I2C_MEMADD_SIZE_8BIT, &id, 1, 100) != HAL_OK)
        return HAL_ERROR;

    if (id != 0x68U)
        return HAL_ERROR;

    /* Wake up the sensor */
    if (HAL_I2C_Mem_Write(&hi2c1, MPU_ADDR, MPU_PWR_MGMT_1,
        I2C_MEMADD_SIZE_8BIT, &value, 1, 100) != HAL_OK)
        return HAL_ERROR;

    /* Gyroscope range: +/-250 degrees/s */
    value = 0;
    if (HAL_I2C_Mem_Write(&hi2c1, MPU_ADDR, 0x1B,
        I2C_MEMADD_SIZE_8BIT, &value, 1, 100) != HAL_OK)
        return HAL_ERROR;

    /* Accelerometer range: +/-2 g */
    if (HAL_I2C_Mem_Write(&hi2c1, MPU_ADDR, 0x1C,
        I2C_MEMADD_SIZE_8BIT, &value, 1, 100) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(100);
    return HAL_OK;
}

static HAL_StatusTypeDef MPU_Read(IMU_Data *d)
{
    uint8_t b[14];

    if (HAL_I2C_Mem_Read(&hi2c1, MPU_ADDR, MPU_DATA_REG,
        I2C_MEMADD_SIZE_8BIT, b, 14, 20) != HAL_OK)
        return HAL_ERROR;

    d->ax = (int16_t)((b[0] << 8) | b[1]);
    d->ay = (int16_t)((b[2] << 8) | b[3]);
    d->az = (int16_t)((b[4] << 8) | b[5]);
    d->temperature = (int16_t)((b[6] << 8) | b[7]);
    d->gx = (int16_t)((b[8] << 8) | b[9]);
    d->gy = (int16_t)((b[10] << 8) | b[11]);
    d->gz = (int16_t)((b[12] << 8) | b[13]);

    return HAL_OK;
}

/* ---------------- RC PWM capture ---------------- */

/*
 * Call this function from the one HAL_TIM_IC_CaptureCallback()
 * definition in main.c.
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance != TIM2)
        return;

    uint32_t ch;
    uint8_t i;

    switch (htim->Channel) {
    case HAL_TIM_ACTIVE_CHANNEL_1:
        ch = TIM_CHANNEL_1; i = 0; break;
    case HAL_TIM_ACTIVE_CHANNEL_2:
        ch = TIM_CHANNEL_2; i = 1; break;
    case HAL_TIM_ACTIVE_CHANNEL_3:
        ch = TIM_CHANNEL_3; i = 2; break;
    case HAL_TIM_ACTIVE_CHANNEL_4:
        ch = TIM_CHANNEL_4; i = 3; break;
    default:
        return;
    }

    uint16_t capture =
        (uint16_t)HAL_TIM_ReadCapturedValue(htim, ch);

    if (!rc[i].falling) {
        rc[i].rise = capture;
        rc[i].falling = 1;

        __HAL_TIM_SET_CAPTUREPOLARITY(
            htim, ch, TIM_INPUTCHANNELPOLARITY_FALLING);
    } else {
        uint16_t width = (uint16_t)(capture - rc[i].rise);
        rc[i].falling = 0;

        __HAL_TIM_SET_CAPTUREPOLARITY(
            htim, ch, TIM_INPUTCHANNELPOLARITY_RISING);

        if (width >= RC_MIN_US && width <= RC_MAX_US) {
            rc[i].pulse = width;
            rc[i].last_ms = HAL_GetTick();
            rc[i].valid = 1;
        } else {
            rc[i].valid = 0;
        }
    }
}

static uint8_t RC_Valid(void)
{
    uint32_t now = HAL_GetTick();

    for (int i = 0; i < RC_CHANNELS; i++) {
        if (!rc[i].valid ||
            rc[i].pulse < RC_MIN_US ||
            rc[i].pulse > RC_MAX_US ||
            (uint32_t)(now - rc[i].last_ms) > RC_TIMEOUT_MS)
            return 0;
    }

    return 1;
}

/* ---------------- Basic attitude estimate ---------------- */

/*
 * Educational complementary filter only.
 * Gyro bias calibration and board-axis verification are still needed.
 */
static void Estimate_Attitude(float dt)
{
    float ax = (float)imu.ax / 16384.0f;
    float ay = (float)imu.ay / 16384.0f;
    float az = (float)imu.az / 16384.0f;

    float gx = (float)imu.gx / 131.0f;
    float gy = (float)imu.gy / 131.0f;

    float accel_roll =
        atan2f(ay, az) * 57.2957795f;

    float accel_pitch =
        atan2f(-ax, sqrtf(ay * ay + az * az)) * 57.2957795f;

    roll_deg = 0.98f * (roll_deg + gx * dt)
             + 0.02f * accel_roll;

    pitch_deg = 0.98f * (pitch_deg + gy * dt)
              + 0.02f * accel_pitch;
}

/* ---------------- Main application ---------------- */

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_TIM2_Init();

    memset(rc, 0, sizeof(rc));

    /*
     * Safe startup: no ESC PWM is started.
     * Verify that your CubeMX GPIO configuration keeps ESC signals
     * at the required safe level during startup and reset.
     */

    imu_ok = (MPU_Init() == HAL_OK);

    for (int i = 0; i < RC_CHANNELS; i++) {
        __HAL_TIM_SET_CAPTUREPOLARITY(
            &htim2, rc_timer_channels[i],
            TIM_INPUTCHANNELPOLARITY_RISING);

        HAL_TIM_IC_Start_IT(&htim2, rc_timer_channels[i]);
    }

    previous_loop_ms = HAL_GetTick();

    while (1) {
        uint32_t now = HAL_GetTick();
        float dt = (now - previous_loop_ms) / 1000.0f;
        previous_loop_ms = now;

        if (dt <= 0.0f || dt > 0.1f)
            dt = 0.002f;

        if (MPU_Read(&imu) == HAL_OK) {
            imu_ok = 1;
            Estimate_Attitude(dt);
        } else {
            imu_ok = 0;
        }

        /*
         * Safe-state policy:
         * - Never arm in this starter.
         * - No motor output or motor mixer is enabled.
         * - Receiver loss and IMU errors keep the system disarmed.
         */
        if (!imu_ok || !RC_Valid()) {
            armed = 0;
        }

        /*
         * Debug variables for CubeIDE Live Expressions:
         * imu.ax, imu.ay, imu.az
         * imu.gx, imu.gy, imu.gz
         * rc[0].pulse ... rc[3].pulse
         * roll_deg, pitch_deg
         * imu_ok, armed
         */

        HAL_Delay(2);
    }
}
