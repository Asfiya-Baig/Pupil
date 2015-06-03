// This code uses OpenCV for detecting pupils.


#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <fstream>
#include <highgui.h>
using namespace std;
using namespace cv;

int main()
{
char key;			//For exiting from while loop
double thresh ;			//Threshold value
double len;			//arclength of contour
int i=0,j=0;
int x1, x2;			//		
int histSize = 256;		//no of bins in the histogram
float range[] = { 0, 255 } ;	//range of intensity values
const float* histRange = { range };
int hist_w = 512; int hist_h = 400;	//width and height of window to display the histogram
int bin_w = cvRound( (double) hist_w/histSize );	//bin_w=2
RotatedRect temp;
Mat image_RGB, image, output,output_c,hist, normalized;
VideoCapture capture("vid_3.avi");	//Opens the camera of the device connected
capture>>image_RGB;			//Extract a frame and store in image matrix. 
while(1)
{
	capture>>image_RGB;		//Extract a frame and store in image matrix. 
	cvtColor(image_RGB, image, CV_BGR2GRAY);	//Converts the 3 channel image to single channel gray scale image	
	normalize(image, normalized, 0, 255, NORM_MINMAX);	//Increases contrast in the image by a process called linear stretching
    	//ROI by creating masks for Ellipses AND Selecting only the ellipse as part of image. Background subtraction?? 
	Mat mask = Mat::zeros(image.size(),CV_8UC1);	//Makes a black mask
	ellipse(mask, Point( 0, 278 ), Size( 179.0, 125.0 ), 0, 0, 360, Scalar( 255, 255, 255), -1, 8 );//mask_LEFT- ellipse on the left
	ellipse(mask, Point( 639,300), Size( 164.0, 125.0 ), 0, 0, 360, Scalar( 255, 255, 255), -1, 8 );//mask_RIGHT- ellipse on the right
	Mat res= Mat::zeros(image.size(),CV_8UC1);   
	bitwise_and(normalized,mask,res);  /*Black with image gives black. White with image gives image. S only elliptical regions of the 								image are visible.*/
	GaussianBlur(res, res,Size(7,7),0);	//To blur out noise caused die to eye brows and eye lashes

	//for( i =0; i<=640; i++)
	//{
	//for(j=0;j<480;j++)
	//	{
	//		if(res.at<uchar>(j,i)==0 )
	//		res.at<uchar>(j,i)=255;			
	//	}
	//}

	calcHist(&res, 1, 0, Mat(), hist,1, &histSize, &histRange,true, false  );
	Mat histImage( hist_h, hist_w, CV_8UC1, Scalar( 0,0,0) );
	normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1 );
	for(  i = 1; i < histSize; i++ )
	{
    		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
                Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
                Scalar( 255, 0, 0), 2, 8, 0  );
	}

	

	//Calculating threshold value from the histogram
	for( i=0;i<=hist_w;i++)				//hist_w=512
	{
		if(histImage.at<uchar>(390,i)==255)	
		{
		
		if(i<256)
		x1=i;
		else 
		x2=i;
		}
	
	}
	//cout<<x1<<endl<<x2<<endl;
	thresh=(x1+x2)/5;			 
	threshold(res,output, thresh, 255, THRESH_BINARY_INV); //Using an opencv function to threshold the image
	//morphologyEx(output,output_c, MORPH_CLOSE, 3); 	// for closing the contours 
	
	Mat drawing=image_RGB.clone();
	std::vector<std::vector<cv::Point> > contours;	//Defining Contour variable as a vector
	findContours(output.clone(), contours, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);//Finding contours in the thresholded image;thr.clone() creates a copy of the thresholded image. Done as source image is modified.
	

	for( i=0; i<contours.size(); i++)
	{
		len = arcLength(contours[i], true); 	
		if(  len>100 &&len<400)
		{
		 temp = fitEllipse(Mat(contours[i]));
		//drawContours( drawing, contours, i, (0,0,255) , 2 );
		ellipse(drawing, temp, Scalar(0,255,0), 2, 8);
		}
	}
	
	
	//namedWindow("normalized", CV_WINDOW_NORMAL);
	//imshow("normalized" , normalized);
	namedWindow("calcHist Demo", CV_WINDOW_NORMAL);
	imshow("calcHist Demo", histImage );
    	namedWindow("BLURRED",WINDOW_NORMAL);		
        imshow("BLURRED",res); 
       	namedWindow("canny",WINDOW_NORMAL);		
  	imshow("canny",output);  		
	namedWindow("contour",WINDOW_NORMAL);		
	imshow("contour", drawing);  		


	key = waitKey(100); 	//Capture Keyboard stroke
    	if (char(key) == 27)
	{
	        break; 		//If you hit ESC key loop will break and code will terminate

	}
}
return 0;
}

