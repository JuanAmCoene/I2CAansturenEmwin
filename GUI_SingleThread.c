#include "cmsis_os2.h"
#include "GUI.h"
#include "DIALOG.h"
#include "Board_LED.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h" 
#include "Driver_I2C.h"
#include <stdint.h> 
#include "temp.h"

#define ID_FRAMEWIN_0   (GUI_ID_USER + 0x00)
#define ID_MULTIEDIT_0  (GUI_ID_USER + 0x01)
#define ID_BUTTON_0     (GUI_ID_USER + 0x02)
#define ID_SLIDER_0     (GUI_ID_USER + 0x04)
#define ID_EDIT_0       (GUI_ID_USER + 0x06)

extern WM_HWIN CreateLogViewer(void);

extern int fanon;
extern int automatic;
extern uint8_t currentTemp;
extern uint8_t setTemp;

char bufferCurrent[3];
char bufferSet[3];

#define GUI_THREAD_STK_SZ    (2048U)

static void         GUIThread (void *argument);         /* thread function */
static osThreadId_t GUIThread_tid;                      /* thread id */
static uint64_t     GUIThread_stk[GUI_THREAD_STK_SZ/8]; /* thread stack */

static const osThreadAttr_t GUIThread_attr = {
  .stack_mem  = &GUIThread_stk[0],
  .stack_size = sizeof(GUIThread_stk),
  .priority   = osPriorityIdle 
};

// Function prototypes
void MX_I2C1_Init(void);


int Init_GUIThread(void) {
  GUIThread_tid = osThreadNew(GUIThread, NULL, &GUIThread_attr);
  if (GUIThread_tid == NULL) {
    return -1;
  }
  return 0;
}

__NO_RETURN static void GUIThread(void *argument) {
  (void)argument;
  WM_HWIN hWin, hItem, hItem1;

  GUI_Init();
  hWin = CreateLogViewer();
  hItem = WM_GetDialogItem(hWin, ID_MULTIEDIT_0);
  hItem1 = WM_GetDialogItem(hWin, ID_EDIT_0);



  while (1) {

		Temp_Read (&currentTemp);
    sprintf(bufferCurrent, "%d", currentTemp);
    MULTIEDIT_SetText(hItem, bufferCurrent);

    sprintf(bufferSet, "%d", setTemp);
    EDIT_SetText(hItem1, bufferSet);
		
		
		if(automatic == 1){
		if(currentTemp>setTemp){
			fanon=1;
		}else{
		fanon=0;
			}
		}

    GUI_TOUCH_Exec();
    GUI_Exec();
    GUI_X_ExecIdle();

    if (fanon == 1) {
      LED_On(1);
    } else {
      LED_Off(1);
    }

    osThreadYield();  // Yield to allow other tasks to run
  }
}

