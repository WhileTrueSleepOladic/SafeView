#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>

using namespace std;
using namespace cv;
using namespace cv::dnn;

// Параметры программы:
const float CONFIDENCE_THRESHOLD = 0.4; // Пороговая точность определения объекта
const int INPUT_WIDTH = 640; // Оптимальная высота
const int INPUT_HEIGHT = 360; // Оптимальная ширина кадра 
const float base = 0.6; // Сжатие изображения 

// Пути к модели и конфигурации
const string MODEL_PATH = "D:/Apps/OpenCV/opencv/sources/samples/dnn/face_detector/core_model.caffemodel";
const string CONFIG_PATH = "D:/Apps/OpenCV/opencv/sources/samples/dnn/face_detector/deploy.prototxt";

const string INPUT_VIDEO_PATH = "D:/Apps/Videos/Input/human_faces_4_input.mp4";
const string OUTPUT_VIDEO_PATH = "D:/Apps/Videos/Output/human_faces_4_output.mp4";

void detectAndBlurFaces(Mat& frame, Net& net);

int main() {
    // Загрузка модели DNN
    Net net = readNetFromCaffe(CONFIG_PATH, MODEL_PATH);
    if (net.empty()) {
        cerr << "Ошибка: Не удалось загрузить модель DNN!" << endl;
        return -1;
    }

    // Запуск захвата видео (веб-камера)
    VideoCapture capture(INPUT_VIDEO_PATH);
    if (!capture.isOpened()) {
        cerr << "Ошибка: Не удалось открыть камеру!" << endl;
        return -1;
    }

    // Получение информации о видео
    int frameWidth = static_cast<int>(capture.get(CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(capture.get(CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(capture.get(CAP_PROP_FPS));

    // Создание VideoWriter
    VideoWriter writer(OUTPUT_VIDEO_PATH, VideoWriter::fourcc('m', 'p', '4', 'v'), fps, Size(frameWidth, frameHeight));
    if (!writer.isOpened()) {
        cerr << "Ошибка: Не удалось открыть выходной видеофайл!" << endl;
        return -1;
    }
     
    Mat frame;
    while (capture.read(frame)) {
        if (frame.empty()) break;
        // Обнаружение лиц и блюр
        detectAndBlurFaces(frame, net);

        // Запись кадра в итоговый файл
        writer.write(frame);

        // Очистка памяти
        frame.release();
        
        char c = (char)waitKey(10);
        if (c == 27 || c == 'q' || c == 'й' || c == 'Й') break; // Выход
    }
    return 0;
}

void detectAndBlurFaces(Mat& frame, Net& net) {
    // Создание блоба из изображения
    Mat blob = blobFromImage(frame, base, Size(INPUT_WIDTH, INPUT_HEIGHT), Scalar(104.0, 177.0, 123.0), false, false);
    net.setInput(blob);

    // Обнаружение лиц
    Mat detections = net.forward();

    // Получение матрицы с результатами
    Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());
    for (int i = 0; i < detectionMat.rows; i++) {
        float confidence = detectionMat.at<float>(i, 2);

        // Получение координат объектов из матрицы
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
            Mat faceROI = frame(faceRect); // Матрица пикселей лица
            GaussianBlur(faceROI, faceROI, Size(91, 91), 30); // Применение размытия
        }   
    }
}
