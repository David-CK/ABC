// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

///////////////////////////////////////////////////////////
// librealsense tutorial #2 - Accessing multiple streams //
///////////////////////////////////////////////////////////

// First include the librealsense C++ header file
#include <cstdio>

#include <fcntl.h>
#include <termios.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include <fstream>
#include <sstream>
#include "SerialPort.hpp"

int set_interface_attribs ()
{
	int serial_fd;
	struct termios tios;

	serial_fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

	cfsetispeed(&tios, B9600);
	cfsetospeed(&tios, B9600);
	tios.c_cflag &=  CS8;
    tios.c_cflag &= ~PARENB;
	tios.c_cflag &= ~CSTOPB;

    // as \000 chars
    //tios.c_iflag &= ~IGNBRK;         // disable break processing
    //tios.c_lflag = 0;                // no signaling chars, no echo, no canonical processing
    //tios.c_oflag = 0;                // no remapping, no delays
    //tios.c_cc[VMIN]  = 1;            // read doesn't block
    //tios.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
    //tios.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    //tios.c_cflag |= (CLOCAL | CREAD);// ignore modem controls, 
    //tios.c_cflag &= ~(PARENB | PARODD);      // shut off parity

	tcsetattr(serial_fd, TCSANOW, &tios);
    //tcflush(serial_fd,TCIOFLUSH);
	return serial_fd;
}

bool IsRecieveDoorOpen = false;
int iRecieveDoorOpenDoorCloseCount = 0;
bool IsDoorOpen = false;
int iDoorCloseCount = 0;
int iBadSerialCount = 0;

int main()
{
    string strModemDevice = "/dev/";
    strModemDevice.append((char*)"ttyUSB0"); //在string後面加上一個char*字串
    SerialPort* serialPort = new SerialPort(strModemDevice, SerialPort::BR9600);

 //    unsigned char door_open[] = {0x80, 0x01, 0x01, 0x00, 0x80};
 //    unsigned char door_close[] = {0x80, 0x01, 0x01, 0x11, 0x91};
    string door_open = "\x80";
    string door_close = "\x91";

    int count = 0;
    if(serialPort->isOpen())
    {//開檔成功
        while (true)
        {
            //把 Hex String 轉換到 Byte Array
            string strSendOpenDoor = "\x8A\x01\x01\x11\x9B";
            string strSendCheck = "\x80\x01\x01\x33\xB3";
            string strOpenDoorSucces = "\x80\x01\x01\xEE\x6E";

            //送出 Byte Array 資料
            if (count == 50)
            {
                serialPort->Send(strSendOpenDoor);
                string strRx_temp = serialPort->Recv();
                printf("what %d \n", strRx_temp.length());
                
                if (strRx_temp == strOpenDoorSucces)
                {
                    printf("aaa\n");
                    //message out door open success
                }
                else
                {
                                   printf("bbb\n");
                    for (int i = 0; i < strRx_temp.length(); i++)
                    {

                        printf("%x ", strRx_temp[i]);
                    }
                    //message out door open failed
                }
                sleep(0.1);
            }
            serialPort->Send(strSendCheck);
          
            int status = -1;
            string strRx = serialPort->Recv();
            if (strRx[strRx.length() - 1] == door_open[0])
            {
                status = 1;
                if (IsRecieveDoorOpen)
                {
                    IsDoorOpen = true;
                    IsRecieveDoorOpen = false;
                }
            }
            else if (strRx[strRx.length() - 1] == door_close[0])
            {
                status = 0;
            }

            if (IsDoorOpen)
            {
                if (iDoorCloseCount > 3)
                {
                    IsDoorOpen = false;
                    iDoorCloseCount = 0;
                     //socket message out, door close
                }
                if (status == 0)
                {
                    iDoorCloseCount++;
                }
                if (status == 1)
                {
                    iDoorCloseCount = 0;
                }
            }
            else
            {
                if (IsRecieveDoorOpen)
                {
                    if (iRecieveDoorOpenDoorCloseCount > 60)
                    {
                        IsDoorOpen = false;
                        iDoorCloseCount = 0; 
                        IsRecieveDoorOpen = false;
                        iRecieveDoorOpenDoorCloseCount = 0;
                        //socket message out, door close
                    }
                    iRecieveDoorOpenDoorCloseCount++;
                }
            }

            if (status == -1)
            {
                if (iBadSerialCount > 5)
                {
                    //socket message out, something wrong with serial
                    iBadSerialCount = 0;
                }
                iBadSerialCount++;
            }
            else
            {
                iBadSerialCount = 0;
            }

            sleep(0.1);
            count++;
        }
    }
    else
    {//開檔失敗
        cout << "Fail!" << endl; 
    }

    serialPort->Close();
    delete(serialPort);

    return 0;
}