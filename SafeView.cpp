#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>

using namespace std;
using namespace cv;
using namespace cv::dnn;

const float CONFIDENCE_THRESHOLD = 0.4;
const int INPUT_WIDTH = 600;
const int INPUT_HEIGHT = 600;

// Пути к модели и конфигурации
// 
// const string MODEL_PATH = "D:/Apps/OpenCV/opencv/sources/samples/dnn/face_detector/linear_svm.xml";
// const string MODEL_PATH = "D:/Apps/OpenCV/opencv/sources/samples/dnn/face_detector/new_model_1.caffemodel";
// const string MODEL_PATH = "D:/Apps/OpenCV/opencv/sources/samples/dnn/face_detector/new_model_2.caffemodel";
// const string MODEL_PATH = "D:/Apps/OpenCV/opencv/sources/samples/dnn/face_detector/bvlc_alexnet.caffemodel";

const string MODEL_PATH = "D:/Apps/OpenCV/opencv/sources/samples/dnn/face_detector/res10_300x300_ssd_iter_140000.caffemodel";
const string CONFIG_PATH = "D:/Apps/OpenCV/opencv/sources/samples/dnn/face_detector/deploy.prototxt";

// const int VIDEO_PATH = 0; // if need to see cam
const string VIDEO_PATH = "D:/Apps/Videos/human_faces_2.mp4";

void detectAndBlurFaces(Mat& frame, Net& net);

int main() {
    // Загрузка модели DNN
    Net net = readNetFromCaffe(CONFIG_PATH, MODEL_PATH);
    if (net.empty()) {
        cerr << "Ошибка: Не удалось загрузить модель DNN!" << endl;
        return -1;
    }

    // Запуск захвата видео (веб-камера)
    VideoCapture capture(VIDEO_PATH);
    if (!capture.isOpened()) {
        cerr << "Ошибка: Не удалось открыть камеру!" << endl;
        return -1;
    }

    Mat frame;
    while (capture.read(frame)) {
        if (frame.empty()) break;

        // Обнаружение лиц и блюр
        detectAndBlurFaces(frame, net);

        // Отображение кадра
        imshow("Обнаружение лиц с размытием", frame);

        char c = (char)waitKey(10);
        if (c == 27 || c == 'q' || c == 'й' || c == 'Й') break; // Выход
    }

    return 0;
}

void detectAndBlurFaces(Mat& frame, Net& net) {
    // Создание блоба из изображения
    Mat blob = blobFromImage(frame, 0.9, Size(INPUT_WIDTH, INPUT_HEIGHT), Scalar(104.0, 177.0, 123.0), false, false);
    net.setInput(blob);

    // Обнаружение лиц
    Mat detections = net.forward();

    // Разбор результатов
    Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());
    for (int i = 0; i < detectionMat.rows; i++) {
        float confidence = detectionMat.at<float>(i, 2);

        if (confidence > CONFIDENCE_THRESHOLD) {
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

            // Проверка границ (на случай некорректных координат)

            x1 = max(0, x1);
            y1 = max(0, y1);
            x2 = min(frame.cols - 1, x2);
            y2 = min(frame.rows - 1, y2);

            // Размытие области лица
            Rect faceRect(x1, y1, x2 - x1, y2 - y1);
            Mat faceROI = frame(faceRect); // ROI лица
            GaussianBlur(faceROI, faceROI, Size(51, 51), 30); // Применение размытия
            
            putText(frame, format("Confidence: %.2f", confidence), Point(x1, y1 - 10),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 1);
               
        }   
    }
}
