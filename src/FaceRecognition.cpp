#include "FaceRecognition.h"

FaceRecognition::FaceRecognition(const string& appId, const string& sdkKey, const string& activeKey) : appId(appId), sdkKey(sdkKey), activeKey(activeKey), handle(nullptr) {
    
    // getSdkRelatedInfo();

    // 使用 const_cast 来转换 const char* 到 MPChar
    MRESULT res = ASFOnlineActivation(const_cast<char*>(appId.c_str()), const_cast<char*>(sdkKey.c_str()), const_cast<char*>(activeKey.c_str()));
    if (res != MOK && res != MERR_ASF_ALREADY_ACTIVATED) {
        cout << "激活SDK失败，错误代码：" << res << endl;
    } else {
        cout << "SDK激活成功或已经激活。" << endl;
        // SDK版本信息
        const ASF_VERSION version = ASFGetVersion();
        cout << "Version: " <<  version.Version << endl;
        cout << "BuildDate: " <<  version.BuildDate << endl;
        cout << "CopyRight: " <<  version.CopyRight << endl;

        //获取激活文件信息
        ASF_ActiveFileInfo activeFileInfo = { 0 };
        res = ASFGetActiveFileInfo(&activeFileInfo);
        if (res != MOK)
        {
            printf("ASFGetActiveFileInfo fail: %d\n", res);
        }
        else
        {
            //这里仅获取了有效期时间，还需要其他信息直接打印即可
            char startDateTime[32];
            timestampToTime(activeFileInfo.startTime, startDateTime, 32);
            printf("startTime: %s\n", startDateTime);
            char endDateTime[32];
            timestampToTime(activeFileInfo.endTime, endDateTime, 32);
            printf("endTime: %s\n", endDateTime);
        }
    }

    MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER |
	        ASF_LIVENESS | ASF_IR_LIVENESS | ASF_MASKDETECT | ASF_IMAGEQUALITY | ASF_UPDATE_FACEDATA;
    res = ASFInitEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_0_ONLY, ASF_MAX_DETECTFACENUM, mask, &handle);
    if (res != MOK) {
        cout << "初始化引擎失败，错误代码：" << res << endl;
    } else {
        cout << "引擎初始化成功。" << endl;
    }

    // 设置遮挡检测阈值
    ASF_FaceAttributeThreshold faceAttributeThreshold;
    faceAttributeThreshold.eyeOpenThreshold = 0.5f;
    faceAttributeThreshold.mouthCloseThreshold = 0.5f;
    faceAttributeThreshold.wearGlassesThreshold = 0.5f;
    res = ASFSetFaceAttributeParam(handle, &faceAttributeThreshold);
    if (res != MOK){
        printf("ASFSetFaceAttributeParam failed: %d\n", res);
    }
}

FaceRecognition::~FaceRecognition() {
    if (handle) {
        MRESULT res = ASFUninitEngine(handle);
        cout << (res == MOK ? "引擎反初始化成功。" : "引擎反初始化失败。") << endl;
    }
}

//时间戳转换为日期格式
void FaceRecognition::timestampToTime(char* timeStamp, char* dateTime, int dateTimeSize)
{
	time_t tTimeStamp = atoll(timeStamp);
	struct tm* pTm = gmtime(&tTimeStamp);
	strftime(dateTime, dateTimeSize, "%Y-%m-%d %H:%M:%S", pTm);
}

void FaceRecognition::getSdkRelatedInfo()
{
    printf("\n************* ArcFace SDK Info *****************\n");
    MRESULT res = MOK;

    //采集当前设备信息，用于离线激活
    char* deviceInfo = NULL;
    res = ASFGetActiveDeviceInfo(&deviceInfo);
    if (res != MOK) {
        printf("ASFGetActiveDeviceInfo fail: %d\n", res);
    } else {
        printf("ASFGetActiveDeviceInfo sucess: %s\n", deviceInfo);
    }

    //获取激活文件信息
    ASF_ActiveFileInfo activeFileInfo = { 0 };
    res = ASFGetActiveFileInfo(&activeFileInfo);
    if (res != MOK)
    {
        printf("ASFGetActiveFileInfo fail: %d\n", res);
    }
    else
    {
        //这里仅获取了有效期时间，还需要其他信息直接打印即可
        char startDateTime[32];
        timestampToTime(activeFileInfo.startTime, startDateTime, 32);
        printf("startTime: %s\n", startDateTime);
        char endDateTime[32];
        timestampToTime(activeFileInfo.endTime, endDateTime, 32);
        printf("endTime: %s\n", endDateTime);
    }

    //SDK版本信息
    const ASF_VERSION version = ASFGetVersion();
    printf("\nVersion:%s\n", version.Version);
    printf("BuildDate:%s\n", version.BuildDate);
    printf("CopyRight:%s\n", version.CopyRight);
    
}

// 人脸检测
MRESULT FaceRecognition::detectFace(ASVLOFFSCREEN inputImg, ASF_MultiFaceInfo& detectedFaces) {
    return ASFDetectFacesEx(handle, &inputImg, &detectedFaces);
}

// 人脸特征提取
MRESULT FaceRecognition::extractFeatures(ASVLOFFSCREEN inputImg, ASF_SingleFaceInfo& faceInfo, ASF_FaceFeature& feature) {
    cout << "开始人脸特征提取:" << endl;
    return ASFFaceFeatureExtractEx(handle, &inputImg,&faceInfo,ASF_REGISTER, 0, &feature);
}

float FaceRecognition::compareFaces(ASF_FaceFeature& feature1, ASF_FaceFeature& feature2) {
    MFloat confidenceLevel = 0.0; // 初始化置信度
    MRESULT res = ASFFaceFeatureCompare(handle, &feature1, &feature2, &confidenceLevel);
    if (res != MOK) {
        std::cerr << "ASFFaceFeatureCompare failed: " << res << std::endl;
        return 0.0f;                // 如果比对失败，返回0.0或其他表示失败的值
    }       
    std::cout << "ASFFaceFeatureCompare success: " << confidenceLevel << std::endl;
    return confidenceLevel;         // 返回检测到的置信度
}

ASVLOFFSCREEN FaceRecognition::convertMatToASVLOFFSCREEN(const cv::Mat& image) {
    ASVLOFFSCREEN inputImg = {0};
    inputImg.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
    inputImg.i32Width = image.cols;
    inputImg.i32Height = image.rows;
    inputImg.ppu8Plane[0] = image.data;
    inputImg.pi32Pitch[0] = image.step;
    return inputImg;
}

cv::Mat FaceRecognition::prepareImageForDetection(const cv::Mat& image) {
    cv::Mat adjustedImage;
    int width = image.cols, height = image.rows;
    AdjustAndCropImage(image, adjustedImage, 0, 0, width, height);
    return adjustedImage;
}

void FaceRecognition::AdjustAndCropImage(const cv::Mat& src, cv::Mat& dst, int x, int y, int& width, int& height) {
    if (x + width > src.cols) width = src.cols - x;
    if (y + height > src.rows) height = src.rows - y;
    width = (width / 4) * 4; // 保证宽度为4的倍数
    cv::Rect roi(x, y, width, height);
    dst = src(roi).clone(); // 克隆需要的区域
}
