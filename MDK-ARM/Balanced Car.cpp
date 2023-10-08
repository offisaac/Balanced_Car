#include "internal.h"

float angle_p=50;
float angle_d=2;
float velocity_p=0;
float velocity_i=0.05;
float velocity_i_value=0;

//float angle_p=0;
//float angle_d=0;
//float velocity_p=0;
//float velocity_i=0;
//float velocity_i_value=0;
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

void Wheel::Wheel_Velocity_Update()
{
this->Velocity=Velocity_Cal();
this->Lase_Count=this->Count;//与数据更新分开是因为两者需要的频率不一样 速度的频率太高会导致分辨率很低很低
}
void Wheel::Wheel_Data_Update()
{
this->Angle=mpu_receive.pitch;
this->Angular_Velocity=mpu_receive.gyro[0]+2;//人为弥补偏差
}

int Wheel::Velocity_Cal()
{
float out=0;
out=(this->Count-this->Lase_Count);//减小速度值 否则太大
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
//if(this->Last_Velocity!=this->Velocity)
//{this->PID_Velocity.integral_e=0;}	单速度环调节时使用
this->PID_Velocity.SetPIDParam(velocity_p,velocity_i,0,velocity_i_value,1000);
this->PID_Velocity.Current=this->Velocity;
this->PID_Velocity.Target=0;
this->PID_Angle.SetPIDParam(angle_p,0,0,1000,1000);//kd在下面另外使用
this->PID_Angle.Current=this->Angle;//这里逻辑是正确的 往前角度角速度都是负值 所以输出值为正 向前 极性巧了为正 要不然这里输出要反过来
this->PID_Angle.Target=-this->PID_Velocity.Adjust();
this->Out= (PID_Angle.Adjust()-this->Angular_Velocity*angle_d);//人为使用角速度作为d项 并且角速度为负输出应为正(向前倒 角度减 角速度应该为负)
}
//这里的极性两种理解方式 1.教程中的一转两个疯转---速度为正时输出为正(教程中的角度是不是也是向前为负) 2.从双环的本质理解 速度为正 输出为正的目标角度值 使车体正（速度为正时车体前倾角度为负）
//角加速度的极性理解 向前转时为负 即角度减少 其作用效果应该是使角度变大




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
