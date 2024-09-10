cmake_minimum_required(VERSION 3.0)		#指定cmake版本 
project(AFCFaceDemo_Linux)				#指定项目的名称，一般和项目的文件夹名称对应

set(CMAKE_CXX_STANDARD 11)				#设置c++标准

add_compile_options(
	-w
)

find_package(OpenCV REQUIRED)
find_package(nlohmann_json 3.11.3 REQUIRED) # json c++

#指定头文件目录
include_directories(./
    ${OpenCV_INCLUDE_DIRS}
)	
include_directories(./inc)

#指定静态和动态文件目录
link_directories(./linux_so)

#该指令的主要作用就是将指定的源文件生成链接文件，然后添加到工程中去。
#arcsoft_face生成的库文件名		[STATIC | SHARED | MODULE] 库类型
add_executable(arcsoft_face_engine_test 
        ./inc/amcomdef.h
        ./inc/arcsoft_face_sdk.h
		./inc/asvloffscreen.h
		./inc/merror.h
		./main.cpp
		./src/FaceRecognition.cpp)

		
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/workspace)
#-fPIC	在给定的作用域内设置一个命名的属性
set_property(TARGET arcsoft_face_engine_test PROPERTY POSITION_INDEPENDENT_CODE ON)

#链接库文件
target_link_libraries(arcsoft_face_engine_test	
		arcsoft_face
		arcsoft_face_engine
		${OpenCV_LIBS}
		nlohmann_json::nlohmann_json
        )

 # 在构建后复制库文件到 workspace 目录
add_custom_command(TARGET arcsoft_face_engine_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/linux_so ${PROJECT_SOURCE_DIR}/workspace
    COMMENT "复制库文件到 workspace 目录"
)
