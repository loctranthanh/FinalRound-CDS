#include "lane_detection.h"

/////// My function
Mat filterLane(const Mat &imgLane, bool &pop, Point &point, int check, bool &preState)
{
    pop = false;
    //point.x = 0;
    //point.y = 0;
	if (check==-1){
		point.x = 0;
		point.y = imgLane.rows/2;
	} else {
		point.x = imgLane.cols;
		point.y = imgLane.rows/2;	
	}
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(imgLane, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));
    if (contours.size() == 0)
    {
        Mat none = Mat::zeros(imgLane.size(), CV_8UC1);
        return none;
    }
    Mat result = Mat::zeros(imgLane.size(), CV_8UC1);
    int sumX = 0;
    int sumY = 0;
    int maxArea = 0;
    int maxIndex = 0;
    for (int i = 0; i < (int)contours.size(); ++i)
    {
        int s = contourArea(contours[i]);
        if (s > maxArea)
        {
            maxArea = s;
            maxIndex = i;
        }
    }
    if (contourArea(contours[maxIndex]) < 100)
    {
        Mat none = Mat::zeros(imgLane.size(), CV_8UC1);
        return none;
    }
    int xMin = 0, yMin = 1000, xMax = 0, yMax = -1;
    /*for (int i = 0; i < contours[maxIndex].size(); i++)
    {
        // if (contours[maxIndex][i].y > yMax)
        // {
        //     yMax = contours[maxIndex][i].y;
        //     xMax = contours[maxIndex][i].x;
        // }
        // if (contours[maxIndex][i].y < yMin)
        // {
        //     yMin = contours[maxIndex][i].y;
        //     xMin = contours[maxIndex][i].x;
        // }
        sumX += contours[maxIndex][i].x;
        sumY += contours[maxIndex][i].y;
    }
	*/
    //if((xMax>=xMin) && (check==1)){
    drawContours(result, contours, maxIndex, Scalar(255), CV_FILLED);
    //point.x = sumX / contours[maxIndex].size();
    //point.y = sumY / contours[maxIndex].size();
	if(check==-1){
		point.x = 0;
		for (int i = 0; i < contours[maxIndex].size(); i++)
    		{
			if(contours[maxIndex][i].y - imgLane.rows/2>-20 && contours[maxIndex][i].y - imgLane.rows/2<20){
				if (point.x < contours[maxIndex][i].x)
					point.x = contours[maxIndex][i].x;
			}
        	//sumX += contours[maxIndex][i].x;
        	//sumY += contours[maxIndex][i].y;
    		}	
	} else {
		point.x = imgLane.cols;
		for (int i = 0; i < contours[maxIndex].size(); i++)
    		{
			if(contours[maxIndex][i].y - imgLane.rows/2>-20 && contours[maxIndex][i].y - imgLane.rows/2<20){
				if (point.x > contours[maxIndex][i].x)
					point.x = contours[maxIndex][i].x;
			}
        	//sumX += contours[maxIndex][i].x;
        	//sumY += contours[maxIndex][i].y;
    		}	
	}
	point.y = imgLane.rows/2;

    if ((point.x > imgLane.cols / 2) && (check == -1) && (preState == false))
    {
        //cout << "sum X: " << sumX << endl;
        Mat none = Mat::zeros(imgLane.size(), CV_8UC1);
        return none;
    }
    if ((point.x < imgLane.cols / 2) && (check == 1) && (preState == false))
    {
        Mat none = Mat::zeros(imgLane.size(), CV_8UC1);
        return none;
    }

    pop = true;
    return result;
    // } else
    // if((xMax<=xMin) && (check==-1)){
    //     drawContours(result, contours, maxIndex, Scalar(255), CV_FILLED);
    //     point.x = sumX / contours[maxIndex].size();
    //     point.y = sumY / contours[maxIndex].size();
    //     pop = true;
    //     return result;
    // }
    // return result;
}


void LaneProcessing(cv::Mat& colorImg,cv::Mat& binImg,int& xTam,int& yTam,bool& preLeft,bool& preRight,bool& oneLine,int preX,int preY) {
		Rect crop1(0, 0, binImg.cols / 2 - OFFSET_DIVIDE, binImg.rows);
                Mat Left = binImg(crop1);
                Rect crop2(binImg.cols / 2 + OFFSET_DIVIDE, 0, binImg.cols / 2 - OFFSET_DIVIDE, binImg.rows);
                Mat Right = binImg(crop2);
		/*                
		if (is_show_cam)
                {
                    //imshow("LEFT",Left);
                    //imshow("RIGHT",Right);
                }
		*/
                Mat dstLeft = keepLanes(Left, false);
                Mat dstRight = keepLanes(Right, false);
                //Mat dst = keepLanes(binImg, false);
                //imshow("dst", dst);
                bool isLeft = false;
                bool isRight = false;
                Point pointLeft(0, 0);
                Left = filterLane(dstLeft, isLeft, pointLeft, -1, preLeft);
                pointLeft.y += 3 * binImg.rows / 4;
                Point pointRight(0, 0);
                Right = filterLane(dstRight, isRight, pointRight, 1, preRight);
                pointRight.x += (binImg.cols / 2 + OFFSET_DIVIDE);
                pointRight.y += 3 * binImg.rows / 4;
                //circle(binImg, pointRight, 2, Scalar(255, 0, 255), 3);
                //circle(binImg, pointLeft, 2, Scalar(255, 0, 255), 3);
                preLeft = isLeft;
                preRight = isRight;
                imshow("LEFT", Left);
                imshow("RIGHT", Right);
                

                
                cout << "Left: " << isLeft << " Right: " << isRight << endl;
                


                if (isLeft && isRight)
                {
                    oneLine = false;
                    xTam = (pointLeft.x + pointRight.x) / 2;
                    yTam = (pointLeft.y + pointRight.y) / 2;
                  /*  
			if (road_width_set)
                    {
                        road_width = (road_width + pointRight.x - pointLeft.x) / 2;
                    }
                    else
                    {
                        road_width = pointRight.x - pointLeft.x;
                        road_width_set = true;
                    }
		*/
                }
                else if (isLeft)
                {
                    putText(colorImg, "one left", Point(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(255, 0, 0), 1, CV_AA);
                    oneLine = true;
                    xTam = (binImg.cols + pointLeft.x) / 2; // binImg.cols / 2 + 150;
                    yTam = pointLeft.y;
                    //std::cout << "angdiff: " << angDiff << std::endl;
                    // theta = (0.00);
                    //api_set_STEERING_control(pca9685, theta);
                }
                else if (isRight)
                {
                    putText(colorImg, "one right", Point(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(255, 0, 0), 1, CV_AA);
                    oneLine = true;
                    xTam = pointRight.x / 2; // binImg.cols / 2 - 150;
                    yTam = pointRight.y;
                }
                else
                {
                    //oneLine = false;
                    xTam = preX;
                    yTam = preY;
                }
		circle(binImg, Point(xTam, yTam), 2, Scalar(255, 255, 0), 3);
                circle(colorImg, Point(xTam, yTam), 2, Scalar(255, 255, 0), 3);
		        circle(colorImg, Point(pointLeft.x, pointLeft.y), 2, Scalar(255, 255, 0), 3);
	        	circle(colorImg, Point(pointRight.x, pointRight.y), 2, Scalar(255, 255, 0), 3);
		        imshow("color", colorImg);
}










