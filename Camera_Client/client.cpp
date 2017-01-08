#include <stdio.h>
#include <Winsock2.h>
#include <opencv/cv.h>  
#include <opencv/cxcore.h>  
#include <opencv/highgui.h>  

#pragma comment(lib, "ws2_32.lib")  

using namespace cv;

void main()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	const int blocksize=28880;
	//定义包结构  
	struct recvbuf//包格式  
	{  
		char buf[blocksize];//存放数据的变量  
		int flag;//标志          
	}; 
	struct recvbuf data; 


	wVersionRequested = MAKEWORD( 2, 0 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		return;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2||HIBYTE( wsaData.wVersion ) != 0 ) {
		WSACleanup( );
		return;
	}
	SOCKET sockClient=socket(AF_INET,SOCK_DGRAM,0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(6000);

	int nRecvBuf=1024*1024*10;//接收缓存10M  
	setsockopt(sockClient,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
	//声明IplImage指针  
	IplImage* pFrame = NULL,*frame_copy=0;  

	//获取摄像头  
	CvCapture* pCapture = cvCreateCameraCapture(0);  

	//创建窗口  
	//cvNamedWindow("video", 1);  

	//显示视屏  
	/*while(1)  
	{  
	pFrame=cvQueryFrame( pCapture );  
	if(!pFrame)break;  
	cvShowImage("video",pFrame);  
	char c=cvWaitKey(33);  
	if(c==27)break;  
	}  
	*/

	//IplImage *frame=NULL, *frame_copy=0;

	while(1)
	{
		//memset(data.buf, 0, sizeof(char)*blocksize);

		pFrame=cvQueryFrame(pCapture);                            //取帧操作  
		printf("the width is %d;the height is %d;the deeps is %d; the channel is :%d\n",pFrame->width,pFrame->height, pFrame->depth,pFrame->nChannels);
		if(!pFrame)break;  
		//cvShowImage("video",frame);
		Mat L(pFrame);
		// namedWindow("Lena.jpg", CV_WINDOW_AUTOSIZE); 
		// imshow("Lena.jpg", L);
		//waitKey(33);
		if(pFrame)  
		{ 
			uchar* img=L.data;                        //指向该帧的数据矩阵  
			for(int i=0;i<32;i++)                    //640*480*3= 921600 
			{
				for(int k=0;k<blocksize;k++)  
				{ 
					data.buf[k]=img[i*blocksize+k];       
				}  
				if(i==31)                         //标识一帧中最后一个数据包             
				{  
					data.flag=2;   
				}  
				else  
				{  
					data.flag=1;  
				}  
				sendto(sockClient,(char*)(&data),sizeof(data),0,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR)); 
			}      
			cvReleaseImage( &frame_copy );   // 退出之前结束底层api的捕获操作
			Sleep(25);

		}
	}
	cvReleaseCapture(&pCapture);
	closesocket(sockClient);
	WSACleanup();
}
