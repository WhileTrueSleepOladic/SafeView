import subprocess

# Функция для обработки видео
def process_video(input_path, output_path, processor_path):
    try:
        result = subprocess.run(
            [processor_path, input_path, output_path],
            capture_output=True,
            text=True
        )
        if result.returncode != 0:
            raise RuntimeError(result.stderr)
        return True
    except Exception as e:
        return str(e)
