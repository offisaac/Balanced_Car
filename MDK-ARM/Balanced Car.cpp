#include "internal.h"

	 
	  
	  
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




void Wheel::Motor_Control()//进行前后判断 进行id判断 最后运行
{
if(this->Out>=0)
{
this->State=Forward;
if(this->Wheel_ID==Right)	
{
Right_Forward();
__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_4,-(this->Out));
}
if(this->Wheel_ID==Left)	
{
Left_Forward();
__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,-(this->Out));
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
