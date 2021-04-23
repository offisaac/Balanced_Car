/**
  **********************************************************************************
  * @file   : App_CeremonyDancing.cpp
  * @brief  : This program is for Robomaster Engineer Dance Control in ceremoney.
  *           (Robomaster 2019 version 4.0).
  * @author : Mentos_Seetoo
  * @date   : September,2019
  **********************************************************************************
  *   
**/
#include "App_CeremonyDancing.h"
#include "Service_Communication.h"
#include "Service_Devices.h"
#include "Service_EngineerChassis.h"

/*Task Define---------------------------------------------------------------------*/
TaskHandle_t DanceCtrl_Handle;        
uint8_t   dance_is_lock = 0;

uint8_t turn_body(uint16_t turn_time, uint16_t correct_time, uint16_t straight_time, float speed);
uint8_t translation(float move_direction,uint16_t addition_time);
uint8_t catWalk(uint16_t turn_time, uint16_t wait_time);
uint8_t wave_flags(uint16_t lift_time, uint16_t expand_time);
uint8_t move_one_direction(float speed, float direction, uint16_t add_time);

_TRANS_STEP_ catWalk_step_count = TRANS_WAIT;
_TRANS_STEP_ step_count = TRANS_WAIT;
_DANCE_STEP dance_step_count = DANCE_WAIT;
int32_t wave_zero_time = 0;
int32_t turn_body_zero_time = 0;
int32_t move_zero_time = 0;
/*Function Prototypes-------------------------------------------------------------*/
/*
  ���������Ҫ��ʱ����������2020��������ѧ�����Ͻ���һ�μ��衣
*/
/**
* @brief  Task for remote control.
*/
void Task_DanceInRemote(void *arg)
{
  /* Cache for Task */
  uint8_t  temp;
  static COB_TypeDef RelayCmdBuf;
  
  /* Pre-Load for task */
  RelayCmdBuf.ID = 0x001;
  RelayCmdBuf.DLC= 8;
  memset(RelayCmdBuf.Data, 0 ,8);
  Engineer_Master.Switch_ActionMode(NORMAL);
    
  /* Infinite loop ----------------------------------------------------------*/
  TickType_t xLastWakeTime_Remote;
  xLastWakeTime_Remote = xTaskGetTickCount();
  for(;;)
  { 
    //����Ϊ�˸Ϲ��Ͱ�service����������������ݷŵ��������ֱ��ת45��
    Engineer_Master.IO_Port(IMU_IN, IMU_Obj.GetRoll(), -IMU_Obj.GetPitch(), IMU_Obj.GetYaw());
      
    if(DR16.GetStatus() == Connection_Established)
    { 
      //S2��������Ϊ����ģʽ������Ϊ�ֱ�ģʽ(��ΪĿǰ�õ�ң����S1�ǻ���~ QAQ )
      if(DR16.GetS2() != DOWN)
      {
          if(dance_is_lock == ON){
              SuperRelay_Obj.Write_Cmd(B);
          }
          dance_is_lock = OFF;
          dance_step_count = DANCE_WAIT;
          catWalk_step_count = TRANS_WAIT;
          step_count = TRANS_WAIT;
          wave_zero_time = 0;
          turn_body_zero_time = 0;
          ControllerMode();
      }
      else
      {
          DancingMode();
      }
    }
    //���߱���
    else
    {
      Engineer_Master.Switch_ActionMode(CHASSIS_LOCK);
    }
    
    /* 
      ���ͼ̵�������Ҫ�İ�����Ϣ 
    */
    temp = SuperRelay_Obj.Get_Cmd();
    if(temp != RelayCmdBuf.Data[0])
    {
      RelayCmdBuf.Data[0] = temp;
      if(RelayCmdBuf.Data[0] != 0)
        xQueueSend(CAN2_TxPort,&RelayCmdBuf,0);
    }
    
    SuperRelay_Obj.Clear_Cmd();
   /* Pass control to the next task ------------------------------------------*/
    vTaskDelayUntil(&xLastWakeTime_Remote,5);
  }
}


/*------------------------------------------------------------------------------------------------*/
/*
ÿ������ֻ��һ��
*/
void DancingMode(void)
{
    if(!dance_is_lock)
    {
        switch(dance_step_count)
        {
            case DANCE_WAIT:
                dance_step_count = DANCE_STEP1;
                break;
            
            case DANCE_STEP1:
                //è��
                if(catWalk(7923,9207) == 1)
                {
                    dance_step_count = DANCE_STEP2;
                }
                else {};
                break;
            
            case DANCE_STEP2:
                //ǰ�����ң���
                if(translation(ROUND_POSITIVE,1284) == 1)
                {
                    dance_step_count = DANCE_STEP3;
                }
                else {};
                break;
            
            case DANCE_STEP3:
                //è��
                if(catWalk(2568,3872 + 1284) == 1)
                {
                    dance_step_count = DANCE_STEP4;
                }
                else {};
                break;
            
            case DANCE_STEP4:
                //ǰ�����ң���
                if(translation(ROUND_NEGATIVE,1284) == 1)
                {
                    dance_step_count = DANCE_STEP9;
                }
                else {};
                break;
                    
//            case DANCE_STEP5:
//                //�ҷ�ƽ�ƹ���
//                if(move_one_direction(0.2f,-1.0f,0) == 1)
//                {
//                    dance_step_count = DANCE_STEP6;
//                }
//                else {};
//                break;
//            
//            case DANCE_STEP6:
//                //��ƽ�ƹ���
//                if(move_one_direction(0.2f,1.0f,1284) == 1)
//                {
//                    dance_step_count = DANCE_STEP7;
//                }
//                else {};
//                break;
//            
//            case DANCE_STEP7:
//                //ǰ�����ң���
//                if(translation(ROUND_POSITIVE,0) == 1)
//                {
//                    dance_step_count = DANCE_STEP8;
//                }
//                else {};
//                break;
//            
//            case DANCE_STEP8:
//                //è��
//                if(catWalk(2568,3872 + 1284) == 1)
//                {
//                    dance_step_count = DANCE_STEP9;
//                }
//                else{}
//                break;
//            
            case DANCE_STEP9:
                //��ת45�㣬����
                if(turn_body(2568, 3420 + 500, 5300, 0.46f) == 1)
                {
                    dance_step_count = DANCE_STEP10;
//                    dance_is_lock = ON;
//                    dance_step_count = DANCE_WAIT;
                }
                else {};
                break;
            
            case DANCE_STEP10:
                //�������첢ƽ��
                if(wave_flags(1000,1500) == 1)
                {
                    dance_is_lock = ON;
                    dance_step_count = DANCE_WAIT;
                }
                else{}
                break;
            
            default:break;
        }
    }
    else
    {
        Engineer_Master.Switch_ActionMode(NORMAL);
        Engineer_Master.Manual_XYZ(-DR16.Get_LX_Norm(),
                                   -DR16.Get_LY_Norm(),
                                   DR16.Get_RX_Norm());
    }
}

void ControllerMode()
{
  Engineer_Master.Switch_ActionMode(NORMAL);
  //����
  if(DR16.GetS2() == MID)
  {
    Engineer_Master.Manual_XYZ(-DR16.Get_LX_Norm(),
                               -DR16.Get_LY_Norm(),
                               DR16.Get_RX_Norm());
  }
  //����
  else if(DR16.GetS2() == UP)
  {
    Engineer_Master.Manual_XYZ(DR16.Get_LX_Norm(),
                               DR16.Get_LY_Norm(),
                               DR16.Get_RX_Norm());
  }
}


/*------------------------------------------------------------------------------------------------*/

uint8_t catWalk(uint16_t turn_time, uint16_t wait_time)
{
    static int32_t current_time = 0;
    static int32_t zero_time = 0;
        
    current_time = xTaskGetTickCount() - zero_time;
        
    switch(catWalk_step_count)
    {
        case TRANS_WAIT:
            zero_time = xTaskGetTickCount();
            current_time = xTaskGetTickCount() - zero_time;
            catWalk_step_count = TRANS_STEP1;
            break;
        
        case TRANS_STEP1:
            if(current_time >= turn_time)
                catWalk_step_count = TRANS_STEP2;
            else
            {
//              SuperRelay_Obj.Write_Cmd(Mouse_Right);
//              if(Engineer_Master.Get_CurrentMode() == NORMAL){
//                Engineer_Master.Switch_ActionMode(AUTO_ROTATE);
//                Engineer_Master.Operations.auto_rotate_flag = CAT_WALK;
//                Engineer_Master.cat_walk_flag = CAT_LEFT;
//              }
            }
            break;
            
        case TRANS_STEP2:
            if(current_time >= wait_time)
                catWalk_step_count = TRANS_END;
            else
            {
                Engineer_Master.Switch_ActionMode(NORMAL);
                Engineer_Master.Manual_XYZ(0,
                                           0,
                                           0);
            }
            break;
        
        default:break;
    }
    
    if(catWalk_step_count == TRANS_END)
    {
        catWalk_step_count = TRANS_WAIT;
        Engineer_Master.Switch_ActionMode(NORMAL);
        Engineer_Master.Manual_XYZ(0,
                                   0,
                                   0);
        return 1;
    }
    else 
        return 0;
}


uint8_t translation(float move_direction,uint16_t addition_time)
{
    static int32_t current_time = 0;
    static int32_t zero_time;
    const  float speed = 0.2f;
    
    current_time = xTaskGetTickCount() - zero_time;
    
    switch(step_count)
    {
        case TRANS_WAIT:
            zero_time = xTaskGetTickCount();
            step_count = TRANS_STEP1;
            break;
        //ǰ
        case TRANS_STEP1:
            if(current_time >= 1284)
                step_count = TRANS_STEP2;
            else
                Engineer_Master.Manual_XYZ(speed*move_direction,
                                           -speed,
                                           0);
            break;
        //��
        case TRANS_STEP2:
            if(current_time >= 2568)
                step_count = TRANS_STEP3;
            else
                Engineer_Master.Manual_XYZ(-speed*move_direction,
                                           -speed,
                                           0);
            break;
        //��
        case TRANS_STEP3:
            if(current_time >= 3852)
                step_count = TRANS_STEP4;
            else
                Engineer_Master.Manual_XYZ(-speed*move_direction,
                                            speed,
                                            0);
            break;
        //��
        case TRANS_STEP4:
            if(current_time >= 5136)
                step_count = TRANS_END;
            else
                Engineer_Master.Manual_XYZ(speed*move_direction,
                                           speed,
                                           0);
            break;
        
        //�ȴ�
        case TRANS_STEP5:
            if(current_time >= 6420 + addition_time)
                step_count = TRANS_END;
            else
                Engineer_Master.Manual_XYZ(0,
                                           0,
                                           0);
            break;
        default:break;
    }
    if(step_count == TRANS_END)
    {
        step_count = TRANS_WAIT;
        Engineer_Master.Manual_XYZ(0,
                                   0,
                                   0);
        return 1;
    }
    else 
        return 0;
}

uint8_t turn_body(uint16_t turn_time, uint16_t correct_time, uint16_t straight_time, float speed)
{
    static int32_t current_time = 0;
    
    if(turn_body_zero_time == 0)
        turn_body_zero_time = xTaskGetTickCount();
    else{}
    
    current_time = xTaskGetTickCount() - turn_body_zero_time;
    
    if(current_time < turn_time)
    {
        Engineer_Master.IO_Port(IMU_IN, IMU_Obj.GetRoll(), -IMU_Obj.GetPitch(), IMU_Obj.GetYaw() + 45);
        return 0;
    }
    else
    {
        if(current_time < correct_time)
        {
            Engineer_Master.IO_Port(IMU_IN, IMU_Obj.GetRoll(), -IMU_Obj.GetPitch(), IMU_Obj.GetYaw());
            return 0;
        }
        else
        {
            if(current_time < straight_time)
            {
                Engineer_Master.Manual_XYZ(0,
                                           -speed,
                                           0);
            }
            else
            {
                turn_body_zero_time = 0;
                return 1;
            }
        }
    }
}


uint8_t wave_flags(uint16_t lift_time, uint16_t expand_time)
{
    static int32_t current_time = 0;

    
    if(wave_zero_time == 0)
        wave_zero_time = xTaskGetTickCount();
    else{}
    
    current_time = xTaskGetTickCount() - wave_zero_time;
        
    if(current_time < lift_time)
    {
        Engineer_Master.Switch_ActionMode(GET_BULLET);     //ȡ������ϵ�У�������׼
        SuperRelay_Obj.Write_Cmd(Shift_X);
        Engineer_Master.Manual_XYZ(0,
                                   0,
                                   0);
        return 0;
    }
    else
    {
        if(current_time < expand_time)
        {
           SuperRelay_Obj.Write_Cmd(Mouse_Right);
           return 0;
        }
        else
        {
            wave_zero_time = 0;
            return 1;
        }
    }
}

uint8_t move_one_direction(float speed, float direction, uint16_t add_time)
{
    static int32_t current_time = 0;

    
    if(move_zero_time == 0)
        move_zero_time = xTaskGetTickCount();
    else{}
    
    current_time = xTaskGetTickCount() - move_zero_time;
        
    if(current_time < 2500)
    {//ƽ��
        Engineer_Master.Manual_XYZ(-speed*direction,
                                   0,
                                   0);
        return 0;
    }
    else
    {//����
        if(current_time < 5000)
        {
            Engineer_Master.Manual_XYZ(speed*direction,
                                       0,
                                       0);
            return 0;
        }
        else 
        {
            if(current_time > 5000 + add_time)
            {
                move_zero_time = 0;
                current_time = 0;
                return 1;
            }
            else
            {
                Engineer_Master.Manual_XYZ(0,
                                           0,
                                           0);
                return 0;
            }
        }
    }
}
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
