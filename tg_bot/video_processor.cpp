#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    // Проверяем количество аргументов
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_path> <output_path>" << std::endl;
        return 1;
    }

    // Получаем пути к файлам
    std::string input_path = argv[1];
    std::string output_path = argv[2];

    // Открываем входной файл
    std::ifstream input_file(input_path, std::ios::binary);
    if (!input_file) {
        std::cerr << "Error: Cannot open input file: " << input_path << std::endl;
        return 1;
    }

    // Открываем выходной файл
    std::ofstream output_file(output_path, std::ios::binary);
    if (!output_file) {
        std::cerr << "Error: Cannot create output file: " << output_path << std::endl;
        return 1;
    }

    // Копируем содержимое файла
    output_file << input_file.rdbuf();

    // Закрываем файлы
    input_file.close();
    output_file.close();

    std::cout << "Processing complete. Output saved to: " << output_path << std::endl;
    return 0;
}
