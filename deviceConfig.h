#define SERIAL_RBT_TX	(PA_9)//PA9, PA10 UART1
#define SERIAL_RBT_RX	(PA_10)

#define I2C1_SDA			(PB_9)
#define I2C1_SCL			(PB_8)

#define I2C_ADDRESS        0x1E

#define I2Cx                             I2C1
#define I2Cx_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2Cx_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_8
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SCL_AF                     GPIO_AF4_I2C1
#define I2Cx_SDA_PIN                    GPIO_PIN_9
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SDA_AF                     GPIO_AF4_I2C1

/* Definition for I2Cx's NVIC */
#define I2Cx_EV_IRQn                    I2C1_EV_IRQn
#define I2Cx_EV_IRQHandler              I2C1_EV_IRQHandler
#define I2Cx_ER_IRQn                    I2C1_ER_IRQn
#define I2Cx_ER_IRQHandler              I2C1_ER_IRQHandler
