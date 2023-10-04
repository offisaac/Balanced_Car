 /**
  ******************************************************************************
  * @file   task.cpp
  * @brief  freertos task running file.
  ******************************************************************************
  * @note
  *  - Before running your devices, just do what you want ~ !
  *  - More devices or using other classification is decided by yourself ~ !
  ===============================================================================
                                    Task List
  ===============================================================================
  * <table>
  * <tr><th>Task Name     <th>Priority          <th>Frequency/Hz    <th>Stack/Byte
  * <tr><td>              <td>                  <td>                <td>
  * </table>
  *
 */

/* Includes ------------------------------------------------------------------*/
#include "internal.h"
/* Private define ------------------------------------------------------------*/
TaskHandle_t DjiMotor_Handle;		
TaskHandle_t IMU_Handle;		
TaskHandle_t Upper_Monitor_Handle;
TaskHandle_t Wheel_Update_Handle;	
/* Private function declarations ---------------------------------------------*/
void tskDjiMotor(void *arg);
void tskIMU(void *arg);
void tskUpper_Monitor(void *arg);
void tskWheel_Update(void *arg);
/* Function prototypes -------------------------------------------------------*/
/**
* @brief  Initialization of device management service
* @param  None.
* @return None.
*/
void Service_Devices_Init(void)
{
  xTaskCreate(tskDjiMotor, 	"App.Motor",   Small_Stack_Size, NULL, PriorityAboveNormal, &DjiMotor_Handle);
	#if  USE_SRML_MPU6050
  xTaskCreate(tskIMU,				"App.IMU",	   Small_Stack_Size, NULL, PriorityNormal,      &IMU_Handle);
	#endif
	xTaskCreate(tskUpper_Monitor,				"App.Upper_Monitor",	   Small_Stack_Size, NULL, PriorityNormal,      &Upper_Monitor_Handle);
	xTaskCreate(tskWheel_Update,				"App.Wheel_Update",	   Small_Stack_Size, NULL, PriorityNormal,      &Wheel_Update_Handle);
}

void tskWheel_Update(void *arg)
{
	for(;;){
		vTaskDelay(20);
	  Right_Wheel.Wheel_Data_Update();
		Left_Wheel.Wheel_Data_Update();//更新内部参数
		Right_Wheel.Adjust();//pid计算并内部赋值
    Left_Wheel.Adjust();
	}
}

void tskUpper_Monitor(void *arg)
{
	TickType_t xLastWakeTime_t;
	  xLastWakeTime_t = xTaskGetTickCount();
	for(;;){
vTaskDelayUntil(&xLastWakeTime_t, 5);
Sent_Contorl(&huart1);
	}
}

void tskDjiMotor(void *arg)
{
	/*	pre load for task	*/
	static Motor_CAN_COB Tx_Buff;
	for(;;){
		/* wait for next circle */
		vTaskDelay(1);//实际上电机的时候一定注释这个
    Right_Wheel.Motor_Control();
    Left_Wheel.Motor_Control();
	}
}

#if  USE_SRML_MPU6050
/**
 * @brief MPU6050读取数据
 */
void tskIMU(void *arg)
{
	/* Pre-Load for task */
	  TickType_t xLastWakeTime_t;
	  xLastWakeTime_t = xTaskGetTickCount();
	for(;;){
		/* wait for next circle */
		vTaskDelayUntil(&xLastWakeTime_t, 5);
		/*	读取MPU6050数据	*/
		dmp_read_data(&mpu_receive);
	}
}
#endif



