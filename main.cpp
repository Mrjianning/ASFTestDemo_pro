
#include "src/FaceRecognition.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 主函数
int main() {

    // 打开并解析JSON文件
    std::ifstream file("config.json"); // 假设你的 JSON 文件名为 config.json
    if (!file.is_open()) {
        std::cerr << "无法打开JSON文件" << std::endl;
        return -1;
    }
    
    json config;
    file >> config;

    // 从JSON中获取值
    std::string APPID = config["APPID"];
    std::string SDKKEY = config["SDKKEY"];
    std::string ACTIVEKEY = config["ACTIVEKEY"];

    std::string image_path1 = config["image_path1"];
    std::string image_path2 = config["image_path2"];

    // 使用FaceRecognition类
    FaceRecognition faceRec(APPID, SDKKEY,ACTIVEKEY);

    // ===================================================加载图像1=======================
    cv::Mat image = cv::imread(image_path1, cv::IMREAD_COLOR);
    if (image.empty()) {
        cout << "无法加载图像！" << endl;
        return -1;
    }

	// 0、确保图像为4的倍数宽度并转为ArcSoft要求的格式
	cv::Mat adjustedImage = faceRec.prepareImageForDetection(image);
	ASVLOFFSCREEN inputImg = faceRec.convertMatToASVLOFFSCREEN(adjustedImage);

    // 1、进行人脸检测
	ASF_MultiFaceInfo faceInfo = {0};
    MRESULT res=faceRec.detectFace(inputImg,faceInfo);
	if (res != MOK) {
        std::cout << "人脸检测失败, 错误代码: " << res << std::endl;
    }

	// 2、进行人脸特征提取
	std::map<int, ASF_FaceFeature> features;
    for (int i = 0; i < faceInfo.faceNum; ++i) {
		// 将检测到的人脸框绘制到图像上
        MRECT& rect = faceInfo.faceRect[i];
        cv::rectangle(image, cv::Point(rect.left, rect.top), cv::Point(rect.right, rect.bottom), cv::Scalar(0, 255, 0), 2);

		// 特征提取
		ASF_SingleFaceInfo SingleDetectedFaces = { 0 };	 	// 保存单个人脸
		ASF_FaceFeature feature = { 0 };               	// 保存人脸特征
		ASF_FaceFeature copyfeature1 = { 0 }; 

        // 信息准备
		SingleDetectedFaces.faceRect=faceInfo.faceRect[i];
		SingleDetectedFaces.faceOrient = faceInfo.faceOrient[i];
        SingleDetectedFaces.faceDataInfo = faceInfo.faceDataInfoList[0];
			
		MRESULT res=faceRec.extractFeatures(inputImg,SingleDetectedFaces,feature);
		if (res != MOK)
		{
			printf("%s ASFFaceFeatureExtractEx 1 fail: %d\n", res);
		}else
		{
			cout << "特征提取成功:" <<  i << endl;
			//拷贝feature，否则第二次进行特征提取，会覆盖第一次特征提取的数据，导致比对的结果为1
			copyfeature1.featureSize = feature.featureSize;
			copyfeature1.feature = (MByte *)malloc(feature.featureSize);
			memset(copyfeature1.feature, 0, feature.featureSize);
			memcpy(copyfeature1.feature, feature.feature, feature.featureSize);

			std::cout << "Feature Size: " << feature.featureSize << "\n";
			std::cout << "Feature Data (Hex): ";
			for (int i = 0; i < feature.featureSize; ++i) {
				std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(feature.feature[i]) << " ";
				if ((i + 1) % 16 == 0) std::cout << "\n"; // 每16字节换行，方便查看
			}
			std::cout << std::dec << "\n"; // 恢复到十进制输出
			
			features[i]=copyfeature1;
		}
    }

	// ===================================================加载图像2=======================
    cv::Mat image2 = cv::imread(image_path2, cv::IMREAD_COLOR);
    if (image2.empty()) {
        cout << "无法加载图像！" << endl;
        return -1;
    }

	// 0、确保图像为4的倍数宽度并转为ArcSoft要求的格式
	cv::Mat adjustedImage2 = faceRec.prepareImageForDetection(image2);
	ASVLOFFSCREEN inputImg2 = faceRec.convertMatToASVLOFFSCREEN(adjustedImage2);

    // 1、进行人脸检测
	ASF_MultiFaceInfo faceInfo2 = {0};
    MRESULT res2=faceRec.detectFace(inputImg2,faceInfo2);
	if (res2 != MOK) {
        std::cout << "人脸检测失败, 错误代码: " << res << std::endl;
    }

	// 2、进行人脸特征提取
	std::map<int, ASF_FaceFeature> features2;
    for (int i = 0; i < faceInfo2.faceNum; ++i) {
		// 将检测到的人脸框绘制到图像上
        MRECT& rect = faceInfo2.faceRect[i];
        cv::rectangle(image2, cv::Point(rect.left, rect.top), cv::Point(rect.right, rect.bottom), cv::Scalar(0, 255, 0), 2);

		// 特征提取
		ASF_SingleFaceInfo SingleDetectedFaces = { 0 };	 	// 保存单个人脸
		ASF_FaceFeature feature1 = { 0 };               	// 保存人脸特征

		SingleDetectedFaces.faceRect=faceInfo.faceRect[i];
		SingleDetectedFaces.faceOrient = faceInfo.faceOrient[i];
        SingleDetectedFaces.faceDataInfo = faceInfo.faceDataInfoList[0];

		MRESULT res2=faceRec.extractFeatures(inputImg2,SingleDetectedFaces,feature1);
		if (res2 != MOK)
		{
			printf("%s ASFFaceFeatureExtractEx 1 fail: %d\n", res2);
		}else
		{
			cout << "特征2提取成功:" <<  i << endl;
			features2[i]=feature1;
		}
    }

	// 3、人脸对比
	float condif=faceRec.compareFaces(features[0],features2[0]);
	cout << "相似度：" <<  condif << endl;

	// 转换相似度分数为字符串
    std::string similarityText = "Similarity: " + std::to_string(condif) + "%";
	// 设置文本显示的位置和样式
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.7;
    int thickness = 2;
    cv::Point textOrg(10, 30);  // 设置文本在图片上的起始位置

    // 将文本绘制到第一张图片上
    cv::putText(image, similarityText, textOrg, fontFace, fontScale, cv::Scalar(0, 255, 0), thickness);


    // 显示结果
     // 创建第一个窗口并显示第一张图片
    cv::namedWindow("Face Detection 1", cv::WINDOW_AUTOSIZE);
    cv::imshow("Face Detection 1", image);

    // // 创建第二个窗口并显示第二张图片
    cv::namedWindow("Face Detection 2", cv::WINDOW_AUTOSIZE);
    cv::imshow("Face Detection 2", image2);
    cv::waitKey(0);

    return 0;
}
