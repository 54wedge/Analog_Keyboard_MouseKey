// HID
// https://www.usb.org/document-library/hid-usage-tables-112

// clang++ main.cpp -std=c++17 -O2 -lwooting_analog_sdk -framework ApplicationServices -o main.out -Wall

#include <iostream>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <cstring>
using namespace std;

#include <ApplicationServices/ApplicationServices.h>
#include "wooting-analog-wrapper.h"

CGEventRef null_event = CGEventCreate(NULL);
CGPoint pointer_location = CGEventGetLocation(null_event);
int mouse_x = int(pointer_location.x);
int mouse_y = int(pointer_location.y);
//pointer_location.x = int(pointer_location.x);
//pointer_location.y = int(pointer_location.y);

//int32_t scroll_up = 10;
//int32_t scroll_down = 10;
//int32_t mouse_up = 10;
//int32_t mouse_down = 10;
//int32_t mouse_left = 10;
//int32_t mouse_right = 10;

CGEventRef wheel_scroll_event = CGEventCreateScrollWheelEvent(
    NULL, kCGScrollEventUnitPixel, 1, 0);

CGEventRef mouse_move_event = CGEventCreateMouseEvent(
    NULL, kCGEventMouseMoved, pointer_location, kCGMouseButtonLeft);

CGEventRef mouse_left_down = CGEventCreateMouseEvent(
    NULL, kCGEventLeftMouseDown, pointer_location, kCGMouseButtonLeft);
CGEventRef mouse_left_drag = CGEventCreateMouseEvent(
    NULL, kCGEventLeftMouseDragged, pointer_location, kCGMouseButtonLeft);
CGEventRef mouse_left_up = CGEventCreateMouseEvent(
    NULL, kCGEventLeftMouseUp, pointer_location, kCGMouseButtonLeft);

CGEventRef mouse_right_down = CGEventCreateMouseEvent(
    NULL, kCGEventRightMouseDown, pointer_location, kCGMouseButtonRight);
//CGEventRef mouse_right_drag = CGEventCreateMouseEvent(q
//    NULL, kCGEventRightMouseDragged, pointer_location, kCGMouseButtonRight);
CGEventRef mouse_right_up = CGEventCreateMouseEvent(
    NULL, kCGEventRightMouseUp, pointer_location, kCGMouseButtonRight);

CGEventRef mouse_mid_down = CGEventCreateMouseEvent(
    NULL, kCGEventOtherMouseDown, pointer_location, kCGMouseButtonCenter);
//CGEventRef mouse_mid_drag = CGEventCreateMouseEvent(
//    NULL, kCGEventOtherMouseDragged, pointer_location, kCGMouseButtonCenter);
CGEventRef mouse_mid_up = CGEventCreateMouseEvent(
    NULL, kCGEventOtherMouseUp, pointer_location, kCGMouseButtonCenter);


// TODO non-linear value for precise control
int scroll_speed = 50;
int mouse_speed = 10;

float pres_9 = 0.;
float pres_3 = 0.;
float pres_8 = 0.;
float pres_5 = 0.;
float pres_4 = 0.;
float pres_6 = 0.;
float pres_0 = 0.;
float pres_plus = 0.;
float pres_dot = 0.;
float pres_7 = 0.;
float pres_1 = 0.;
float pres_2 = 0.;

bool is_loop = 1;

mutex move_mutex;
mutex click_mutex;
mutex loop_mutex;

void read_analog(){
    while(is_loop){
        pres_3 = wooting_analog_read_analog(0x5b);
        pres_9 = wooting_analog_read_analog(0x61);

        pres_8 = wooting_analog_read_analog(0x60);
        pres_5 = wooting_analog_read_analog(0x5d);
        pres_4 = wooting_analog_read_analog(0x5c);
        pres_6 = wooting_analog_read_analog(0x5e);

        pres_0 = wooting_analog_read_analog(0x62);
        pres_plus = wooting_analog_read_analog(0x57);
        pres_7 = wooting_analog_read_analog(0x5f);
        pres_1 = wooting_analog_read_analog(0x59);
        pres_2 = wooting_analog_read_analog(0x5a);

        usleep(16666/2);      // 120Hz
    }
}

// TODO find easier way to trigger dock
// TODO invert analog curve for precise control
void mouse_move(){
    int delta_x;
    int delta_y;
    while(is_loop){
        move_mutex.lock();
        delta_x = int((pres_6 - pres_4) * mouse_speed);
        delta_y = int((pres_5 - pres_8) * mouse_speed);
        if(delta_x != 0 || delta_y != 0){
            mouse_x += delta_x;
            mouse_y += delta_y;
            if(mouse_x > 1920){      // for a 1080p screen
                mouse_x = 1920;
            }
            else if(mouse_x < 0){
                mouse_x = 0;
            }
            if(mouse_y > 1080){
                mouse_y = 1080;
            }
            else if(mouse_y < 0){
                mouse_y = 0;
            }
            CGEventSetLocation(mouse_move_event, CGPointMake(mouse_x, mouse_y));
            //cout << CGEventGetIntegerValueField(wheel_scroll_event, kCGScrollWheelEventPointDeltaAxis1) << endl;
            CGEventPost(kCGHIDEventTap, mouse_move_event);
        }
        else{      // update the cursor location in case the mouse is moved
            null_event = CGEventCreate(NULL);
            pointer_location = CGEventGetLocation(null_event);
            mouse_x = int(pointer_location.x);
            mouse_y = int(pointer_location.y);
        }
        move_mutex.unlock();
        usleep(16666);        // 60Hz
    }
}

// TODO fix drag in safari
void left_button(float pres_0, bool &is_left_click, bool &is_left_hold){
    if(pres_0 >= 0.70 && is_left_click == 0){
        cout << "left click" << endl;

        CGEventSetLocation(mouse_left_down, CGPointMake(mouse_x, mouse_y));
        CGEventPost(kCGHIDEventTap, mouse_left_down);
        usleep(16666*10);
        is_left_click = 1;
    }
    else if(pres_0 >= 0.70 && is_left_click == 1){
        //is_left_click = 1;
//        if(is_left_hold == 0){
//            cout << "left click hold" << endl;
//            CGEventSetLocation(mouse_left_drag, CGPointMake(mouse_x, mouse_y));
//            CGEventPost(kCGHIDEventTap, mouse_left_drag);
//            is_left_hold = 1;
//        }
        cout << "left click hold" << endl;
        CGEventSetLocation(mouse_left_drag, CGPointMake(mouse_x, mouse_y));
        CGEventPost(kCGHIDEventTap, mouse_left_drag);
        is_left_hold = 1;
    }
    else if(pres_0 < 0.70 && is_left_click == 1){
        cout << "left release" << endl;

        CGEventSetLocation(mouse_left_up, CGPointMake(mouse_x, mouse_y));
        CGEventPost(kCGHIDEventTap, mouse_left_up);
        usleep(16666*10);
        is_left_click = 0;
        is_left_hold = 0;
    }
    else if(pres_0 < 0.70 && is_left_click == 0){
        // do nothing
        //cout << "no input" << endl;
        //is_left_click = 0;
    }
}

void right_button(float pres_plus, bool &is_right_click){
    if(pres_plus >= 0.70 && is_right_click == 0){
        cout << "right click" << endl;

        CGEventSetLocation(mouse_right_down, CGPointMake(mouse_x, mouse_y));
        CGEventPost(kCGHIDEventTap, mouse_right_down);
        usleep(16666*10);
        is_right_click = 1;
    }
    else if(pres_plus >= 0.70 && is_right_click == 1){
        //disale hold
//        cout << "right click hold" << endl;
//        CGEventSetLocation(mouse_right_drag, CGPointMake(mouse_x, mouse_y));
//        CGEventPost(kCGHIDEventTap, mouse_right_drag);
    }
    else if(pres_plus < 0.70 && is_right_click == 1){
        cout << "right release" << endl;

        CGEventSetLocation(mouse_right_up, CGPointMake(mouse_x, mouse_y));
        CGEventPost(kCGHIDEventTap, mouse_right_up);
        usleep(16666*10);
        is_right_click = 0;
    }
    else if(pres_plus < 0.70 && is_right_click == 0){
        // do nothing
        //cout << "no input" << endl;
        //is_right_click = 0;
    }
}

void mid_button(float pres_7, bool &is_mid_click){
    if(pres_7 >= 0.70 && is_mid_click == 0){
        cout << "mid click" << endl;

        CGEventSetLocation(mouse_mid_down, CGPointMake(mouse_x, mouse_y));
        CGEventPost(kCGHIDEventTap, mouse_mid_down);
        usleep(16666*10);
        is_mid_click = 1;
    }
    else if(pres_7 >= 0.70 && is_mid_click == 1){
        //disale hold
//        cout << "mid click hold" << endl;
//        CGEventSetLocation(mouse_mid_drag, CGPointMake(mouse_x, mouse_y));
//        CGEventPost(kCGHIDEventTap, mouse_mid_drag);
    }
    else if(pres_7 < 0.70 && is_mid_click == 1){
        cout << "mid release" << endl;

        CGEventSetLocation(mouse_mid_up, CGPointMake(mouse_x, mouse_y));
        CGEventPost(kCGHIDEventTap, mouse_mid_up);
        usleep(16666*10);
        is_mid_click = 0;
    }
    else if(pres_7 < 0.70 && is_mid_click == 0){
        // do nothing
        //cout << "no input" << endl;
        //is_mid_click = 0;
    }
}

// TODO finish double click
void double_click(){

}

// TODO finish triple click
void triple_click(){

}

// TODO finsh drag lock
void drag_lock(){

}

void scroll_wheel(float pres_9, float pres_3){
    int delta_scroll = int((pres_9 - pres_3) * scroll_speed);

    if(delta_scroll != 0){
        CGEventSetIntegerValueField(wheel_scroll_event, kCGScrollWheelEventPointDeltaAxis1, delta_scroll);
        //cout << CGEventGetIntegerValueField(wheel_scroll_event, kCGScrollWheelEventPointDeltaAxis1) << endl;
        CGEventPost(kCGHIDEventTap, wheel_scroll_event);
    }
}

void mouse_click(){
    bool is_left_click = 0;
    bool is_left_hold = 0;

    bool is_right_click = 0;
    bool is_mid_click = 0;
    while(is_loop){
        click_mutex.lock();
        left_button(pres_0, is_left_click, is_left_hold);
        right_button(pres_plus, is_right_click);
        mid_button(pres_7, is_mid_click);
        scroll_wheel(pres_9, pres_3);
        click_mutex.unlock();
        usleep(16666);     // 60Hz
    }
}

int main() {
    //int ret = 0;
    WootingAnalogResult res;

    float pres_num = 0.;

    unsigned int device_info_num = 0;
    device_info_num = wooting_analog_initialise();
    cout << device_info_num << endl;

    if(wooting_analog_is_initialised() && device_info_num > 0){
        res = wooting_analog_set_keycode_mode(WootingAnalog_KeycodeType::WootingAnalog_KeycodeType_HID);
        sleep(1);

        thread t1(read_analog);
        thread t2(mouse_move);
        thread t3(mouse_click);

        while(1){
            pres_num = wooting_analog_read_analog(0x53);
            if(pres_num > 0.9){
                is_loop = 0;
                break;
            }
            usleep(16666*4);     // 15Hz
        }

        t1.join();
        t2.join();
        t3.join();
    }

    CFRelease(null_event);
    CFRelease(wheel_scroll_event);
    CFRelease(mouse_move_event);

    CFRelease(mouse_left_down);
    CFRelease(mouse_left_drag);
    CFRelease(mouse_left_up);

    CFRelease(mouse_right_down);
    //CFRelease(mouse_right_drag);
    CFRelease(mouse_right_up);

    CFRelease(mouse_mid_down);
    //CFRelease(mouse_mid_drag);
    CFRelease(mouse_mid_up);
    return 0;
}
