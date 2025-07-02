#include "flash.h"
#include "pid.h"

#define FLASH_SECTION_INDEX       (127)                                         // 存储数据用的扇区 倒数第一个扇区
#define FLASH_PAGE_INDEX          (3)                                           // 存储数据用的页码 倒数第一个页码





/**
* @brief  菜单参数保存
* @param   无
*/
void menu_save(void)
{
    if(flash_check(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX))                      // 判断是否有数据
    {
        flash_erase_page(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);                // 擦除这一页
    }
    flash_buffer_clear(); //擦除缓存区
    //写入缓冲区
    flash_union_buffer[0].float_type  = gyro_pid_param.kp;
    flash_union_buffer[1].float_type  = gyro_pid_param.ki;

    flash_union_buffer[2].float_type  = angle_pid_param.kp;
    flash_union_buffer[3].float_type  = angle_pid_param.kd;

    flash_union_buffer[4].float_type  = speed_pid_param.kp;
    flash_union_buffer[5].float_type  = speed_pid_param.ki;

    flash_union_buffer[6].float_type  = turn_pid_param.kp;
    flash_union_buffer[7].float_type  = turn_pid_param.kp2;
    flash_union_buffer[8].float_type  = turn_pid_param.kd;
    flash_union_buffer[9].float_type  = turn_pid_param.kd2;

    flash_union_buffer[10].float_type = gyro_pid_param.PID_I_LIMIT_MAX;
    flash_union_buffer[11].float_type = gyro_pid_param.PID_OUT_LIMIT_MAX;

    flash_union_buffer[12].float_type = angle_pid_param.PID_OUT_LIMIT_MAX;

    flash_union_buffer[13].float_type = speed_pid_param.PID_OUT_LIMIT_MAX;
    flash_union_buffer[14].float_type = speed_pid_param.PID_I_LIMIT_MAX;

    flash_union_buffer[15].float_type = turn_pid_param.PID_OUT_LIMIT_MAX;

    flash_union_buffer[16].float_type = user_param.mid_angle;
    flash_union_buffer[17].float_type = user_param.target_speed;

    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);        // 向指定 Flash 扇区的页码写入缓冲区数据
}

/**
* @brief  调好的pid参数取出
* @param   无
*/
void menu_load(void)
{
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);           // 将数据从 flash 读取到缓冲区ad_page_to_buffer;
    //参数读出
    gyro_pid_param.kp=flash_union_buffer[0].float_type;
    gyro_pid_param.ki=flash_union_buffer[1].float_type;

    angle_pid_param.kp=flash_union_buffer[2].float_type;
    angle_pid_param.kd=flash_union_buffer[3].float_type;

    speed_pid_param.kp=flash_union_buffer[4].float_type;
    speed_pid_param.ki=flash_union_buffer[5].float_type;

    turn_pid_param.kp=flash_union_buffer[6].float_type;
    turn_pid_param.kp2=flash_union_buffer[7].float_type;
    turn_pid_param.kd=flash_union_buffer[8].float_type;
    turn_pid_param.kd2=flash_union_buffer[9].float_type;

    gyro_pid_param.PID_I_LIMIT_MAX = flash_union_buffer[10].float_type;
    gyro_pid_param.PID_OUT_LIMIT_MAX= flash_union_buffer[11].float_type;

    angle_pid_param.PID_OUT_LIMIT_MAX= flash_union_buffer[12].float_type;

    speed_pid_param.PID_OUT_LIMIT_MAX = flash_union_buffer[13].float_type;
    speed_pid_param.PID_I_LIMIT_MAX = flash_union_buffer[14].float_type;

    turn_pid_param.PID_OUT_LIMIT_MAX = flash_union_buffer[15].float_type;

    user_param.mid_angle = flash_union_buffer[16].float_type;
    user_param.target_speed = flash_union_buffer[17].float_type;
    
    flash_buffer_clear(); //擦除缓存区
}

