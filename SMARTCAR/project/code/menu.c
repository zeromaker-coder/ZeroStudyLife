#include "menu.h"
#include "pid.h"
#include "flash.h"

#define IPS200_TYPE     (IPS200_TYPE_SPI)                                 // 双排排针 并口两寸屏 这里宏定义填写 IPS200_TYPE_PARALLEL8
                                                                                // 单排排针 SPI 两寸屏 这里宏定义填写 IPS200_TYPE_SPI

uint8 main_menu_item;//主菜单指针
uint8 sec_menu_item;//副菜单指针
uint8 car_go;//小车发车

float change_item_num[7]={1000,100,10,1,0.1f,0.01f,0.001f};


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
    key_init(5);
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
    ips200_show_string(16,94,"turn_pppdd");  //转向环ppdd
    ips200_show_string(16,110,"parameter");//常用参数(机械中值之类的)
    ips200_show_string(16,126,"save_param");//保存参数
    ips200_show_string(16,142,"load_param");//载入参数

    ips200_show_string(0,(sec_menu_item+1)*16,"->");//菜单指针
	
	  key_scanner();//这个千万不要忘;
	
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
        sec_menu_item++;
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
                main_menu_item = 2;
                sec_menu_item = 1;
                ips200_clear();
                break;
            case 3:
                // 进入角度环PID菜单
                main_menu_item = 3;
                sec_menu_item = 1;
                ips200_clear();
                break;
            case 4:
                // 进入速度环PID菜单
                main_menu_item = 4;
                sec_menu_item = 1;
                ips200_clear();
                break;
            case 5:
                // 进入转向环PID菜单
                main_menu_item = 5;
                sec_menu_item = 1;
                ips200_clear();
                break;
            case 6:
                // 进入常用参数菜单
                main_menu_item = 6;
                sec_menu_item = 1;
                ips200_clear();
                break;
            case 7:
                // 保存参数
                ips200_show_string(60, 160, "Param Saved!");
                menu_save();
                system_delay_ms(200);
                ips200_clear();
                break;
            case 8:
                // 载入参数
                ips200_show_string(60, 160, "Param Loaded!");
                menu_load();
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
    static uint8 change_mode=0;
    static int8 change_item_i=4;
    
    ips200_show_string(60,0,"gyro_pid");//二级菜单标题
    ips200_show_string(16,30+16*0,"gyro_kp:");//角速度环kp
    ips200_show_string(16,30+16*1,"gyro_ki");//角速度环ki
    ips200_show_string(16,30+16*2,"back");//返回上一级菜单
    ips200_show_string(16,30+16*10,"change_item");//修改量


    //显示数字
    ips200_show_float(150,30+16*0,gyro_pid_param.kp,4,3);
    ips200_show_float(150,30+16*1,gyro_pid_param.ki,4,3);
    ips200_show_float(150,30+16*10,change_item_num[change_item_i],4,3);

    if(!change_mode)ips200_show_string(0,(sec_menu_item+1)*16,"->");//菜单指针
    else ips200_show_string(0,(sec_menu_item+1)*16,"*");

    if(change_mode==2)ips200_show_string(0,30+10*16,"*");

    key_scanner();//千万不要忘

    if(!change_mode)
    {
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
            sec_menu_item++;
            if(sec_menu_item > 3)//如果副菜单指针大于3
            {
                sec_menu_item = 1; //则将副菜单指针置为1
            }
            key_clear_state(KEY_2);//清除按键状态
            ips200_clear();//清屏
        }
        //返回
        if((sec_menu_item == 3 && KEY_SHORT_PRESS == key_get_state(KEY_3))||KEY_SHORT_PRESS == key_get_state(KEY_4))
        {
            //返回主菜单
            main_menu_item = 1;
            sec_menu_item = 1;
            key_clear_state(KEY_3);//清除按键状态
            key_clear_state(KEY_4);//清除按键状态
            ips200_clear();//清屏
        }
        
        if(sec_menu_item>=1&&sec_menu_item<=3 && KEY_SHORT_PRESS == key_get_state(KEY_3))
        {
            change_mode=1;
            key_clear_state(KEY_3);//清除按键状态
            ips200_clear();//清屏
        }
    }
    else
    {
        if(change_mode==1)//修改变量模式
        {
            switch (sec_menu_item)
            {
                case 1:
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        gyro_pid_param.kp+= change_item_num[change_item_i];; //增加角速度环kp
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        gyro_pid_param.kp-= change_item_num[change_item_i];; //增加角速度环kp
                        key_clear_state(KEY_2); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_3))
                    {
                        change_mode=2;//修改加减量模式
                        key_clear_state(KEY_3); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_4))
                    {
                        change_mode=0;
                        key_clear_state(KEY_4); //清除按键状态
                        ips200_clear();//清屏
                    }
                case 2:
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        gyro_pid_param.ki+= change_item_num[change_item_i];; //增加角速度环kp
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        gyro_pid_param.ki-= change_item_num[change_item_i];; //增加角速度环kp
                        key_clear_state(KEY_2); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_3))
                    {
                        change_mode=2;//修改加减量模式
                        key_clear_state(KEY_3); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_4))
                    {
                        change_mode=0;
                        key_clear_state(KEY_4); //清除按键状态
                        ips200_clear();//清屏
                    }
            }
        }
        else if(change_mode==2)//修改加减量模式
        {
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        change_item_i--;

                        if(change_item_i>6) change_item_i=0;
                        if(change_item_i<0) change_item_i=6;


                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        change_item_i++;

                        if(change_item_i>6) change_item_i=0;
                        if(change_item_i<0) change_item_i=6;

    
                        key_clear_state(KEY_2); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_4))
                    {
                        change_mode=1;
                        key_clear_state(KEY_4); //清除按键状态
                        ips200_clear();//清屏
                    }
        }

    }	
}
   






