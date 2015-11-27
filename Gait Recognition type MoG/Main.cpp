// StereoD3D.cpp : Defines the entry point for the application.
//


#include "common.h"
#include <fstream>
#include <ctime>
#include <opencv2\core\ocl.hpp>

#include <io.h>
#include <cstdlib>

#include <cmath>


//-----------------------------------------------------------------------------
//Define
//-----------------------------------------------------------------------------

#define WEBCAM_MODE false
#define WEBCAM_NUMBER 1

#define FRAMESKIP_ENABLE false

#define FILESAVE_MODE_EN false

#define MORPH_STRUCT_SIZE_X 1
#define MORPH_STRUCT_SIZE_Y 2

#define READ_VIDEO_FOLDER "Input/"
#define READ_VIDEO_NAME "Jgag.avi"

#define TARGET_FPS 50.0

#define LEFT_WALK -1
#define SAME_WALK 0
#define RIGHT_WALK 1

#define WALK_DIRECTION_THRESHOLD 5

#define MAX_BOX_X 40
#define MAX_BOX_Y 80

#define toggle(a) !a

//컴파일러 설정
#define OPENCL_SUPPORT 1
#define MASK_MODE 0
#define WINDOWS_MODE 1
#define DEBUG_MODE 0
#define DEMO_MODE 1

#define USE_CONFIG_FILE 1

//에러코드
#define END_OK 0
#define END_ERROR 1


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------


//OpenCV Global Variables
char* videoFilename = READ_VIDEO_NAME;
char* SaveFilename = "Result.avi";
cv::VideoCapture capture;
cv::VideoCapture camera;

cv::Mat Current_Frame;
cv::Mat BackgroundMOG;
cv::Mat Silhouette_Final;
cv::Mat ContourImages;

cv::Mat Mask_Output;



cv::UMat CL_Current_Frame;
cv::UMat CL_Background_Frame;

int Rows, Cols;
int frame_no = 0;

int Walk_Direction = 0;
cv::Point Previous_Point;

LARGE_INTEGER Frequency;
LARGE_INTEGER Start_counter;
LARGE_INTEGER End_counter;

LARGE_INTEGER Lag1, Lag2, Lag3, Lag4, Lag5;

double frame_difference = 0;

int Direction_Tally[2] = { 0 };

//설정 변수
string Train_data_path;
string Video_File_Path;
bool Webcam_mode = WEBCAM_MODE;
int Webcam_number = WEBCAM_NUMBER;

//

using namespace std;
using namespace cv;


enum Config_type{
	TRAIN_PATH = 1,
	VIDEO_PATH,
	WEBCAM,
	WEBCAM_NO,
}; //config 파일용

string FindConfigurationString(string currentline, int* Type)
{
	
	//변수 설명
	//configuration_data : 파일에서 반환되는 configuration 파일의 설정값을 저장하는 변수
	//i: index 변수, '='의 위치를 찾아 configuration_data에 넣을 string을 짜를 위치를 참조할때 사용
	//data_index : =의 위치를 저장하는 용도
	//type : 어느 설정 값인지 알려주는 용도.

	string configuration_data;
	int i, data_index;

	data_index = 0;
	*Type = 0;

	//어느 구문인지를 판별
	{
		if (currentline.find("training_data_path=") != string::npos)
			*Type = TRAIN_PATH;

		if (currentline.find("input_video_path=") != string::npos)
			*Type = VIDEO_PATH;

		if (currentline.find("webcam_mode=") != string::npos)
			*Type = WEBCAM;

		if (currentline.find("webcam_number=") != string::npos)
			*Type = WEBCAM_NO;
	}

	//Type이 안정해지는 경우는 설정이 아니므로 에러반환
	if (*Type == 0)
		return " ";


	for (i = 0; i < currentline.size(); i++)
		if (currentline.c_str()[i] == '=')
		{
			data_index = i;
			break;
		}
	
	//data 부분만을 반환
	configuration_data = currentline.substr(data_index + 1, currentline.size() - 1);

	return configuration_data;
}

int ConfigurationFileRead() //옵션 파일을 읽어, 해당하는 변수를 모두 설정하는 함수. 실행시에 호출
{
	ifstream Input_Stream("Config.txt");

	if (!Input_Stream.is_open())
	{ 
		cout << "Config.txt not found" << endl;
		return END_ERROR;
	}

	string current_line;

	string data;
	int Type;

	while (!Input_Stream.eof()) //파일 끝까지 읽는다.
	{
		getline(Input_Stream, current_line); //줄을 읽는다.

		data = FindConfigurationString(current_line, &Type);

		switch (Type) 
		{
		case 0:
			continue; //에러이므로
		case TRAIN_PATH:
			Train_data_path = data;
			break;
		case VIDEO_PATH:
			Video_File_Path = data;
			break;
		case WEBCAM:
			if (data.find("true") != string::npos)
				Webcam_mode = true;
			else
				Webcam_mode = false;
			break;
		case WEBCAM_NO:
			Webcam_number = stoi(data.c_str());
			break;
		default:
			continue;
		}


	}

	return END_OK;

}


void InitOpenCVModules() //OPENCV 데이터들의 초기화
{
	/*----------------------*/
	//
	//
	//
	//
	/*-----------------------*/
	

	if (Webcam_mode)
	{
		camera.open(Webcam_number);

		if (!camera.isOpened())  //소스 영상 에러체크
		{
			//error in opening the video input
			cerr << "Unable to open Camera Stream" << endl;
			exit(EXIT_FAILURE);
		}


		camera >> Current_Frame; //카메라 소스에서 Current Frame으로 데이터를 넣어준다.
	}

	else
	{
#if USE_CONFIG_FILE
		capture = VideoCapture(Video_File_Path);
#else
		ostringstream FilePathVideo;
		FilePathVideo << READ_VIDEO_FOLDER << videoFilename;
		capture = VideoCapture(FilePathVideo.str()); //파일에서 읽을 것을 할당해준다.
#endif

		if (!capture.isOpened())  //소스 영상 에러체크
		{
			//error in opening the video input
			cerr << "Unable to open video file: " << videoFilename << endl;
			exit(EXIT_FAILURE);
		}

		if (!capture.read(Current_Frame))
		{
			cerr << "Unable to read next frame." << endl;
			cerr << "Exiting..." << endl;
			exit(EXIT_FAILURE);
		}
	}

	imwrite("Reference.bmp", Current_Frame);


		//최초의 한번만 확인을 거친다
		Rows = Current_Frame.rows;
		Cols = Current_Frame.cols;
		Silhouette_Final = Mat::zeros(Rows, Cols, CV_8UC1);

		Mask_Output = Mat::zeros(Rows, Cols, CV_8UC3);

		ContourImages = Mat::zeros(Rows, 2*Cols, CV_8UC3);

}


void InitWindows()
{
	InitContourWindow();
}

int main(int argc, char *argv[])
{
	

	//--------------------------------------
	//초기화
	//--------------------------------------

	//Config File read
	ConfigurationFileRead(); //최우선적으로 처리


	//OPENCV initialize
	InitOpenCVModules();


	//Variable Initialize
	Mat Morph_Element = getStructuringElement(MORPH_RECT, Size(2 * MORPH_STRUCT_SIZE_X + 1, 2 * MORPH_STRUCT_SIZE_Y + 1), Point(MORPH_STRUCT_SIZE_X, MORPH_STRUCT_SIZE_Y));
	
	vector<vector<Point>> VectorPointer;
	Mat ContourData;


	QueryPerformanceFrequency(&Frequency);

	frame_difference =  (double)Frequency.QuadPart / TARGET_FPS;

	QueryPerformanceCounter(&Start_counter); //초기 타이머

	double frame_rate = 0.0;

	//-------------------------------------------------------------
	// 프레임 처리 루프 시작점
	//--------------------------------------------------------------

	while (1) //Frame Processing Loop Start
	{
		frame_no++; //프레임 넘버 기록

		QueryPerformanceCounter(&End_counter);

		long long Counter_diff = (End_counter.QuadPart - Start_counter.QuadPart);

		//Framerate 안정화 Start
		if ( Counter_diff < frame_difference) //Framerate 고정기
		{
			continue;
		}

		else
		{
			frame_rate = (double)Frequency.QuadPart / (double)(Counter_diff);

			if (frame_rate < TARGET_FPS && FRAMESKIP_ENABLE)
			{
				capture.read(Current_Frame);
			}

			QueryPerformanceCounter(&Start_counter);
		}
		//Framerate 안정화 End



		if (Webcam_mode) //웹캠 모드인지 아닌지를 판별용 조건문
		{
			camera >> Current_Frame;
		}

		else
		{
			if (!capture.read(Current_Frame))
			{
				cerr << "Unable to read next frame." << endl;
				cerr << "Exiting..." << endl;
				exit(EXIT_FAILURE);
			}

		}

		
#if OPENCL_SUPPORT
		CL_Current_Frame = Current_Frame.getUMat(ACCESS_READ);
		FastBGSubtract();
#else
		FastBGSubtract_NonCL();
#endif
	
			

		static Mat Longest_Contour;

		ContourBasedFilter(&Longest_Contour, &Silhouette_Final);
		cv::morphologyEx(Longest_Contour, Longest_Contour, MORPH_DILATE, Morph_Element);
		


		//---------------------------------------------------------------//
		//     Preproccesing START (Feature Extraction)                  //
		//---------------------------------------------------------------//
		//variable 선언
		Mat Contour_out_image_array;
		Mat Cutting_image_array;
		Mat Resampled_image_array;

		int i;
		int height = 0;   int width = 0;    int period = 0;
		double Bounding_box_ratio;

		vector<Point> contour_point_array;
		vector<Point> refer_point;
		vector<vector<Point> > Segment;

		if (!CheckEmpty(&Longest_Contour))
		{
		
			
			//Silhouette screen using contour length
			
			
			
			//Frame Process step
			Cutting_image_array = Cutting_silhouette_area(&Longest_Contour, &height, &width);
			Bounding_box_ratio = (double)height / (double)width;

			// Contour extraction
			Contour_out_image_array = Contour(&Cutting_image_array, &contour_point_array);

			// Resampling
			refer_point = Find_refer_point(contour_point_array);
			Segment = Resampling(&contour_point_array, &refer_point);
			Resampled_image_array = Draw_Resampling(Segment, Contour_out_image_array);

#if DEBUG_MODE

			resize(Contour_out_image_array, Contour_out_image_array, Size(2 * width, 2 * height), 0, 0, CV_INTER_NN);
			resize(Resampled_image_array, Resampled_image_array, Size(2 * width, 2 * height), 0, 0, CV_INTER_NN);

			//imshow("Cutting_image", Cutting_image_array);
			cv::imshow("Contour_image", Contour_out_image_array);
			cv::imshow("Resampling_image", Resampled_image_array);
#endif
		}

		//---------------------------------------------------------------//
		//     Preproccesing END (Feature Extraction)                    //
		//---------------------------------------------------------------//

		if (Direction_Tally[0] > WALK_DIRECTION_THRESHOLD)
			Walk_Direction = LEFT_WALK;
		
		if (Direction_Tally[1] > WALK_DIRECTION_THRESHOLD)
			Walk_Direction = RIGHT_WALK;
		
		//출력 UI는 컬러
		//cvtColor(Silhouette_Final, UI_Output, CV_GRAY2BGR);

#if MASK_MODE
		ImageMask(&Mask_Output, &Current_Frame, &Longest_Contour);
#else	
		Current_Frame.copyTo(Mask_Output);
#endif

		ostringstream FPS;

		//프레임 레이트 표기
		FPS << "FPS: " << frame_rate;
		cv::putText(Mask_Output, FPS.str(), Point(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(255, 255, 255), 3, LINE_AA, false);
		cv::putText(Mask_Output, FPS.str(), Point(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(40, 40, 40), 2, LINE_AA, false);



		//이미지에 추적되는 사람에 rectangle 표시
		rectangle(Mask_Output, Previous_Point - maxSize / 2, Previous_Point + maxSize / 2, Scalar(128, 128, 128), 2, 8, 0);

		//걷는 방향에 빠라서 표기할 String을 입력 후 이미지에 표시
		if (Walk_Direction == LEFT_WALK)
			cv::putText(Mask_Output, "Left", Point(30, Rows - 30), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 255, 0), 1, LINE_AA, false);
		else if (Walk_Direction == RIGHT_WALK)
			cv::putText(Mask_Output, "Right", Point(30, Rows - 30), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 0, 0), 1, LINE_AA, false);
		else
			cv::putText(Mask_Output, "Stop", Point(30, Rows - 30), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 255), 1, LINE_AA, false);

		



		//팀명을 표기
		cv::putText(Mask_Output, "Team MMI-01", Point(Cols - 150, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(255, 255, 255), 3, LINE_AA, false);
		cv::putText(Mask_Output, "Team MMI-01", Point(Cols - 150, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(64, 64, 64), 1, LINE_AA, false);



		//박스에 용주를 표시(인식 기능은 아직 없고 데모 차원)
#if DEMO_MODE
		cv::putText(Mask_Output, "Recognized as:", Point(Cols - 170, Rows - 60), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 0), 1, LINE_AA, false);

		if ((maxP.y - minP.y) > 125)
		{
			cv::putText(Mask_Output, "Jae Sung Lee", Point(Cols - 170, Rows - 30), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 0, 255), 1, LINE_AA, false);
			cv::putText(Mask_Output, "Recognized ", Point(Cols - 120, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(128, 255, 128), 3, LINE_AA, false);
			cv::putText(Mask_Output, "Recognized ", Point(Cols - 120, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(64, 64, 64), 1, LINE_AA, false);
		}

		else
		{
			cv::putText(Mask_Output, "Searching.. ", Point(Cols - 120, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(128, 128, 255), 3, LINE_AA, false);
			cv::putText(Mask_Output, "Searching.. ", Point(Cols - 120, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(64, 64, 64), 1, LINE_AA, false);
		}
#endif
		//Demo End



		//설명 표기
		cv::putText(Mask_Output, "Press ESC to quit", Point(Cols / 2 - 80, Rows - 30), FONT_HERSHEY_PLAIN, 1.2, Scalar(255, 255, 255, 0.5), 1, LINE_AA, false);
		


		cv::imshow("Masked Final", Mask_Output);
				
		//창을 알맞는 위치로 이동
		cv::moveWindow("Masked Final", 0, 0);

#if DEBUG_MODE
		cv::imshow("Input", Current_Frame);
		cv::moveWindow("Input", 0 + Cols, 0);

		//실루엣은 중앙에 위치하도록 가변 위치 적용
		cv::moveWindow("Contour_image", 0 + (Cols - width)/2, 0 + Rows + 30);
		cv::moveWindow("Resampling_image", 0 + (3*Cols - width) / 2, 0 + Rows + 30);

#endif

		if (waitKey(1) == 27) //ESC키로 종료 
			break;

		QueryPerformanceCounter(&End_counter);

	}
	



	return 0;
}