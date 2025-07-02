#ifndef _MENU_H
#define _MENU_H


#include "zf_common_headfile.h"

void menu_init(void);

void menu(void);

void main_menu(void);

void Sec_Menu_01(void);

void Sec_Menu_02(void);

void Sec_Menu_03(void);

void Sec_Menu_04(void);

void Sec_Menu_05(void);

void image_menu(void);

extern uint8 car_go;

extern uint8 main_menu_item;

extern uint8 sec_menu_item;

#endif