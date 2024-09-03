#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"
#include "task.h"

#include "lvgl/lvgl.h"

osThreadId_t lvglTickHandle;
osThreadId_t lvglTimerHandle;
osThreadId_t defaultTaskHandle;

const osThreadAttr_t lvglTick_attributes = {
    .name = "lvglTick",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 4 * 1024,
};
const osThreadAttr_t lvglTimer_attributes = {
    .name = "lvglTimer",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 4 * 1024,
};
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

void LVGLTimer(void* argument);
void LVGLTick(void* argument);
void StartDefaultTask(void* argument);
extern void MX_USB_HOST_Init(void);
void MX_FREERTOS_Init(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char* pcTaskName);

void vApplicationIdleHook(void) {}
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char* pcTaskName) {}

void MX_FREERTOS_Init(void) {
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  lvglTickHandle = osThreadNew(LVGLTick, NULL, &lvglTick_attributes);
  lvglTimerHandle = osThreadNew(LVGLTimer, NULL, &lvglTimer_attributes);
}

void StartDefaultTask(void* argument) {
  MX_USB_HOST_Init();
  for (;;) {
    osDelay(1);
  }
}

void LVGLTimer(void* argument) {
  for (;;) {
    lv_timer_handler();
  }
}
void LVGLTick(void* argument) {
  for (;;) {
    lv_tick_inc(10);
    osDelay(10);
  }
}