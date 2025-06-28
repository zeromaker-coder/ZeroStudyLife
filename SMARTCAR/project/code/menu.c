#include "menu.h"
#include "pid.h"

#define IPS200_TYPE     (IPS200_TYPE_PARALLEL8)                                 // 双排排针 并口两寸屏 这里宏定义填写 IPS200_TYPE_PARALLEL8
                                                                                // 单排排针 SPI 两寸屏 这里宏定义填写 IPS200_TYPE_SPI

uint8 main_menu_item;//主菜单指针
uint8 sec_menu_item;//副菜单指针
uint8 car_go;//小车发车

/**
* @brief  菜单参数保存
* @param   无
*/
void menu_save(void)
{
    ;
}

/**
* @brief  调好的pid参数保存
* @param   无
*/
void pid_save(void)
{
    ;
}

/**
* @brief  调好的pid参数读出
* @param   无
*/
void pid_load(void)
{
    ;
}

/**
* @brief  初始化菜单,初始化IPS200显示屏
* @param   无
*/
void menu_init(void)
{
    ips200_set_dir(IPS200_PORTAIT);
    ips200_set_font(IPS200_8X16_FONT);
    ips200_set_color(RGB565_RED, RGB565_BLACK);
    ips200_init(IPS200_TYPE);
    main_menu_item=1;//主菜单指针
    sec_menu_item=1;//副菜单指针
}

/**
* @brief  菜单总显示函数
* @param   无
*/
void menu(void)
{
    if(main_menu_item==1)main_menu();//主菜单
    if(main_menu_item==2)Sec_Menu_01(); //角速度环PID菜单
}

/**
* @brief  主菜单显示函数
* @param   无
*/
void main_menu(void)
{
    ips200_show_string(60,0,"main_menu");  //主菜单标题
    ips200_show_string(16,30,"car_go");    //小车发车
    ips200_show_string(16,46,"gyro_pid");  //角速度环PID
    ips200_show_string(16,62,"angle_pid"); //角度环PID
    ips200_show_string(16,78,"speed_pid"); //速度环PID
    ips200_show_string(16,94,"turn_pid");  //转向环PID
    ips200_show_string(16,110,"parameter");//常用参数(机械中值之类的)
    ips200_show_string(16,126,"save_param");//保存参数
    ips200_show_string(16,142,"load_param");//载入参数

    ips200_show_string(0,sec_menu_item*16,"->");//菜单指针

    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
    {
        sec_menu_item--;
        if(sec_menu_item < 1) //如果副菜单指针小于1
        {
            sec_menu_item = 8; //则将副菜单指针置为8
        }
        key_clear_state(KEY_1); //清除按键状态
        ips200_clear(); //清屏
    }

    if(KEY_SHORT_PRESS == key_get_state(KEY_2))
    {
        sec_menu_item--;
        if(sec_menu_item > 8)//如果副菜单指针大于8
        {
            sec_menu_item = 1; //则将副菜单指针置为1
        }
        key_clear_state(KEY_2);//清除按键状态
        ips200_clear(); //清屏
    }
    //选择按钮按下
    if(KEY_SHORT_PRESS == key_get_state(KEY_3))
    {
        switch(sec_menu_item)
        {
            case 1:
                // 进入小车发车菜单
                car_go=1;//小车发车
                ips200_clear();
                break;
            case 2:
                // 进入角速度环PID菜单
                main_menu_item = 3;
                sec_menu_item = 1;
                ips200_clear();
                break;
            case 3:
                // 进入角度环PID菜单
                main_menu_item = 4;
                sec_menu_item = 1;
                ips200_clear();
                break;
            case 4:
                // 进入速度环PID菜单
                main_menu_item = 5;
                sec_menu_item = 1;
                ips200_clear();
                break;
            case 5:
                // 进入转向环PID菜单
                main_menu_item = 6;
                sec_menu_item = 1;
                ips200_clear();
                break;
            case 6:
                // 进入常用参数菜单
                main_menu_item = 7;
                sec_menu_item = 1;
                ips200_clear();
                break;
            case 7:
                // 保存参数
                ips200_show_string(60, 160, "Param Saved!");
                system_delay_ms(200);
                ips200_clear();
                break;
            case 8:
                // 载入参数
                ips200_show_string(60, 160, "Param Loaded!");
                system_delay_ms(200);
                ips200_clear();
                break;
            default:
                break;
        }
        key_clear_state(KEY_3);
    }
}


/**
* @brief  角速度环菜单显示函数
* @param   无
*/
void Sec_Menu_01(void)
{
    char gyro_p_str[50];
    char gyro_i_str[50];

    sprintf(gyro_p_str, "gyro_p: %.2f", gyro_pid_param.kp); //格式化角速度环kp
    sprintf(gyro_i_str, "gyro_i: %.2f", gyro_pid_param.ki); //格式化角速度环ki

    ips200_show_string(60,0,"gyro_pid");//二级菜单标题
    ips200_show_string(16,30,gyro_p_str);//角速度环kp
    ips200_show_string(16,46,gyro_i_str);//角速度环ki
    ips200_show_string(16,62,"back");//返回上一级菜单

    ips200_show_string(0,sec_menu_item*16,"->");//菜单指针

    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
    {
        sec_menu_item--;
        if(sec_menu_item < 1) //如果副菜单指针小于1
        {
            sec_menu_item = 3; //则将副菜单指针置为3
        }
        key_clear_state(KEY_1); //清除按键状态
        ips200_clear();//清屏
    }

    if(KEY_SHORT_PRESS == key_get_state(KEY_2))
    {
        sec_menu_item--;
        if(sec_menu_item > 3)//如果副菜单指针大于3
        {
            sec_menu_item = 1; //则将副菜单指针置为1
        }
        key_clear_state(KEY_2);//清除按键状态
        ips200_clear();//清屏
    }

    //返回
    if(sec_menu_item == 3 && KEY_SHORT_PRESS == key_get_state(KEY_3))
    {
        //返回主菜单
        main_menu_item = 1;
        sec_menu_item = 1;
        key_clear_state(KEY_3);//清除按键状态
        ips200_clear();//清屏
    }

    //修改角速度环kp
    if(sec_menu_item == 1)
    {
        if( KEY_SHORT_PRESS == key_get_state(KEY_3))
        {
            gyro_pid_param.kp+= 0.1f; //增加角速度环kp
            key_clear_state(KEY_3); //清除按键状态
            ips200_clear();//清屏
        }
        else if(KEY_SHORT_PRESS == key_get_state(KEY_4))
        {
            gyro_pid_param.kp-= 0.1f; //增加角速度环kp
            key_clear_state(KEY_4); //清除按键状态
            ips200_clear();//清屏
        }
    }

    //修改角速度环ki
    if(sec_menu_item == 2)
    {
        if( KEY_SHORT_PRESS == key_get_state(KEY_3))
        {
            gyro_pid_param.ki+= 0.1f; //增加角速度环kp
            key_clear_state(KEY_3); //清除按键状态
            ips200_clear();//清屏
        }
        else if(KEY_SHORT_PRESS == key_get_state(KEY_4))
        {
            gyro_pid_param.ki-= 0.1f; //增加角速度环kp
            key_clear_state(KEY_4); //清除按键状态
            ips200_clear();//清屏
        }
    }
}






