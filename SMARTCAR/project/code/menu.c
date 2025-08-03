#include "menu.h"
#include "pid.h"
#include "flash.h"
#include "image.h"
#include "beep.h"
#include "encoder.h"
#include "imu.h"


#define IPS200_TYPE     (IPS200_TYPE_SPI)                                 // 双排排针 并口两寸屏 这里宏定义填写 IPS200_TYPE_PARALLEL8
                                                                                // 单排排针 SPI 两寸屏 这里宏定义填写 IPS200_TYPE_SPI

uint8 main_menu_item;//主菜单指针
uint8 sec_menu_item;//副菜单指针
uint8 car_go;//小车发车

float change_item_num[7]={1000,100,10,1,0.1f,0.01f,0.001f};//加减范围


/**
* @brief  初始化菜单,初始化IPS200显示屏
* @param   无
*/
void menu_init(void)
{
    ips200_set_dir(IPS200_PORTAIT_180);
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
    if(main_menu_item==2)Sec_Menu_01();//角速度环PI菜单
    if(main_menu_item==3)Sec_Menu_02();//角度环PD菜单
    if(main_menu_item==4)Sec_Menu_03();//速度环PI菜单
    if(main_menu_item==5)Sec_Menu_04();//转向环PPDD菜单
    if(main_menu_item==6)Sec_Menu_05();//用户常用参数
    if(main_menu_item==7)image_menu();//图像菜单
    if(main_menu_item==8)circle_param();//环岛参数
}

/**
* @brief  主菜单显示函数
* @param   无
*/
void main_menu(void)
{
    ips200_show_string(60,0,"main_menu");  //主菜单标题
    ips200_show_string(16,30,"car_go");    //小车发车
    ips200_show_string(16,46,"gyro_pi");  //角速度环PI
    ips200_show_string(16,62,"angle_pd"); //角度环PD
    ips200_show_string(16,78,"speed_pi"); //速度环PI
    ips200_show_string(16,94,"turn_ppdd");  //转向环ppdd
    ips200_show_string(16,110,"parameter");//常用参数(机械中值之类的)
    ips200_show_string(16,126,"image_and_other_param");//图像以及其他参数显示界面
    ips200_show_string(16,142,"circle_param");//环岛参数
    ips200_show_string(16,158,"save_param");//保存参数
    ips200_show_string(16,174,"load_param");//载入参数

    ips200_show_string(0,(sec_menu_item+1)*16,"->");//菜单指针
	
	key_scanner();//这个千万不要忘;
	
    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
    {
        sec_menu_item--;
        if(sec_menu_item < 1) //如果副菜单指针小于1
        {
            sec_menu_item = 10; //则将副菜单指针置为9
        }
        key_clear_state(KEY_1); //清除按键状态
        ips200_clear(); //清屏
    }

    if(KEY_SHORT_PRESS == key_get_state(KEY_2))
    {
        sec_menu_item++;
        if(sec_menu_item > 10)//如果副菜单指针大于9
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
                beep_on();
                ips200_clear();
                car_go=1;//小车发车
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
                //进入图像参数以及其他参数界面
                main_menu_item = 7;
                sec_menu_item = 1;
                ips200_clear();
                break;

            case 8:
                // 进入环岛参数菜单
                main_menu_item = 8;
                sec_menu_item = 1;
                ips200_clear();
                break;
            case 9:
                // 保存参数
                ips200_clear();
                ips200_show_string(60, 160, "Param Saved!");
                menu_save();
                //beep_on();
                system_delay_ms(50);
                ips200_clear();
                break;
            case 10:
                // 载入参数
                ips200_clear();
                ips200_show_string(60, 160, "Param Loaded!");
                menu_load();
                //beep_on();
                system_delay_ms(50);
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
    ips200_show_string(16,30+16*1,"gyro_ki:");//角速度环ki
    ips200_show_string(16,30+16*2,"back");//返回上一级菜单
    ips200_show_string(16,30+16*10,"change_item:");//修改量


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
                        gyro_pid_param.kp-= change_item_num[change_item_i];; //减少角速度环kp
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
                        gyro_pid_param.ki+= change_item_num[change_item_i];; //增加角速度环ki
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        gyro_pid_param.ki-= change_item_num[change_item_i];; //减少角速度环ki
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
   
/**
* @brief  角度环菜单显示函数
* @param   无
*/
void Sec_Menu_02(void)
{
    static uint8 change_mode=0;
    static int8 change_item_i=4;
    
    ips200_show_string(60,0,"angle_pid");//二级菜单标题
    ips200_show_string(16,30+16*0,"angle_kp:");//角度环kp
    ips200_show_string(16,30+16*1,"angle_kd:");//角度环ki
    ips200_show_string(16,30+16*2,"back");//返回上一级菜单
    ips200_show_string(16,30+16*10,"change_item:");//修改量


    //显示数字
    ips200_show_float(150,30+16*0,angle_pid_param.kp,4,3);
    ips200_show_float(150,30+16*1,angle_pid_param.kd,4,3);
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
                        angle_pid_param.kp+= change_item_num[change_item_i];//增加角度环kp
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        angle_pid_param.kp-= change_item_num[change_item_i];//减少角度环kp
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
                        angle_pid_param.kd+= change_item_num[change_item_i]; //增加角度环kd
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        angle_pid_param.kd-= change_item_num[change_item_i];//减少角度环kd
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

/**
* @brief  速度环菜单显示函数
* @param   无
*/
void Sec_Menu_03(void)
{
    static uint8 change_mode=0;
    static int8 change_item_i=4;
    
    ips200_show_string(60,0,"speed_pid");//二级菜单标题
    ips200_show_string(16,30+16*0,"speed_kp:");//速度环kp
    ips200_show_string(16,30+16*1,"speed_ki:");//速度环ki
    ips200_show_string(16,30+16*2,"back");//返回上一级菜单
    ips200_show_string(16,30+16*10,"change_item:");//修改量


    //显示数字
    ips200_show_float(150,30+16*0,speed_pid_param.kp,4,3);
    ips200_show_float(150,30+16*1,speed_pid_param.ki,4,3);
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
                        speed_pid_param.kp+= change_item_num[change_item_i]; //增加速度环kp
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        speed_pid_param.kp-= change_item_num[change_item_i];//减少速度环kp
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
                        speed_pid_param.ki+= change_item_num[change_item_i];//增加速度环ki
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        speed_pid_param.ki-= change_item_num[change_item_i]; //减少速度环ki
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

/**
* @brief  转向环显示函数
* @param   无
*/
void Sec_Menu_04(void)
{
    static uint8 change_mode=0;
    static int8 change_item_i=4;
    
    ips200_show_string(60,0,"turn_ppdd");//二级菜单标题
    ips200_show_string(16,30+16*0,"turn_kp:");//转向环kp
    ips200_show_string(16,30+16*1,"turn_kp2:");//转向环kp2
    ips200_show_string(16,30+16*2,"turn_kd:");//转向环kd
    ips200_show_string(16,30+16*3,"turn_kd2:");//转向环kd2
    ips200_show_string(16,30+16*4,"back");//返回上一级菜单
    ips200_show_string(16,30+16*10,"change_item:");//修改量


    //显示数字
    ips200_show_float(150,30+16*0,turn_pid_param.kp,4,3);
    ips200_show_float(150,30+16*1,turn_pid_param.kp2,4,3);
    ips200_show_float(150,30+16*2,turn_pid_param.kd,4,3);
    ips200_show_float(150,30+16*3,turn_pid_param.kd2,4,3);
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
                sec_menu_item = 5; //则将副菜单指针置为5
            }
            key_clear_state(KEY_1); //清除按键状态
            ips200_clear();//清屏
        }

        if(KEY_SHORT_PRESS == key_get_state(KEY_2))
        {
            sec_menu_item++;
            if(sec_menu_item > 5)//如果副菜单指针大于5
            {
                sec_menu_item = 1; //则将副菜单指针置为1
            }
            key_clear_state(KEY_2);//清除按键状态
            ips200_clear();//清屏
        }
        //返回
        if((sec_menu_item == 5 && KEY_SHORT_PRESS == key_get_state(KEY_3))||KEY_SHORT_PRESS == key_get_state(KEY_4))
        {
            //返回主菜单
            main_menu_item = 1;
            sec_menu_item = 1;
            key_clear_state(KEY_3);//清除按键状态
            key_clear_state(KEY_4);//清除按键状态
            ips200_clear();//清屏
        }
        
        if(sec_menu_item>=1&&sec_menu_item<=4 && KEY_SHORT_PRESS == key_get_state(KEY_3))
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
                        turn_pid_param.kp+= change_item_num[change_item_i]; 
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        turn_pid_param.kp-= change_item_num[change_item_i];
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
                        turn_pid_param.kp2+= change_item_num[change_item_i];
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        turn_pid_param.kp2-= change_item_num[change_item_i]; 
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
                case 3:
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        turn_pid_param.kd+= change_item_num[change_item_i]; 
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        turn_pid_param.kd-= change_item_num[change_item_i];
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

                case 4:
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        turn_pid_param.kd2+= change_item_num[change_item_i]; 
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        turn_pid_param.kd2-= change_item_num[change_item_i];
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



/**
* @brief  常用数据显示函数
* @param   无
*/
void Sec_Menu_05(void)
{
    static uint8 change_mode=0;
    static int8 change_item_i=4;
    
    ips200_show_string(60,0,"User Param");//二级菜单标题
    ips200_show_string(16,30+16*0,"mid_angle:");//机械中值
    ips200_show_string(16,30+16*1,"target_speed:");//目标速度
    ips200_show_string(16,30+16*2,"gyro_i_out_max:");//角速度环i最大输出
    ips200_show_string(16,30+16*3,"gyro_out_max:");//角速度环最大输出
    ips200_show_string(16,30+16*4,"angle_out_max:");//角度环最大输出
    ips200_show_string(16,30+16*5,"speed_i_out_max:");//速度环i最大输出
    ips200_show_string(16,30+16*6,"speed_out_max:");//速度环最大输出
    ips200_show_string(16,30+16*7,"turn_out_max:");//转向环最大输出
    ips200_show_string(16,30+16*8,"err_start:");//误差起点
    ips200_show_string(16,30+16*9,"err_end:");//误差终点
    ips200_show_string(16,30+16*10,"back");//返回上一级菜单
    ips200_show_string(16,30+16*11,"change_item:");//修改量


    //显示数字
    ips200_show_float(150,30+16*0,user_param.mid_angle,4,3);
    ips200_show_float(150,30+16*1,user_param.target_speed,4,3);
    ips200_show_float(150,30+16*2,gyro_pid_param.PID_I_LIMIT_MAX,4,3);
    ips200_show_float(150,30+16*3,gyro_pid_param.PID_OUT_LIMIT_MAX,4,3);
    ips200_show_float(150,30+16*4,angle_pid_param.PID_OUT_LIMIT_MAX,4,3);
    ips200_show_float(150,30+16*5,speed_pid_param.PID_I_LIMIT_MAX,4,3);
    ips200_show_float(150,30+16*6,speed_pid_param.PID_OUT_LIMIT_MAX,4,3);
    ips200_show_float(150,30+16*7,turn_pid_param.PID_OUT_LIMIT_MAX,4,3);
    ips200_show_float(150,30+16*8,user_param.err_start,4,3);
    ips200_show_float(150,30+16*9,user_param.err_end,4,3);
    ips200_show_float(150,30+16*11,change_item_num[change_item_i],4,3);

    if(!change_mode)ips200_show_string(0,(sec_menu_item+1)*16,"->");//菜单指针
    else ips200_show_string(0,(sec_menu_item+1)*16,"*");

    if(change_mode==2)ips200_show_string(0,30+11*16,"*");

    key_scanner();//千万不要忘

    if(!change_mode)
    {
        if(KEY_SHORT_PRESS == key_get_state(KEY_1))
        {
            sec_menu_item--;
            if(sec_menu_item < 1) //如果副菜单指针小于1
            {
                sec_menu_item = 11; //则将副菜单指针置为11
            }
            key_clear_state(KEY_1); //清除按键状态
            ips200_clear();//清屏
        }

        if(KEY_SHORT_PRESS == key_get_state(KEY_2))
        {
            sec_menu_item++;
            if(sec_menu_item > 11)//如果副菜单指针大于11
            {
                sec_menu_item = 1; //则将副菜单指针置为1
            }
            key_clear_state(KEY_2);//清除按键状态
            ips200_clear();//清屏
        }
        //返回
        if((sec_menu_item == 11 && KEY_SHORT_PRESS == key_get_state(KEY_3))||KEY_SHORT_PRESS == key_get_state(KEY_4))
        {
            //返回主菜单
            main_menu_item = 1;
            sec_menu_item = 1;
            key_clear_state(KEY_3);//清除按键状态
            key_clear_state(KEY_4);//清除按键状态
            ips200_clear();//清屏
        }
        
        if(sec_menu_item>=1&&sec_menu_item<=10 && KEY_SHORT_PRESS == key_get_state(KEY_3))
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
                        user_param.mid_angle+= change_item_num[change_item_i];
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        user_param.mid_angle-= change_item_num[change_item_i];
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
                        user_param.target_speed+= change_item_num[change_item_i]; 
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        user_param.target_speed-= change_item_num[change_item_i];
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
                case 3:
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        gyro_pid_param.PID_I_LIMIT_MAX+= change_item_num[change_item_i]; 
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        gyro_pid_param.PID_I_LIMIT_MAX-= change_item_num[change_item_i];
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
                case 4:
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        gyro_pid_param.PID_OUT_LIMIT_MAX+= change_item_num[change_item_i]; 
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        gyro_pid_param.PID_OUT_LIMIT_MAX-= change_item_num[change_item_i];
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
                case 5:
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        angle_pid_param.PID_OUT_LIMIT_MAX+= change_item_num[change_item_i]; 
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        angle_pid_param.PID_OUT_LIMIT_MAX-= change_item_num[change_item_i];
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
                case 6:
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        speed_pid_param.PID_I_LIMIT_MAX+= change_item_num[change_item_i]; 
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        speed_pid_param.PID_I_LIMIT_MAX-= change_item_num[change_item_i];
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
                case 7:
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        speed_pid_param.PID_OUT_LIMIT_MAX+= change_item_num[change_item_i]; 
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        speed_pid_param.PID_OUT_LIMIT_MAX-= change_item_num[change_item_i];
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
                case 8:
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        turn_pid_param.PID_OUT_LIMIT_MAX+= change_item_num[change_item_i]; 
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        turn_pid_param.PID_OUT_LIMIT_MAX-= change_item_num[change_item_i];
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
                case 9:
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        user_param.err_start+= change_item_num[change_item_i]; 
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        user_param.err_start-= change_item_num[change_item_i];
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
                case 10:
                    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
                    {
                        user_param.err_end+= change_item_num[change_item_i]; 
                        key_clear_state(KEY_1); //清除按键状态
                        ips200_clear();//清屏
                    }
                    else if(KEY_SHORT_PRESS == key_get_state(KEY_2))
                    {
                        user_param.err_end-= change_item_num[change_item_i];
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


/**
* @brief  图像参数子菜单显示函数
* @param   无
*/
void image_menu(void)
{
	static uint8 image_flag=1;//图像显示类型
    //显示边线以及中线
    show_boundary_line();
    //显示字符
    ips200_show_string(0,0,"Real_image&binary_image:");
    ips200_show_string(0,150,"encoder_left:");
    ips200_show_string(0,150+16,"encoder_right:");
    ips200_show_string(0,150+16*2,"angle:");
    ips200_show_string(0,150+16*3,"image_threshold:");
    ips200_show_string(0,150+16*4,"line_err:");
    ips200_show_string(0,150+16*5,"left_down_point:");
    ips200_show_string(0,150+16*6,"right_down_point:");
    ips200_show_string(0,150+16*7,"left_up_point:");
    ips200_show_string(0,150+16*8,"right_up_point:");

    //显示数字
    ips200_show_int(150,150,encoder_data_left,3);
    ips200_show_int(150,150+16,encoder_data_right,3);
    ips200_show_float(150,150+16*2,filtering_angle,6,1);
    ips200_show_uint(150,150+16*3,image_threshold,3);
    ips200_show_float(150,150+16*4,line_err,6,1);
    ips200_show_uint(150,150+16*5,left_down_point,3);
    ips200_show_uint(150,150+16*6,right_down_point,3);
    ips200_show_uint(150,150+16*7,left_up_point,3);
    ips200_show_uint(150,150+16*8,right_up_point,3);

    key_clear_state(KEY_4);//清除按键状态
	key_scanner();//千万不要忘

    if(!image_flag)show_binary_image(0,30,image_threshold);
    else show_real_image(0,30);
    

    if(sec_menu_item == 1 && KEY_SHORT_PRESS == key_get_state(KEY_3))
    {
        //返回主菜单
        image_flag=!image_flag;
        key_clear_state(KEY_3);//清除按键状态
        ips200_clear();//清屏
    }
    //返回
    if(sec_menu_item == 1 && KEY_SHORT_PRESS == key_get_state(KEY_4))
    {
        //返回主菜单
        main_menu_item = 1;
        sec_menu_item = 1;
        key_clear_state(KEY_4);//清除按键状态
        ips200_clear();//清屏
    }
}

/**
* @brief  环岛参数子菜单显示函数
* @param   无
*/
void circle_param(void)
{
    static uint8 image_flag=1;//图像显示类型
    //显示边线以及中线
    show_boundary_line();

    //显示字符
    ips200_show_string(0,0,"right_circle_flag:");
    ips200_show_string(0,150,"boudary_start_left:");
    ips200_show_string(0,150+16,"boudary_start_right:");
    ips200_show_string(0,150+16*2,"left_change_line:");
    ips200_show_string(0,150+16*3,"right_change_line:");
    ips200_show_string(0,150+16*4,"continu_left");
    ips200_show_string(0,150+16*5,"continu_right");
    ips200_show_string(0,150+16*6,"search_stop_line");
    ips200_show_string(0,150+16*7,"left_lost_count:");
    ips200_show_string(0,150+16*8,"right_lost_count:");
    ips200_show_string(0,150+16*9,"right_down_point:");

    //显示数字
    ips200_show_uint(170,0,right_circle_flag,3);
    ips200_show_uint(170,150,boundary_start_left,3);
    ips200_show_uint(170,150+16,boundary_start_right,3);
    ips200_show_uint(170,150+16*2,left_change_line,3);
    ips200_show_uint(170,150+16*3,right_change_line,3); 
    ips200_show_uint(170,150+16*4,continuity_left_change_flag,3);
    ips200_show_uint(170,150+16*5,continuity_right_change_flag,3);
    ips200_show_uint(170,150+16*6,search_stop_line,3);
    ips200_show_uint(170,150+16*7,left_lost_count,3);
    ips200_show_uint(170,150+16*8,right_lost_count,3);
    ips200_show_uint(170,150+16*9,right_down_point,3);


    key_clear_state(KEY_4);//清除按键状态
	key_scanner();//千万不要忘

    if(!image_flag)show_binary_image(0,30,image_threshold);
    else show_real_image(0,30);
    

    if(sec_menu_item == 1 && KEY_SHORT_PRESS == key_get_state(KEY_3))
    {
        //返回主菜单
        image_flag=!image_flag;
        key_clear_state(KEY_3);//清除按键状态
        ips200_clear();//清屏
    }
    //返回
    if(sec_menu_item == 1 && KEY_SHORT_PRESS == key_get_state(KEY_4))
    {
        //返回主菜单
        main_menu_item = 1;
        sec_menu_item = 1;
        key_clear_state(KEY_4);//清除按键状态
        ips200_clear();//清屏
    }
}




// // 菜单类型枚举
// typedef enum
// {
//     MENU_TYPE_MENU = 0,     // 普通菜单
//     MENU_TYPE_ACTION,       // 动作项
//     MENU_TYPE_VALUE,        // 数值修改项
//     MENU_TYPE_SWITCH,       // 开关项
//     MENU_TYPE_DISPLAY       // 显示项
// } menu_type_t;

// // 菜单项结构体
// typedef struct menu_item
// {
//     char *name;                     // 菜单项名称
//     menu_type_t type;               // 菜单类型
//     void *value;                    // 指向变量的指针
//     float min_val;                  // 最小值（数值类型）
//     float max_val;                  // 最大值（数值类型）
//     float step;                     // 步长（数值类型）
//     void (*action)(void);           // 动作函数指针
//     struct menu_item *parent;       // 父菜单
//     struct menu_item *child;        // 子菜单
//     struct menu_item *next;         // 下一个菜单项
//     struct menu_item *prev;         // 上一个菜单项
// } menu_item_t;

// // 全局变量
// static menu_item_t *current_menu = NULL;   // 当前菜单
// static menu_item_t *current_item = NULL;   // 当前项
// static uint8 edit_mode = 0;                // 编辑模式
// static uint8 step_index = 4;               // 步长索引
// static uint8 menu_refresh = 1;             // 刷新标志

// // 动作函数声明
// void action_car_go(void);
// void action_save_param(void);
// void action_load_param(void);

// /**
// *
// * @brief  创建菜单项
// **/
// menu_item_t* create_menu_item(char *name, menu_type_t type, void *value, 
//                               float min_val, float max_val, float step, 
//                               void (*action)(void))
// {
//     menu_item_t *item = (menu_item_t*)malloc(sizeof(menu_item_t));
//     if (item == NULL) return NULL;
    
//     item->name = name;
//     item->type = type;
//     item->value = value;
//     item->min_val = min_val;
//     item->max_val = max_val;
//     item->step = step;
//     item->action = action;
//     item->parent = NULL;
//     item->child = NULL;
//     item->next = NULL;
//     item->prev = NULL;
    
//     return item;
// }

// /**
// *
// * @brief  添加子菜单
// **/
// void add_child_menu(menu_item_t *parent, menu_item_t *child)
// {
//     if (parent == NULL || child == NULL) return;
    
//     child->parent = parent;
    
//     if (parent->child == NULL)
//     {
//         parent->child = child;
//     }
//     else
//     {
//         menu_item_t *temp = parent->child;
//         while (temp->next != NULL)
//         {
//             temp = temp->next;
//         }
//         temp->next = child;
//         child->prev = temp;
//     }
// }
// int count = 0;
// float mid_angle_1 = 0.0f;
// float target_speed_1 = 0.0f;
// float error_start_1 = 0.0f;
// float error_end_1 = 0.0f;
// /**
// *
// * @brief  初始化链表菜单
// **/
// void linked_menu_init(void)
// {
//     // 创建主菜单
//     menu_item_t *main_menu = create_menu_item("Main Menu", MENU_TYPE_MENU, NULL, 0, 0, 0, NULL);
    
//     // 创建主菜单项
//     menu_item_t *car_go_item = create_menu_item("Car Go", MENU_TYPE_ACTION, NULL, 0, 0, 0, action_car_go);
//     menu_item_t *pid_menu = create_menu_item("PID", MENU_TYPE_MENU, NULL, 0, 0, 0, NULL);
//     menu_item_t *param_menu = create_menu_item("Parameters", MENU_TYPE_MENU, NULL, 0, 0, 0, NULL);
//     menu_item_t *image_menu = create_menu_item("Image Param", MENU_TYPE_MENU, NULL, 0, 0, 0, NULL);
//     menu_item_t *circle_menu = create_menu_item("Circle Param", MENU_TYPE_MENU, NULL, 0, 0, 0, NULL);
//     menu_item_t *save_item = create_menu_item("Save Param", MENU_TYPE_ACTION, NULL, 0, 0, 0, action_save_param);
//     menu_item_t *load_item = create_menu_item("Load Param", MENU_TYPE_ACTION, NULL, 0, 0, 0, action_load_param);

//     // 添加到主菜单
//     add_child_menu(main_menu, car_go_item);
//     add_child_menu(main_menu, pid_menu);
//     add_child_menu(main_menu, param_menu);
//     add_child_menu(main_menu, image_menu);
//     add_child_menu(main_menu, circle_menu);
//     add_child_menu(main_menu, save_item);
//     add_child_menu(main_menu, load_item);

//     menu_item_t *gyro_menu = create_menu_item("Gyro PID", MENU_TYPE_MENU, NULL, 0, 0, 0, NULL);
//     menu_item_t *angle_menu = create_menu_item("Angle PID", MENU_TYPE_MENU, NULL, 0, 0, 0, NULL);
//     menu_item_t *speed_menu = create_menu_item("Speed PID", MENU_TYPE_MENU, NULL, 0, 0, 0, NULL);
//     menu_item_t *turn_menu = create_menu_item("Turn PID", MENU_TYPE_MENU, NULL, 0, 0, 0, NULL);

    
//     // 创建角速度环PID子菜单
//     menu_item_t *gyro_kp = create_menu_item("Gyro Kp", MENU_TYPE_VALUE, &gyro_pid_param.kp, 0, 1000, 0.001f, NULL);
//     menu_item_t *gyro_ki = create_menu_item("Gyro Ki", MENU_TYPE_VALUE, &gyro_pid_param.ki, 0, 1000, 0.001f, NULL);
//     add_child_menu(gyro_menu, gyro_kp);
//     add_child_menu(gyro_menu, gyro_ki);
    
//     // 创建角度环PID子菜单
//     menu_item_t *angle_kp = create_menu_item("Angle Kp", MENU_TYPE_VALUE, &angle_pid_param.kp, 0, 1000, 0.001f, NULL);
//     menu_item_t *angle_kd = create_menu_item("Angle Kd", MENU_TYPE_VALUE, &angle_pid_param.kd, 0, 1000, 0.001f, NULL);
//     add_child_menu(angle_menu, angle_kp);
//     add_child_menu(angle_menu, angle_kd);
    
//     // 创建速度环PID子菜单
//     menu_item_t *speed_kp = create_menu_item("Speed Kp", MENU_TYPE_VALUE, &speed_pid_param.kp, 0, 1000, 0.001f, NULL);
//     menu_item_t *speed_ki = create_menu_item("Speed Ki", MENU_TYPE_VALUE, &speed_pid_param.ki, 0, 1000, 0.001f, NULL);
//     add_child_menu(speed_menu, speed_kp);
//     add_child_menu(speed_menu, speed_ki);
    
//     // 创建转向环PID子菜单
//     menu_item_t *turn_kp = create_menu_item("Turn Kp", MENU_TYPE_VALUE, &turn_pid_param.kp, 0, 1000, 0.001f, NULL);
//     menu_item_t *turn_kp2 = create_menu_item("Turn Kp2", MENU_TYPE_VALUE, &turn_pid_param.kp2, 0, 1000, 0.001f, NULL);
//     menu_item_t *turn_kd = create_menu_item("Turn Kd", MENU_TYPE_VALUE, &turn_pid_param.kd, 0, 1000, 0.001f, NULL);
//     menu_item_t *turn_kd2 = create_menu_item("Turn Kd2", MENU_TYPE_VALUE, &turn_pid_param.kd2, 0, 1000, 0.001f, NULL);
//     add_child_menu(turn_menu, turn_kp);
//     add_child_menu(turn_menu, turn_kp2);
//     add_child_menu(turn_menu, turn_kd);
//     add_child_menu(turn_menu, turn_kd2);

//     // 添加PID子菜单到PID菜单
//     add_child_menu(pid_menu, gyro_menu);
//     add_child_menu(pid_menu, angle_menu);
//     add_child_menu(pid_menu, speed_menu);
//     add_child_menu(pid_menu, turn_menu);
    
//     // 创建常用参数子菜单
//     menu_item_t *mid_angle = create_menu_item("Mid Angle", MENU_TYPE_VALUE, &mid_angle_1, -5000, 5000, 0.1f, NULL);
//     menu_item_t *target_speed = create_menu_item("Target Speed", MENU_TYPE_VALUE, &target_speed_1, 0, 2000, 0.1f, NULL);
//     menu_item_t *err_start = create_menu_item("Err Start", MENU_TYPE_VALUE, &error_start_1, 0, 120, 0.1f, NULL);
//     menu_item_t *err_end = create_menu_item("Err End", MENU_TYPE_VALUE, &error_end_1, 0, 120, 0.1f, NULL);
//     add_child_menu(param_menu, mid_angle);
//     add_child_menu(param_menu, target_speed);
//     add_child_menu(param_menu, err_start);
//     add_child_menu(param_menu, err_end);

    
//     // 创建图像参数子菜单（显示项）
//     menu_item_t *encoder_left_display = create_menu_item("Encoder L", MENU_TYPE_DISPLAY, &encoder_data_left, 0, 0, 0, NULL);
//     menu_item_t *encoder_right_display = create_menu_item("Encoder R", MENU_TYPE_DISPLAY, &encoder_data_right, 0, 0, 0, NULL);
//     menu_item_t *angle_display = create_menu_item("Angle", MENU_TYPE_DISPLAY, &filtering_angle, 0, 0, 0, NULL);
//     menu_item_t *threshold_display = create_menu_item("Threshold", MENU_TYPE_DISPLAY, &image_threshold, 0, 0, 0, NULL);
//     menu_item_t *line_err_display = create_menu_item("Line Err", MENU_TYPE_DISPLAY, &line_err, 0, 0, 0, NULL);
//     add_child_menu(image_menu, encoder_left_display);
//     add_child_menu(image_menu, encoder_right_display);
//     add_child_menu(image_menu, angle_display);
//     add_child_menu(image_menu, threshold_display);
//     add_child_menu(image_menu, line_err_display);
    
//     // 创建环岛参数子菜单（显示项）
//     menu_item_t *circle_flag_display = create_menu_item("Circle Flag", MENU_TYPE_DISPLAY, &right_circle_flag, 0, 0, 0, NULL);
//     menu_item_t *boundary_left_display = create_menu_item("Boundary L", MENU_TYPE_DISPLAY, &boundary_start_left, 0, 0, 0, NULL);
//     menu_item_t *boundary_right_display = create_menu_item("Boundary R", MENU_TYPE_DISPLAY, &boundary_start_right, 0, 0, 0, NULL);
//     menu_item_t *left_lost_display = create_menu_item("Left Lost", MENU_TYPE_DISPLAY, &left_lost_count, 0, 0, 0, NULL);
//     menu_item_t *right_lost_display = create_menu_item("Right Lost", MENU_TYPE_DISPLAY, &right_lost_count, 0, 0, 0, NULL);
//     add_child_menu(circle_menu, circle_flag_display);
//     add_child_menu(circle_menu, boundary_left_display);
//     add_child_menu(circle_menu, boundary_right_display);
//     add_child_menu(circle_menu, left_lost_display);
//     add_child_menu(circle_menu, right_lost_display);
    
//     // 设置当前菜单和项
//     current_menu = main_menu;
//     current_item = main_menu->child;
// }

// /**
// *
// * @brief  链表菜单显示函数
// **/
// void linked_menu_display(void)
// {
//     if (current_menu == NULL || current_item == NULL) return;
    
//     if (menu_refresh)
//     {
//         ips200_clear();
//         menu_refresh = 0;
//     }
    
//     // 显示菜单标题
//     ips200_show_string(60, 0, current_menu->name);
    
//     // 显示菜单项
//     menu_item_t *item = current_menu->child;
//     uint8 line = 0;
    
//     while (item != NULL && line < 10)
//     {
//         uint16 y_pos = 30 + line * 16;
        
//         // 显示菜单项名称
//         ips200_show_string(16, y_pos, item->name);
        
//         // 显示值或状态
//         switch (item->type)
//         {
//             case MENU_TYPE_VALUE:
//                 if (item->value != NULL)
//                 {
//                     float *val = (float*)item->value;
//                     ips200_show_float(150, y_pos, *val, 4, 3);
//                 }
//                 break;
                
//             case MENU_TYPE_DISPLAY:
//                 if (item->value != NULL)
//                 {
//                     if (item == current_item && strstr(item->name, "Encoder") != NULL)
//                     {
//                         int16 *val = (int16*)item->value;
//                         ips200_show_int(150, y_pos, *val, 4);
//                     }
//                     else if (item == current_item && strstr(item->name, "Angle") != NULL)
//                     {
//                         float *val = (float*)item->value;
//                         ips200_show_float(150, y_pos, *val, 6, 2);
//                     }
//                     else if (item == current_item && (strstr(item->name, "Flag") != NULL || 
//                                                      strstr(item->name, "Boundary") != NULL ||
//                                                      strstr(item->name, "Lost") != NULL))
//                     {
//                         uint8 *val = (uint8*)item->value;
//                         ips200_show_uint(150, y_pos, *val, 3);
//                     }
//                     else
//                     {
//                         float *val = (float*)item->value;
//                         ips200_show_float(150, y_pos, *val, 6, 2);
//                     }
//                 }
//                 break;
                
//             case MENU_TYPE_ACTION:
//                 ips200_show_string(150, y_pos, "[Action]");
//                 break;
                
//             case MENU_TYPE_MENU:
//                 ips200_show_string(150, y_pos, "[Menu]");
//                 break;
//         }
        
//         // 显示指针
//         if (item == current_item)
//         {
//             if (edit_mode)
//             {
//                 ips200_show_string(0, y_pos, "*");
//             }
//             else
//             {
//                 ips200_show_string(0, y_pos, "->");
//             }
//         }
        
//         item = item->next;
//         line++;
//     }
    
//     // 显示步长（编辑模式下）
//     if (edit_mode && current_item->type == MENU_TYPE_VALUE)
//     {
//         ips200_show_string(16, 30 + 11 * 16, "Step:");
//         ips200_show_float(150, 30 + 11 * 16, change_item_num[step_index], 4, 3);
//     }
    
//     // 显示帮助信息
//     ips200_show_string(0, 220, "K1:Up K2:Down K3:Enter K4:Back");
// }

// /**
// *
// * @brief  链表菜单按键处理
// **/
// void linked_menu_key_process(void)
// {
//     key_scanner();
    
//     if (!edit_mode)
//     {
//         // 普通模式按键处理
//         if (KEY_SHORT_PRESS == key_get_state(KEY_1))
//         {
//             // 上移
//             if (current_item->prev != NULL)
//             {
//                 current_item = current_item->prev;
//             }
//             else
//             {
//                 // 移动到最后一项
//                 menu_item_t *last = current_item;
//                 while (last->next != NULL)
//                 {
//                     last = last->next;
//                 }
//                 current_item = last;
//             }
//             key_clear_state(KEY_1);
//             menu_refresh = 1;
//         }
        
//         if (KEY_SHORT_PRESS == key_get_state(KEY_2))
//         {
//             // 下移
//             if (current_item->next != NULL)
//             {
//                 current_item = current_item->next;
//             }
//             else
//             {
//                 // 移动到第一项
//                 current_item = current_menu->child;
//             }
//             key_clear_state(KEY_2);
//             menu_refresh = 1;
//         }
        
//         if (KEY_SHORT_PRESS == key_get_state(KEY_3))
//         {
//             // 进入/执行
//             switch (current_item->type)
//             {
//                 case MENU_TYPE_MENU:
//                     if (current_item->child != NULL)
//                     {
//                         current_menu = current_item;
//                         current_item = current_item->child;
//                         menu_refresh = 1;
//                     }
//                     break;
                    
//                 case MENU_TYPE_ACTION:
//                     if (current_item->action != NULL)
//                     {
//                         current_item->action();
//                     }
//                     menu_refresh = 1;
//                     break;
                    
//                 case MENU_TYPE_VALUE:
//                     edit_mode = 1;
//                     menu_refresh = 1;
//                     break;
//             }
//             key_clear_state(KEY_3);
//         }
        
//         if (KEY_SHORT_PRESS == key_get_state(KEY_4))
//         {
//             // 返回
//             if (current_menu->parent != NULL)
//             {
//                 current_item = current_menu;
//                 current_menu = current_menu->parent;
//                 menu_refresh = 1;
//             }
//             key_clear_state(KEY_4);
//         }
//     }
//     else
//     {
//         // 编辑模式按键处理
//         if (current_item->type == MENU_TYPE_VALUE && current_item->value != NULL)
//         {
//             float *val = (float*)current_item->value;
            
//             if (KEY_SHORT_PRESS == key_get_state(KEY_1))
//             {
//                 *val += change_item_num[step_index];
//                 if (*val > current_item->max_val && current_item->max_val > current_item->min_val)
//                 {
//                     *val = current_item->max_val;
//                 }
//                 key_clear_state(KEY_1);
//                 menu_refresh = 1;
//             }
            
//             if (KEY_SHORT_PRESS == key_get_state(KEY_2))
//             {
//                 *val -= change_item_num[step_index];
//                 if (*val < current_item->min_val && current_item->max_val > current_item->min_val)
//                 {
//                     *val = current_item->min_val;
//                 }
//                 key_clear_state(KEY_2);
//                 menu_refresh = 1;
//             }
            
//             if (KEY_SHORT_PRESS == key_get_state(KEY_3))
//             {
//                 // 切换步长
//                 step_index++;
//                 if (step_index >= 7) step_index = 0;
//                 key_clear_state(KEY_3);
//                 menu_refresh = 1;
//             }
//         }
        
//         if (KEY_SHORT_PRESS == key_get_state(KEY_4))
//         {
//             // 退出编辑模式
//             edit_mode = 0;
//             key_clear_state(KEY_4);
//             menu_refresh = 1;
//         }
//     }
// }

// /**
// *
// * @brief  动作函数实现
// **/
// void action_car_go(void)
// {
//     beep_on();
//     car_go = 1;
//     ips200_show_string(60, 200, "Car Started!");
//     system_delay_ms(1000);
// }

// void action_save_param(void)
// {
//     menu_save();
//     ips200_show_string(60, 200, "Param Saved!");
//     system_delay_ms(1000);
// }

// void action_load_param(void)
// {
//     menu_load();
//     ips200_show_string(60, 200, "Param Loaded!");
//     system_delay_ms(1000);
// }

// /**
// *
// * @brief  链表菜单主函数
// **/
// void linked_menu_main(void)
// {
//     linked_menu_display();
//     linked_menu_key_process();
// }

// /**
// *
// * @brief  替换原有的menu函数
// **/
// void menu(void)
// {
//     linked_menu_main();
// }







