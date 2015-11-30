// StereoD3D.cpp : Defines the entry point for the application.
//
#include "Global Variables.h"
#include "opencv_inc.h"

#include "common.h"
#include <fstream>
#include <ctime>

#include <io.h>
#include <cstdlib>

#include <cmath>

#include "Preprocess_PMS.h"


//setw용
#include <iomanip>

//-----------------------------------------------------------------------------
//Define
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//프로그램 설정
//
//WEBCAM_MODE		: config 파일을 사용안하는 경우 설정하는 웹캠 모드 설정용
//WEBCAM_NUMBER		: 사용할 웹캠 번호
//
//FRAMESKIP_ENABLE	: 프레임이 target framerate가 안나올 경우, 프레임 스킵을 킬지여부
//
//MORPH_STRUCT_SIZE_X	: Longest Contour에 사용하는 morphology element의 사이즈 결정
//MORPH_STRUCT_SIZE_Y	:
//
//WALK_DIRECTION_THRESHOLD : 걷는 방향을 결정할때, 방향성 유지의 역치값
//CONTOUR_CONDITION_THRESHOLD : 컨투어 조건 내에 컨투어 길이가 유지될 때, 얼마나 오래유지가 되면 유효한 데이터로 처리할 것인가를 결정할 값
//
//MIN_DETECT_CONTOUR : 컨투어 길이의 유효한 범위 (dead zone) 설정
//MAX_DETECT_CONTOUR :
//
//READ_VIDEO_FOLDER : config 파일을 사용 안할때 읽을 파일의 경로 
//READ_VIDEO_NAME :
//-----------------------------------------------------------------------------

#define WEBCAM_MODE false
#define WEBCAM_NUMBER 1

#define FRAMESKIP_ENABLE false


#define MORPH_STRUCT_SIZE_X 3
#define MORPH_STRUCT_SIZE_Y 3

#define TARGET_FPS 30.0

#define WALK_DIRECTION_THRESHOLD 5
#define CONTOUR_CONDITION_THRESHOLD 0

#define MIN_DETECT_CONTOUR 300
#define MAX_DETECT_CONTOUR 2000

#define READ_VIDEO_FOLDER "Input/"
#define READ_VIDEO_NAME "Jgag.avi"


//-----------------------------------------------------------------------------
//컴파일러 설정
//
//OPENCL_SUPPORT: BGSubtraction의 OpenCL 사용여부
//MASK_MODE		: 출력 실루엣을 입력 Mask로 사용하여, 잘라진 입력 이미지를 출력으로 사용
//DEBUG_MODE	: 출력창에 Debug용 파라미터들을 표시
//DEMO_MODE		: 출력 결과를 미리 설정한 값으로 표기(UI Demo용 옵션)
//
//INPUT_IS_SILHOUETTE : 입력 영상이 이미 실루엣인 경우, BGSubtraction을 생략
//
//USE_CONFIG_FILE: 설정을 config.txt에서 읽어들일지 여부를 결정
//
//-----------------------------------------------------------------------------

#define OPENCL_SUPPORT 0
#define MASK_MODE 0
#define DEBUG_MODE 1
#define DEMO_MODE 1
#define USE_GUI_BUTTON 1

#define INPUT_IS_SILHOUETTE 0

#define USE_CONFIG_FILE 1

#define FILESAVE_MODE_EN 0

//-----------------------------------------------------------------------------
// 프로그램 내부 사용 define
//-----------------------------------------------------------------------------
#define END_OK 0
#define END_ERROR 1

#define LEFT_WALK -1
#define SAME_WALK 0
#define RIGHT_WALK 1

#define toggle(a) !a



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------


//OpenCV Global Variables
char* videoFilename = READ_VIDEO_NAME;

cv::VideoCapture capture;
cv::VideoCapture camera;

cv::Mat Current_Frame;
cv::Mat BackgroundMOG;
cv::Mat Silhouette_Final;
cv::Mat ContourImages;

cv::Mat Mask_Output;

//OpenCL 사용시 사용하는 변수
cv::UMat CL_Current_Frame;
cv::UMat CL_Background_Frame;


int Rows, Cols; //프레임 사이즈
int frame_no = 0; //프레임 진행도

int Walk_Direction = 0; //걸음 방향 
int Direction_Tally[2] = { 0 }; //걸음 방향 기록용 표
cv::Point Previous_Point; //추적 박스의 이전 좌표 저장용

//프레임 레이트 계산용 Tick Counter 측정 
LARGE_INTEGER Frequency;
LARGE_INTEGER Start_counter;
LARGE_INTEGER End_counter;

LARGE_INTEGER Lag1, Lag2, Lag3, Lag4, Lag5;

double frame_difference = 0;


//config 파일 설정 변수
string Train_data_path;
string Video_File_Path;
bool Webcam_mode = WEBCAM_MODE;
int Webcam_number = WEBCAM_NUMBER;
bool Loop_Mode = false;
//

//인식결과 flag 변수
bool Recognition_Processing = false;
bool Recognition_Success = false;
int Contour_condition = 0;


//Button Click Functions
bool PlayCommand = true;
bool ResetCommand = false;
bool ExitCommand = false;
bool StartRecognition = false;


//StringStream
string SavePath1, SavePath2, SavePath3, SavePath4;


using namespace std;
using namespace cv;


enum Config_type{
	TRAIN_PATH = 1,
	VIDEO_PATH,
	WEBCAM,
	WEBCAM_NO,
	LOOP_MODE,
}; //config 파일용

string FindConfigurationString(string currentline, int* Type)
{
	
	//변수 설명
	//configuration_data : 파일에서 반환되는 configuration 파일의 설정값을 저장하는 변수
	//i: index 변수, '='의 위치를 찾아 configuration_data에 넣을 string을 짜를 위치를 참조할때 사용
	//data_index : =의 위치를 저장하는 용도
	//type : 어느 설정 값인지 알려주는 용도.

	string configuration_data;
	uint i, data_index;

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

		if (currentline.find("loop_mode=") != string::npos)
			*Type = LOOP_MODE;
	}

	//Type이 안정해지는 경우는 설정이 아니므로 에러반환
	if (*Type == 0)
		return "";


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
		case LOOP_MODE:
			if (data.find("true") != string::npos)
				Loop_Mode = true;
			else
				Loop_Mode = false;
			break;
		default:
			continue;
		}


	}

	return END_OK;

}

void InitLocalVariables()
{

	//인식결과 flag 변수
	 Recognition_Processing = false;
	 Recognition_Success = false;

	frame_no = 0;
	 Walk_Direction = 0;
	 Contour_condition = 0;
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

void InitSaveDirectories()
{
	//----------------------------------------------------
	//디버그용 데이터 저장을 위한 디렉토리를 만들어 놓는 작업
	//data폴더를 생성하고 data 밑에 파일들을 저장한다.
	//
	//----------------------------------------------------
	ostringstream StringBuffer1, StringBuffer2, StringBuffer3, StringBuffer4;

	time_t now = time(0);

	CreateDirectoryA("Silhouette Data", NULL);

	int index = 0;
	for (int i = 0; i < Video_File_Path.size(); i++)
		if (Video_File_Path[i] == '/')
			index = i;


	if (WEBCAM_MODE)
		StringBuffer1 << "Silhouette Data/" << "Webcam" << ctime(&now) << '/';
	else
		StringBuffer1 << "Silhouette Data/" << Video_File_Path.substr(index,Video_File_Path.size()) << '/';

	CreateDirectoryA(StringBuffer1.str().c_str(), NULL);

	string CommonData = StringBuffer1.str();

	StringBuffer2 << CommonData;
	StringBuffer3 << CommonData; //contour
	StringBuffer4 << CommonData; // resampled

	StringBuffer1 << "Original/";
	CreateDirectoryA(StringBuffer1.str().c_str(), NULL);

	StringBuffer2 << "Longest Contour/";
	CreateDirectoryA(StringBuffer2.str().c_str(), NULL);

	StringBuffer3 << "Result/";
	CreateDirectoryA(StringBuffer3.str().c_str(), NULL);


	SavePath1 = StringBuffer1.str();
	SavePath2 = StringBuffer2.str();
	SavePath3 = StringBuffer3.str();
	SavePath4 = StringBuffer4.str();

}

void MouseCallBackFunc(int event, int x, int y, int flags, void* userdata)
{

	if (event == EVENT_LBUTTONDOWN)
	{
		//Play Button
		if (y > Rows - 60 && Rows - 30)
		{
			if (x > 30 && x < 90)
			{
				cout << "Play/Pause" << endl;
				if (PlayCommand)
					PlayCommand = false;
				else
					PlayCommand = true;
			}

			else if (x > 100 && x < 160)
			{
				cout << "Reset" << endl;
				ResetCommand = true;
			}

			else if (x > 170 && x < 230)
			{
				cout << "Exit" << endl;
				ExitCommand = true;
			}
			else if (x > 240 && x < 360)
			{
				cout << "Recognition" << endl;
				StartRecognition = true;
			}
			
		}


	}

	/*
	else if (event == EVENT_RBUTTONDOWN)
	{
		cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MBUTTONDOWN)
	{
		cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

	}

	*/
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
	InitLocalVariables();

	//
	namedWindow("Masked Final", CV_WINDOW_AUTOSIZE);
	setMouseCallback("Masked Final", MouseCallBackFunc, NULL);

	
	//createTrackbar("Target FPS")


#if FILESAVE_MODE_EN
	InitSaveDirectories();
		
#endif

	Mat Morph_Element = getStructuringElement(MORPH_RECT, Size(2 * MORPH_STRUCT_SIZE_X + 1, 2 * MORPH_STRUCT_SIZE_Y + 1), Point(MORPH_STRUCT_SIZE_X, MORPH_STRUCT_SIZE_Y));
	
	vector<vector<Point>> VectorPointer;
	Mat ContourData;

	vector<Data_set> Training_data;
	Training_data = Read_training_data(Train_data_path);


	QueryPerformanceFrequency(&Frequency);

	frame_difference =  (double)Frequency.QuadPart / TARGET_FPS;

	QueryPerformanceCounter(&Start_counter); //초기 타이머

	double frame_rate = 0.0;

	//-------------------------------------------------------------
	// 프레임 처리 루프 시작점
	//--------------------------------------------------------------

	frame_no = 0;

	while (1) //Frame Processing Loop Start
	{
		 //프레임 넘버 기록

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
				if (Loop_Mode) //루프 모드에서는 영상이 끝나면 다시 처음으로 돌아간다.
				{
#if USE_CONFIG_FILE
					capture = VideoCapture(Video_File_Path);
#else
					ostringstream FilePathVideo;
					FilePathVideo << READ_VIDEO_FOLDER << videoFilename;
					capture = VideoCapture(FilePathVideo.str()); //파일에서 읽을 것을 할당해준다.
					
#endif
					InitLocalVariables();
					continue;
				}
				else
				{
					cerr << "End of File" << endl;
					cerr << "Exiting..." << endl;
					exit(EXIT_FAILURE);
				}
			}

		}

		static int Longest_Contour_Length;
		static Mat Longest_Contour;
		
#if INPUT_IS_SILHOUETTE //이미 입력이 실루엣인 경우 BGSubtraction도 생략. Contour Based parameter만 뽑는다
		Mat SplitChannel[3];
		split(Current_Frame, SplitChannel);
		ContourBasedParameterCalc(&SplitChannel[0], &Longest_Contour_Length);
		SplitChannel[0].copyTo(Longest_Contour);
#else
		
#if OPENCL_SUPPORT
		CL_Current_Frame = Current_Frame.getUMat(ACCESS_READ);
		FastBGSubtract();
#else
		FastBGSubtract_NonCL();
#endif
		ContourBasedFilter(&Longest_Contour, &Silhouette_Final, &Longest_Contour_Length);
#endif
		
		//cv::morphologyEx(Longest_Contour, Longest_Contour, MORPH_CLOSE, Morph_Element);
		/*
		cv::morphologyEx(Longest_Contour, Longest_Contour, MORPH_DILATE, Morph_Element);
		*/


		vector<Point> contour_point_array;

		//Contour(&Longest_Contour, &contour_point_array);


		/*
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
		*/

		static string Data_Result = "NULL";
		string Returned_name;

		if (Longest_Contour_Length > MIN_DETECT_CONTOUR && Longest_Contour_Length < MAX_DETECT_CONTOUR && frame_no > 0 && !Recognition_Success && StartRecognition) //실루엣이 존재하고, 컨투어 길이가 일정 이상일때
		{
			if (Contour_condition > CONTOUR_CONDITION_THRESHOLD)
			{
				Returned_name = Train_main(Longest_Contour, &Training_data);

				Recognition_Processing = true;

				if (Returned_name.find("No Data") == string::npos)
				{
					Recognition_Success = true;
					StartRecognition = false;
					Data_Result = Returned_name;
				}
				else
				{
					Data_Result = "Not Recognized";
				}
			}

			if (Contour_condition <= CONTOUR_CONDITION_THRESHOLD)
				Contour_condition++;
		}

		else
		{
			if (Contour_condition >0)
				Contour_condition--;
		}



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
		FPS << "Frame#: " << frame_no << " FPS: " << frame_rate; 
		cv::putText(Mask_Output, FPS.str(), Point(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(255, 255, 255), 4, LINE_AA, false);
		cv::putText(Mask_Output, FPS.str(), Point(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(0, 0, 0), 2, LINE_AA, false);

#if DEBUG_MODE
		ostringstream DebugStream;

		DebugStream << "Longest Contour: " << Longest_Contour_Length << " Period: " <<period;
		cv::putText(Mask_Output, DebugStream.str(), Point(30, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(255, 255, 255), 4, LINE_AA, false);
		cv::putText(Mask_Output, DebugStream.str(), Point(30, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(0, 0, 0), 2, LINE_AA, false);

#endif
		
		//걷는 방향에 빠라서 표기할 String을 입력 후 이미지에 표시
		/*
		if (Walk_Direction == LEFT_WALK)
			cv::putText(Mask_Output, "Left", Point(30, Rows - 30), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 255, 0), 1, LINE_AA, false);
		else if (Walk_Direction == RIGHT_WALK)
			cv::putText(Mask_Output, "Right", Point(30, Rows - 30), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 0, 0), 1, LINE_AA, false);
		else
			cv::putText(Mask_Output, "Stop", Point(30, Rows - 30), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 255), 1, LINE_AA, false);
		*/

		//GUI 만들기



		
		//이미지에 추적되는 사람에 rectangle 표시
		if (Longest_Contour_Length > MIN_DETECT_CONTOUR && Longest_Contour_Length < MAX_DETECT_CONTOUR)
			rectangle(Mask_Output, Previous_Point - maxSize / 2, Previous_Point + maxSize / 2, Scalar(128, 128, 128), 2, 8, 0);


		//팀명을 표기
		cv::putText(Mask_Output, "Team MMI-01", Point(Cols - 150, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(255, 255, 255), 3, LINE_AA, false);
		cv::putText(Mask_Output, "Team MMI-01", Point(Cols - 150, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(64, 64, 64), 1, LINE_AA, false);



		//박스에 용주를 표시(인식 기능은 아직 없고 데모 차원)
		if (Recognition_Processing)
		{
			if (Recognition_Success)
			{
				cv::putText(Mask_Output, "Recognized ", Point(Cols - 120, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(128, 255, 128), 3, LINE_AA, false);
				cv::putText(Mask_Output, "Recognized ", Point(Cols - 120, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(64, 64, 64), 1, LINE_AA, false);
			}
			else
			{
				cv::putText(Mask_Output, "Processing ", Point(Cols - 120, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(128, 255, 128), 3, LINE_AA, false);
				cv::putText(Mask_Output, "Processing ", Point(Cols - 120, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(64, 64, 64), 1, LINE_AA, false);
			}
		}

		else
		{
			cv::putText(Mask_Output, "Searching.. ", Point(Cols - 120, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(128, 128, 255), 3, LINE_AA, false);
			cv::putText(Mask_Output, "Searching.. ", Point(Cols - 120, 60), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(64, 64, 64), 1, LINE_AA, false);
		}


		//인식기 반환 결과를 출력
		cv::putText(Mask_Output, "Recognized as:", Point(Cols - 170, Rows - 60), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 255, 0), 1, LINE_AA, false);
		cv::putText(Mask_Output, Data_Result, Point(Cols - 170, Rows - 30), FONT_HERSHEY_COMPLEX_SMALL, 0.9, Scalar(255, 0, 255), 1, LINE_AA, false);


#if DEBUG_MODE
		cv::imshow("Input", Current_Frame);
		cv::moveWindow("Input", 0 + Cols, 0);

		//실루엣은 중앙에 위치하도록 가변 위치 적용

		cv::imshow("Silhouette", Silhouette_Final);
		cv::moveWindow("Silhouette", 0, 0 + Rows + 30);

		cv::imshow("Filtered", Longest_Contour);
		cv::moveWindow("Filtered", 0 + Cols, 0 + Rows + 30);

#endif
		frame_no++;



#if USE_GUI_BUTTON
		//GUI Make

		//Play Button
		if (PlayCommand)
		{
			rectangle(Mask_Output, Point(30, Rows - 60), Point(90, Rows - 30), Scalar(200, 200, 200), -1, 8, 0);
			rectangle(Mask_Output, Point(30, Rows - 60), Point(90, Rows - 30), Scalar(0, 255, 0), 2, 8, 0);
			putText(Mask_Output, "Play", Point(40, Rows - 40), FONT_HERSHEY_PLAIN, 1.2, Scalar(0, 128, 0), 1, LINE_AA, false);
		}

		else
		{
			rectangle(Mask_Output, Point(30, Rows - 60), Point(90, Rows - 30), Scalar(200, 200, 200), -1, 8, 0);
			rectangle(Mask_Output, Point(30, Rows - 60), Point(90, Rows - 30), Scalar(0, 0, 255), 2, 8, 0);
			putText(Mask_Output, "Pause", Point(32, Rows - 40), FONT_HERSHEY_PLAIN, 1.2, Scalar(0, 0, 128), 1, LINE_AA, false);
		}

		//Reset
		rectangle(Mask_Output, Point(100, Rows - 60), Point(100 + 60, Rows - 30), Scalar(200, 200, 200), -1, 8, 0);
				
		if (ResetCommand)
		{
			rectangle(Mask_Output, Point(100, Rows - 60), Point(100 + 60, Rows - 30), Scalar(120, 120, 120), -1, 8, 0);
			//해당하는 함수 호출;

			ResetCommand = false;
		}

		rectangle(Mask_Output, Point(100, Rows - 60), Point(100 + 60, Rows - 30), Scalar(255, 255, 0), 2, 8, 0);
		putText(Mask_Output, "Reset", Point(100 + 2, Rows - 40), FONT_HERSHEY_PLAIN, 1.2, Scalar(128, 128, 0), 1, LINE_AA, false);

		//Exit Button
		rectangle(Mask_Output, Point(170, Rows - 60), Point(170 + 60, Rows - 30), Scalar(200, 200, 200), -1, 8, 0);
		rectangle(Mask_Output, Point(170, Rows - 60), Point(170 + 60, Rows - 30), Scalar(0, 255, 255), 2, 8, 0);
		putText(Mask_Output, "Exit", Point(170 + 10, Rows - 40), FONT_HERSHEY_PLAIN, 1.2, Scalar(0, 128, 128), 1, LINE_AA, false);

		//Recognition Start Button
		rectangle(Mask_Output, Point(240, Rows - 60), Point(240 + 120, Rows - 30), Scalar(200, 200, 200), -1, 8, 0);
		rectangle(Mask_Output, Point(240, Rows - 60), Point(240 + 120, Rows - 30), Scalar(100, 100, 255), 2, 8, 0);
		putText(Mask_Output, "Recognition", Point(240 + 2, Rows - 40), FONT_HERSHEY_PLAIN, 1.2, Scalar(50, 128, 128), 1, LINE_AA, false);

		if (ExitCommand)
			exit(EXIT_SUCCESS);

#endif



#if FILESAVE_MODE_EN

		ostringstream Save1;
		Save1 << SavePath1 << std::setfill('0') << std::setw(5) << frame_no << ".jpg";
		imwrite(Save1.str(), Silhouette_Final);

		ostringstream Save2;
		Save2 << SavePath2 << std::setfill('0') << std::setw(5) << frame_no << ".jpg";
		imwrite(Save2.str(), Longest_Contour);

		ostringstream Save3;
		Save3 << SavePath3 << std::setfill('0') << std::setw(5) << frame_no << ".jpg";
		imwrite(Save3.str(), Mask_Output);


#endif


		cv::imshow("Masked Final", Mask_Output);

		//창을 알맞는 위치로 이동
		cv::moveWindow("Masked Final", 0, 0);

		//Play 상태가 아니면 HOLD, EXIT 조건들은 모두 실행
		while (!PlayCommand)
			if (waitKey(1) == 27) //ESC키로 종료 
				break;
			else if (ExitCommand)
				exit(EXIT_SUCCESS);


		if (waitKey(1) == 27) //ESC키로 종료 
			break;

		QueryPerformanceCounter(&End_counter);

	}
	



	return 0;
}