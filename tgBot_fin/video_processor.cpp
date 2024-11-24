#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>

using namespace std;
using namespace cv;
using namespace cv::dnn;

const float CONFIDENCE_THRESHOLD = 0.4;
const int INPUT_WIDTH = 640;
const int INPUT_HEIGHT = 320;

// Пути к модели и конфигурации
const string MODEL_PATH = "C:/opencv/sources/samples/dnn/face_detector/res10_300x300_ssd_iter_140000.caffemodel";
const string CONFIG_PATH = "C:/opencv/sources/samples/dnn/face_detector/deploy.prototxt";

/*
const string VIDEO_PATH = "D:/Apps/Videos/Input/human_faces_1_input.mp4";
const string OUTPUT_VIDEO_PATH = "D:/Apps/Videos/Output/human_faces_1_output.mp4";
*/

void detectAndBlurFaces(Mat& frame, Net& net);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_path> <output_path>" << std::endl;
        return 1;
    }

    // Получаем пути к файлам
    string input_path = argv[1];
    string output_path = argv[2];

    // Загрузка модели DNN
    Net net = readNetFromCaffe(CONFIG_PATH, MODEL_PATH);
    if (net.empty()) {
        cerr << "Ошибка: Не удалось загрузить модель DNN!" << endl;
        return -1;
    }

    // Запуск видео 
    VideoCapture capture(input_path);
    if (!capture.isOpened()) {
        cerr << "Ошибка: Не удалось открыть камеру!" << endl;
        return -1;
    }

    // Получение информации о видео
    int frameWidth = static_cast<int>(capture.get(CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(capture.get(CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(capture.get(CAP_PROP_FPS));

    // Создание VideoWriter
    VideoWriter writer(output_path, VideoWriter::fourcc('m', 'p', '4', 'v'), fps, Size(frameWidth, frameHeight));
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
        
        char c = (char)waitKey(10);
        if (c == 27 || c == 'q' || c == 'й' || c == 'Й') break; // Выход
    }

    return 0;
}

void detectAndBlurFaces(Mat& frame, Net& net) {
    // Создание блоба из изображения
    Mat blob = blobFromImage(frame, 0.7, Size(INPUT_WIDTH, INPUT_HEIGHT), Scalar(104.0, 177.0, 123.0), false, false);
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
        }   
    }
}