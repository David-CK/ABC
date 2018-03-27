#ifndef __COMMON_H
#define __COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include<sys/time.h>
//#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include "Log.h"
//#include "XMLParse.h"
#include <cstdarg> 

#define DOOR_CLOSE 0
#define DOOR_OPEN 1
#define MAX_CAMERA_NUM 4   //最大使用相机数量
#define PLANT_CAMERA_NUM 4  //平板上的相机数量，需抓图相机数量
#define LOG_PATH	"/tmp/camserver.log"
#define CONFILGFILE_PATH    "/home/nqx/code/fridge_client/cameraServer/device.conf"
#define PARAM_LEN		256
#define STR_MAX_LEN		256

#define TEST_SERV											"@/fridge/test_serv"
#define CAM_SERV											"../socketfile/cam_socket"

#ifdef __cplusplus
extern "C"
{
#endif
inline int loginfo(char* params, ...);
int connectLinuxSocket(const char* unixsockaddr, char* msginfo);
void* listenconnect(void* param);
#ifdef __cplusplus
}
#endif
#endif

