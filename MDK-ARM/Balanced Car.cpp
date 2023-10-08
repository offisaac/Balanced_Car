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
//��������  
void Wheel::Wheel_Type_Update()
{
if(this->Wheel_ID==Left)//1������ 0������
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
//�����ٶ�(���ü�����)�ͽǶ� ���ٶ�(���ü���ֵ)

void Wheel::Wheel_Velocity_Update()
{
this->Velocity=Velocity_Cal();
this->Lase_Count=this->Count;//�����ݸ��·ֿ�����Ϊ������Ҫ��Ƶ�ʲ�һ�� �ٶȵ�Ƶ��̫�߻ᵼ�·ֱ��ʺܵͺܵ�
}
void Wheel::Wheel_Data_Update()
{
this->Angle=mpu_receive.pitch;
this->Angular_Velocity=mpu_receive.gyro[0]+2;//��Ϊ�ֲ�ƫ��
}

int Wheel::Velocity_Cal()
{
float out=0;
out=(this->Count-this->Lase_Count);//��С�ٶ�ֵ ����̫��
return out;
}

void Wheel::Motor_Control()//����ǰ���ж� ����id�ж� �������
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
//{this->PID_Velocity.integral_e=0;}	���ٶȻ�����ʱʹ��
this->PID_Velocity.SetPIDParam(velocity_p,velocity_i,0,velocity_i_value,1000);
this->PID_Velocity.Current=this->Velocity;
this->PID_Velocity.Target=0;
this->PID_Angle.SetPIDParam(angle_p,0,0,1000,1000);//kd����������ʹ��
this->PID_Angle.Current=this->Angle;//�����߼�����ȷ�� ��ǰ�ǶȽ��ٶȶ��Ǹ�ֵ �������ֵΪ�� ��ǰ ��������Ϊ�� Ҫ��Ȼ�������Ҫ������
this->PID_Angle.Target=-this->PID_Velocity.Adjust();
this->Out= (PID_Angle.Adjust()-this->Angular_Velocity*angle_d);//��Ϊʹ�ý��ٶ���Ϊd�� ���ҽ��ٶ�Ϊ�����ӦΪ��(��ǰ�� �Ƕȼ� ���ٶ�Ӧ��Ϊ��)
}
//����ļ���������ⷽʽ 1.�̳��е�һת������ת---�ٶ�Ϊ��ʱ���Ϊ��(�̳��еĽǶ��ǲ���Ҳ����ǰΪ��) 2.��˫���ı������ �ٶ�Ϊ�� ���Ϊ����Ŀ��Ƕ�ֵ ʹ���������ٶ�Ϊ��ʱ����ǰ��Ƕ�Ϊ����
//�Ǽ��ٶȵļ������ ��ǰתʱΪ�� ���Ƕȼ��� ������Ч��Ӧ����ʹ�Ƕȱ��




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
