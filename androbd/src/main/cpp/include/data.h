#ifndef WORING_DATA_H
#define WORING_DATA_H
#include <linux/uinput.h>

#define LOG_TAG "VirtualGamepad"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define ABS_LT ABS_Z
#define ABS_RT ABS_RZ

extern int turn_left[][2];
extern int turn_right[][2];
extern const int TURN_LEFT_SIZE;
extern const int TURN_RIGHT_SIZE;


#endif //WORING_DATA_H
