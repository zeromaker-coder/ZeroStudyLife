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
    if(main_menu_item==2)Sec_Menu_01();//角速度环PI菜单
    if(main_menu_item==3)Sec_Menu_02();//角度环PD菜单
    if(main_menu_item==4)Sec_Menu_03();//速度环PI菜单
    if(main_menu_item==5)Sec_Menu_04();//转向环PPDD菜单
    if(main_menu_item==6)Sec_Menu_05();//用户常用参数
    if(main_menu_item==7)image_menu();//图像菜单
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
    ips200_show_string(16,142,"save_param");//保存参数
    ips200_show_string(16,158,"load_param");//载入参数

    ips200_show_string(0,(sec_menu_item+1)*16,"->");//菜单指针
	
	key_scanner();//这个千万不要忘;
	
    if(KEY_SHORT_PRESS == key_get_state(KEY_1))
    {
        sec_menu_item--;
        if(sec_menu_item < 1) //如果副菜单指针小于1
        {
            sec_menu_item = 9; //则将副菜单指针置为9
        }
        key_clear_state(KEY_1); //清除按键状态
        ips200_clear(); //清屏
    }

    if(KEY_SHORT_PRESS == key_get_state(KEY_2))
    {
        sec_menu_item++;
        if(sec_menu_item > 9)//如果副菜单指针大于9
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
                beep_on();
                system_delay_ms(50);
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
                // 保存参数
                ips200_clear();
                ips200_show_string(60, 160, "Param Saved!");
                menu_save();
                beep_on();
                system_delay_ms(50);
                ips200_clear();
                break;
            case 9:
                // 载入参数
                ips200_clear();
                ips200_show_string(60, 160, "Param Loaded!");
                menu_load();
                beep_on();
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
    ips200_show_string(16,30+16*3,"turn_kp2:");//转向环kd2
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
    ips200_show_string(16,30+16*7,"turn_out_max");//转向环最大输出
    ips200_show_string(16,30+16*8,"back");//返回上一级菜单
    ips200_show_string(16,30+16*10,"change_item:");//修改量


    //显示数字
    ips200_show_float(150,30+16*0,user_param.mid_angle,4,3);
    ips200_show_float(150,30+16*1,user_param.target_speed,4,3);
    ips200_show_float(150,30+16*2,gyro_pid_param.PID_I_LIMIT_MAX,4,3);
    ips200_show_float(150,30+16*3,gyro_pid_param.PID_OUT_LIMIT_MAX,4,3);
    ips200_show_float(150,30+16*4,angle_pid_param.PID_OUT_LIMIT_MAX,4,3);
    ips200_show_float(150,30+16*5,speed_pid_param.PID_I_LIMIT_MAX,4,3);
    ips200_show_float(150,30+16*6,speed_pid_param.PID_OUT_LIMIT_MAX,4,3);
    ips200_show_float(150,30+16*7,turn_pid_param.PID_OUT_LIMIT_MAX,4,3);
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
                sec_menu_item = 9; //则将副菜单指针置为9
            }
            key_clear_state(KEY_1); //清除按键状态
            ips200_clear();//清屏
        }

        if(KEY_SHORT_PRESS == key_get_state(KEY_2))
        {
            sec_menu_item++;
            if(sec_menu_item > 9)//如果副菜单指针大于9
            {
                sec_menu_item = 1; //则将副菜单指针置为1
            }
            key_clear_state(KEY_2);//清除按键状态
            ips200_clear();//清屏
        }
        //返回
        if((sec_menu_item == 9 && KEY_SHORT_PRESS == key_get_state(KEY_3))||KEY_SHORT_PRESS == key_get_state(KEY_4))
        {
            //返回主菜单
            main_menu_item = 1;
            sec_menu_item = 1;
            key_clear_state(KEY_3);//清除按键状态
            key_clear_state(KEY_4);//清除按键状态
            ips200_clear();//清屏
        }
        
        if(sec_menu_item>=1&&sec_menu_item<=8 && KEY_SHORT_PRESS == key_get_state(KEY_3))
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



void image_menu(void)
{
	static uint8 image_flag=1;//图像显示类型
    //显示字符
    ips200_show_string(0,0,"Real_image&binary_image:");
    ips200_show_string(0,150,"encoder_left:");
    ips200_show_string(0,150+16,"encoder_right:");
    ips200_show_string(0,150+16*2,"angle:");
    ips200_show_string(0,150+16*3,"image_threshold:");

    //显示数字
    ips200_show_int(150,150,encoder_data_left,3);
    ips200_show_int(150,150+16,encoder_data_right,3);
    ips200_show_float(150,150+16*2,filtering_angle,3,1);
    ips200_show_uint(150,150+16*3,image_threshold,3);

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
