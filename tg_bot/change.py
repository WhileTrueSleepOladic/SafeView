import os
import subprocess

def process_video(input_path, output_path, cpp_processor_path):

    # Проверка существования входного файла
    if not os.path.exists(input_path):
        raise FileNotFoundError(f"Input video not found: {input_path}")
    
    # Проверка существования C++ обработчика
    if not os.path.exists(cpp_processor_path):
        raise FileNotFoundError(f"C++ processor not found: {cpp_processor_path}")
    
    # Вызов C++ программы
    try:
        subprocess.run(
            [cpp_processor_path, input_path, output_path],
            check=True
        )
        print(f"Видео обработано и сохранено в: {output_path}")
    except subprocess.CalledProcessError as e:
        print(f"Ошибка при выполнении C++ программы: {e}")
    except Exception as e:
        print(f"Общая ошибка: {e}")

