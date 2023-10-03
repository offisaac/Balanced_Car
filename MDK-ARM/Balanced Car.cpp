#include "internal.h"

int anlge_p=0;


	  
//更新类型  
void Wheel::Wheel_Type_Update()
{
if(this->Wheel_ID==Left)//1是左轮 0是右轮
{
this->GPIO_Type=Left_GPIO_Type;
this->GPIO_PIN=Left_GPIO_PIN;
}
if(this->Wheel_ID==Right)
{
this->GPIO_Type=Right_GPIO_Type;
this->GPIO_PIN=Right_GPIO_PIN;
}
}
//更新速度(调用即计算)和角度 角速度(调用即读值)
void Wheel::Wheel_Data_Update()
{
this->Velocity=Velocity_Cal();
this->Angle=mpu_receive.pitch;
this->Angular_Velocity=mpu_receive.gyro[0]+2;
}

float Wheel::Velocity_Cal()
{
float out=0;
Tick_Update(time_tick);//更新 获取时间 上一次的时间是上一次进行更新的时间点
out=(this->Count-this->Lase_Count)/time_tick.dt;
this->Lase_Count=this->Count;//更新 
return out;
}

void Wheel::Motor_Control()//进行前后判断 进行id判断 最后运行
{
if(this->Out>=0)
{
this->State=Forward;
if(this->Wheel_ID==Right)	
{
Right_Forward();
__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_4,(this->Out));
}
if(this->Wheel_ID==Left)	
{
Left_Forward();
__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,(this->Out));
}
}
if(this->Out<0)
{
this->State=Backward;
if(this->Wheel_ID==Right)	
{
Right_Backward();
__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_4,-(this->Out));
}
if(this->Wheel_ID==Left)	
{
Left_Backward();
__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,-(this->Out));
}
}
}




void Wheel::Adjust()
{
	this->PID_Angle.SetPIDParam(anlge_p,0,0,1000,1000);
	this->PID_Angle.Current=this->Angle;
	this->PID_Angle.Target=0;
	this->Out= PID_Angle.Adjust();
}





void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN)
{
if(GPIO_PIN==GPIO_PIN_0)
{
	if(Right_Wheel.State==Forward)
{	
Right_Wheel.Count++;
}
	if(Right_Wheel.State==Backward)
{	
Right_Wheel.Count--;
}
}
if(GPIO_PIN==GPIO_PIN_1)
{
	if(Left_Wheel.State==Forward)
{
Left_Wheel.Count++;
}
	if(Left_Wheel.State==Backward)
{
Left_Wheel.Count--;
}
}
}
