#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
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


#define DOOR_CLOSE 0
#define DOOR_OPEN 1

#define SERVER_TEST	"./server_test"
#define LOCK_TEST	"./lock_test"

int cmd_int = 0;

int connectLinuxSocket(const char* unixsockaddr, char* msginfo);
void* listenconnect(void* param);

int connectLinuxSocket(const char* unixsockaddr, char* msginfo)
{
    int len;
    int clientfd = socket (AF_UNIX, SOCK_STREAM, 0); 
    if(!unixsockaddr || !msginfo)
    {
        return -1;
    }
    
    if(clientfd < 0)
    {
        return -1;
    }
    struct sockaddr_un myaddr;
//    printf("connectUnixSocket, clientfd = %d\n", clientfd);
    memset (&myaddr, 0, sizeof(myaddr));
    myaddr.sun_family = AF_UNIX;
    strncpy (myaddr.sun_path, unixsockaddr,strlen(unixsockaddr)); //Ì×œÓ×ÖµÄÃû×Ö£¬ÈÎÒâÈ¡
//    myaddr.sun_path[0] = 0;
    if(connect (clientfd, (struct sockaddr*)&myaddr, sizeof(myaddr)) == -1)
    {
 //       printf("connect to master socket fail!!!\n");
        goto EXIT_CONN;
    }

    len = send(clientfd, msginfo, 16,0);
    if(len <= 0 )
    {
  //      printf("send failed to master socket !!\n");
        goto EXIT_CONN;
    }
   // printf("send msg succee!\n");
    close(clientfd);
    return 0;
    EXIT_CONN:
    close(clientfd);
    return -1;
}

void* listenconnect(void* param)
{
    char cmd[128] = {0};
    pthread_detach(pthread_self());
    int listening_sockfd = socket (AF_UNIX, SOCK_STREAM, 0);
    //printf("listening_sockfd = %d\n", listening_sockfd);
    fcntl(listening_sockfd, F_SETFL, O_NONBLOCK);
    struct sockaddr_un myaddr;
    memset (&myaddr, 0, sizeof(myaddr));
    myaddr.sun_family = AF_UNIX;
    strcpy (myaddr.sun_path, LOCK_TEST); 
    //myaddr.sun_path[0] = 0;
    sprintf(cmd,"rm %s",LOCK_TEST);
    system(cmd);
    bind (listening_sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
    listen(listening_sockfd, 50);
    fd_set watchset, listenset, clientset;
    FD_ZERO(&clientset);
    FD_ZERO(&watchset);
    FD_ZERO(&listenset);
    //FD_SET(listening_sockfd, &listenset);
    FD_SET(listening_sockfd, &clientset);
    int max_listen = listening_sockfd + 1;
    int max_client = 0;
    int listen;
    unsigned long connect_restricted = 1000;
    unsigned long long live_conn = 0;
    struct sockaddr addr;
    socklen_t len = sizeof(addr);
    int clientfd;
    int msglen;
    char msginfo[16]={0};
    while (1)
    {     
        clientfd = accept(listening_sockfd, &addr, &len);
        if (clientfd <0 )
        {
            usleep(100*1000);
            continue;
        }
        len = recv(clientfd, &msginfo, sizeof(msginfo), 0);
//////////////////
	if (msginfo[0] == 'h') {
		cmd_int = 1;
	}
//////////////////

        if(len > 0)
        {
            //printf("recv msg %s  clientfd %d,max_client %d\n",msginfo,clientfd,max_client);
            close(clientfd);
        }
    }
    close(listening_sockfd);
    return 0;
}

//int main(void)
/*int maina(void)
{
    pthread_t sub_thread[128] = {0};
    int ret;
    int i = 0;
    
    ret = pthread_create(&sub_thread[i],NULL,listenconnect,NULL);
    if(ret!=0)
    {
        printf("pthread_create error!\n");
    }

    while(1)
    {
       // status = check_door_stauts();
       // sleep(1);

        //if(status)
       // {
         //   system("rm /tmp/data/open");
          //  system("rm /tmp/data/close");
       // }
    }

}*/

int set_interface_attribs ()
{
	int serial_fd;
	struct termios tios;

	serial_fd = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY | O_NDELAY);

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
    strModemDevice.append((char*)"ttyUSB1");
    SerialPort* serialPort = new SerialPort(strModemDevice, SerialPort::BR9600);

 //    unsigned char door_open[] = {0x80, 0x01, 0x01, 0x00, 0x80};
 //    unsigned char door_close[] = {0x80, 0x01, 0x01, 0x11, 0x91};
    string door_open = "\x80";
    string door_close = "\x91";
 pthread_t sub_thread[128] = {0};
     int ret;
     int i = 0;
     
  	connectLinuxSocket(SERVER_TEST, "open_success");
     ret = pthread_create(&sub_thread[i],NULL,listenconnect,NULL);
     if(ret!=0)
     {   
         //printf("pthread_create error!\n");
     }
     


	//int count = 0;

	if(serialPort->isOpen()) {
        while (true)
        {
		string strSendOpenDoor = "\x8A\x01\x01\x11\x9B";
		string strSendCheck = "\x80\x01\x01\x33\xB3";
		string strOpenDoorSucces = "\x80\x01\x01\xEE\x6E";

		if (cmd_int == 1) {
			//printf("------>open lock\n");
			cmd_int = 0;
			printf("LOCK_CMD: OPEN_LOCK\n");
                	serialPort->Send(strSendOpenDoor);
                	string strRx_temp = serialPort->Recv();
                	//printf("what %d \n", strRx_temp.length());
IsRecieveDoorOpen = true;//x
                
                	if (strRx_temp == strOpenDoorSucces) {
		    		//printf("lock helper, lock open success\n");
		    		connectLinuxSocket(SERVER_TEST, "open_success");
                	}
                	else {
		    		//printf("lock helper, lock open failed\n");
		    		connectLinuxSocket(SERVER_TEST, "open_fail");
                	}
                	sleep(0.1);
		}
//bool IsRecieveDoorOpen = false;
//int iRecieveDoorOpenDoorCloseCount = 0;
//bool IsDoorOpen = false;
//int iDoorCloseCount = 0;
//int iBadSerialCount = 0;
//string door_open = "\x80";
//string door_close = "\x91";

            	serialPort->Send(strSendCheck);
		printf("CHECKOUT_STATUS\n");
           	int status = -1;
            	string strRx = serialPort->Recv();

            	if (strRx[strRx.length() - 1] == door_open[0]) {
                	status = 1;
                	if (IsRecieveDoorOpen) {
                    		IsDoorOpen = true;
                    		IsRecieveDoorOpen = false;
                	}
            	}
            	else if (strRx[strRx.length() - 1] == door_close[0]) {
                	status = 0;//--
            	}

		if (IsDoorOpen)//F
            	{
                	if (iDoorCloseCount > 3)
                	{
                    		IsDoorOpen = false;
                    		iDoorCloseCount = 0;
                     		//socket message out, door close
		    		connectLinuxSocket(SERVER_TEST, "door_close");
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
            	else//--
            	{
               		if (IsRecieveDoorOpen)//F
                	{
                    		if (iRecieveDoorOpenDoorCloseCount > 60)
                    		{
                        		IsDoorOpen = false;
                        		iDoorCloseCount = 0; 
                        		IsRecieveDoorOpen = false;
                        		iRecieveDoorOpenDoorCloseCount = 0;
                        		//socket message out, door close
		    			connectLinuxSocket(SERVER_TEST, "door_close");
                    		}
                    		iRecieveDoorOpenDoorCloseCount++;
                	}
            	}

            	if (status == -1)
            	{
               		if (iBadSerialCount > 5)
               		{
                    		//socket message out, something wrong with serial
		    		connectLinuxSocket(SERVER_TEST, "serial_error");
                    		iBadSerialCount = 0;
                	}
                	iBadSerialCount++;
            	}
            	else
            	{
               		iBadSerialCount = 0;
            	}

            	sleep(0.1);
            	//count++;
        }
    }
    else
    { 
        cout << "Fail!" << endl; 
    }

    serialPort->Close();
    delete(serialPort);

    return 0;
}
