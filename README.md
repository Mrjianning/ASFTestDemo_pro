
---

# ASFTestDemo_pro

## 项目简介

ASFTestDemo_pro 是一个基于 OpenCV 的人脸识别项目。本项目包含了一些必要的库和源代码，演示了如何使用 ArcSoft 提供的 增值版 SDK 进行人脸检测与识别。该项目还使用 `nlohmann/json` 库来解析 JSON 配置文件。

## 项目结构

```
ASFTestDemo_pro
│
├── build/                 # 构建输出目录
├── images/                # 存放测试图片和视频的目录
├── inc/                   # 头文件目录
│   ├── amcomdef.h
│   ├── arcsoft_face_sdk.h
│   ├── asvloffscreen.h
│   └── merror.h
├── linux_so/              # 存放 Linux 下的共享库文件
├── src/                   # 源代码目录
│   ├── FaceRecognition.cpp
│   ├── FaceRecognition.h
    ├── main.h
├── workspace/             # 编译后的可执行文件输出目录
└── CMakeLists.txt         # CMake 构建脚本
```

## 先决条件

在构建和运行此项目之前，请确保你的系统上已安装以下软件：

- **CMake** 版本 3.0 或更高
- **OpenCV** 版本 4.0 或更高
- **nlohmann/json** 版本 3.11.3 或更高（用于解析 JSON 配置文件）
- ArcSoft 人脸识别 SDK 及其对应的共享库文件

你可以通过以下命令在 Ubuntu 系统上安装必要的软件包：

```bash
sudo apt-get update
sudo apt-get install cmake libopencv-dev nlohmann-json3-dev
```

## 构建步骤

### 1. 克隆仓库

首先，将仓库克隆到本地：

```bash
git clone https://github.com/Mrjianning/ASFTestDemo_pro.git
cd ASFTestDemo_pro
```

### 2. 配置和构建

使用 CMake 进行配置并构建项目：

```bash
mkdir build
cd build
cmake ..
make
```

这将生成可执行文件 `arcsoft_face_engine_test`，存放在 `workspace/` 目录中。

## 使用方法

### 1. 运行程序

确保你已经将正确的配置文件 `config.json` 放在项 workspace 目录中，格式如下：

```json
{
    "apiKey": "your-api-key",
    "apiSecret": "your-api-secret",
    "licenseKey": "your-license-key"
}
```

使用以下命令运行程序：

```bash
./workspace/arcsoft_face_engine_test
```

### 2. 测试数据

你可以将测试图片或视频放在 `images/` 目录中，并在 `main.cpp` 中配置文件路径以进行测试。

## 贡献

欢迎提交问题（issues）和拉取请求（pull requests）以帮助改进本项目。如果你发现任何问题或有新的想法，请随时与我们联系。

## 许可证

该项目遵循 [MIT 许可证](LICENSE)。有关更多详细信息，请参阅 LICENSE 文件。

---

通过此 `README.md` 文件，用户可以轻松了解项目的目的、结构、构建步骤以及如何使用项目。如果你有更多的特定要求或要点，也可以进一步定制此文档。