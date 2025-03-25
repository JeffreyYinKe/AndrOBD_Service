#include <jni.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <android/log.h>
#include <errno.h>
#include "data.h"


bool is_root() {
    int ret = system("su -c 'whoami'");
    LOGE("current UID: %d", ret);
    return ret == 0;  // Check if running as root
}

bool request_root_access() {
    int result = system("su -c 'chmod 666 /dev/uinput'");
    LOGE("%s-%d: result = %d", __func__, __LINE__, result);
    return  result == 0;
}

int create_virtual_gamepad() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        LOGE("Failed to open /dev/uinput: %s", strerror(errno));
        return -1;
    }

    // Enable gamepad buttons
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_A);
    ioctl(fd, UI_SET_KEYBIT, BTN_B);
    ioctl(fd, UI_SET_KEYBIT, BTN_X);
    ioctl(fd, UI_SET_KEYBIT, BTN_Y);

    // Enable gamepad axes
    ioctl(fd, UI_SET_EVBIT, EV_ABS);
    ioctl(fd, UI_SET_ABSBIT, ABS_X);
    ioctl(fd, UI_SET_ABSBIT, ABS_Y);
    ioctl(fd, UI_SET_ABSBIT, ABS_LT);
    ioctl(fd, UI_SET_ABSBIT, ABS_RT);

    // Configure the virtual device
    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    strncpy(usetup.name, "Virtual Gamepad", UINPUT_MAX_NAME_SIZE);
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;
    usetup.id.product = 0x5678;
    usetup.id.version = 1;

    // Set up ABS axis ranges
    struct uinput_abs_setup abs_setup;
    memset(&abs_setup, 0, sizeof(abs_setup));
    abs_setup.code = ABS_X;
    abs_setup.absinfo.minimum = -32768;
    abs_setup.absinfo.maximum = 32767;
    ioctl(fd, UI_ABS_SETUP, &abs_setup);

    abs_setup.code = ABS_Y;
    ioctl(fd, UI_ABS_SETUP, &abs_setup);

    abs_setup.code = ABS_LT;
    abs_setup.absinfo.minimum = 0;
    abs_setup.absinfo.maximum = 1023;
    ioctl(fd, UI_ABS_SETUP, &abs_setup);

    abs_setup.code = ABS_RT;
    ioctl(fd, UI_ABS_SETUP, &abs_setup);

    // Create the device
    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    LOGE("Virtual gamepad created successfully");
    return fd;
}

void destroy_virtual_gamepad(int fd) {
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
    LOGE("Virtual gamepad destroyed");
}

int send_input_event(int fd, uint16_t type, uint16_t code, int32_t value) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = type;
    ev.code = code;
    ev.value = value;
    ev.time.tv_sec = 0;
    ev.time.tv_usec = 0;

    if (write(fd, &ev, sizeof(ev)) < 0) {
        LOGE("Failed to write event: %s", strerror(errno));
        return -1;
    }
    return 0;
}

void send_sync_event(int fd) {
    send_input_event(fd, EV_SYN, SYN_REPORT, 0);
}

void simulate_joystick_movement(int fd, int (*movements)[2], size_t arr_size ) {


    for (int i = 0; i < arr_size; i++) {
        send_input_event(fd, EV_ABS, ABS_X, movements[i][0]);
        send_input_event(fd, EV_ABS, ABS_Y, movements[i][1]);
        send_sync_event(fd);
        usleep(30000);  // 30ms delay for smoother motion
    }
}

JNIEXPORT jint JNICALL
Java_com_fr3ts0n_ecu_gui_androbd_MainActivity_createVirtualGamepad(JNIEnv *env, jobject thiz) {
    return create_virtual_gamepad();
}

JNIEXPORT void JNICALL
Java_com_fr3ts0n_ecu_gui_androbd_MainActivity_destroyVirtualGamepad(JNIEnv *env, jobject thiz, jint fd) {
    destroy_virtual_gamepad(fd);
}

JNIEXPORT void JNICALL
Java_com_fr3ts0n_ecu_gui_androbd_MainActivity_sendButtonPress(JNIEnv *env, jobject thiz, jint fd, jint button) {
    send_input_event(fd, EV_KEY, button, 1); // Key press
    send_sync_event(fd);
    send_input_event(fd, EV_KEY, button, 0); // Key release
    send_sync_event(fd);
}

JNIEXPORT void JNICALL
Java_com_fr3ts0n_ecu_gui_androbd_MainActivity_sendJoystickMove(JNIEnv *env, jobject thiz, jint fd, jint axis, jint value) {
    send_input_event(fd, EV_ABS, axis, value);
    send_sync_event(fd);
}

JNIEXPORT void JNICALL
Java_com_fr3ts0n_ecu_gui_androbd_MainActivity_simulateJoystick(JNIEnv *env, jobject thiz, jint fd, jint actionCode) {
    simulate_joystick_movement(fd, turn_left, TURN_LEFT_SIZE);
}