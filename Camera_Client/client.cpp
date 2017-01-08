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
	//������ṹ  
	struct recvbuf//����ʽ  
	{  
		char buf[blocksize];//������ݵı���  
		int flag;//��־          
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

	int nRecvBuf=1024*1024*10;//���ջ���10M  
	setsockopt(sockClient,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
	//����IplImageָ��  
	IplImage* pFrame = NULL,*frame_copy=0;  

	//��ȡ����ͷ  
	CvCapture* pCapture = cvCreateCameraCapture(0);  

	//��������  
	//cvNamedWindow("video", 1);  

	//��ʾ����  
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

		pFrame=cvQueryFrame(pCapture);                            //ȡ֡����  
		printf("the width is %d;the height is %d;the deeps is %d; the channel is :%d\n",pFrame->width,pFrame->height, pFrame->depth,pFrame->nChannels);
		if(!pFrame)break;  
		//cvShowImage("video",frame);
		Mat L(pFrame);
		// namedWindow("Lena.jpg", CV_WINDOW_AUTOSIZE); 
		// imshow("Lena.jpg", L);
		//waitKey(33);
		if(pFrame)  
		{ 
			uchar* img=L.data;                        //ָ���֡�����ݾ���  
			for(int i=0;i<32;i++)                    //640*480*3= 921600 
			{
				for(int k=0;k<blocksize;k++)  
				{ 
					data.buf[k]=img[i*blocksize+k];       
				}  
				if(i==31)                         //��ʶһ֡�����һ�����ݰ�             
				{  
					data.flag=2;   
				}  
				else  
				{  
					data.flag=1;  
				}  
				sendto(sockClient,(char*)(&data),sizeof(data),0,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR)); 
			}      
			cvReleaseImage( &frame_copy );   // �˳�֮ǰ�����ײ�api�Ĳ������
			Sleep(25);

		}
	}
	cvReleaseCapture(&pCapture);
	closesocket(sockClient);
	WSACleanup();
}
