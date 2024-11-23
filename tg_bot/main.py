import telebot
import os
import subprocess
from moviepy import VideoFileClip
from config import BOT_TOKEN
from change import process_video

# Инициализация бота
bot = telebot.TeleBot(BOT_TOKEN)

# Папки для хранения видео
INPUT_DIR = "videos/input"
OUTPUT_DIR = "videos/output"
TEMP_DIR = "videos/temp"
os.makedirs(INPUT_DIR, exist_ok=True)
os.makedirs(OUTPUT_DIR, exist_ok=True)
os.makedirs(TEMP_DIR, exist_ok=True)

SUPPORTED_FORMATS = ['mp4', 'avi', 'mov', 'mkv', 'flv', 'wmv']

@bot.message_handler(commands=['start', 'help'])
def send_welcome(message):
    bot.reply_to(message, "Привет! Отправь мне видеофайл (MP4, AVI, MOV, MKV, FLV, WMV) в виде видео или документа, и я его обработаю!")

@bot.message_handler(content_types=['video', 'document'])
def handle_video_or_document(message):
    try:
        # Скачивание файла
        if message.content_type == 'video':
            file_info = bot.get_file(message.video.file_id)
        elif message.content_type == 'document':
            file_info = bot.get_file(message.document.file_id)
        else:
            bot.reply_to(message, "Этот тип файла не поддерживается.")
            return

        downloaded_file = bot.download_file(file_info.file_path)

        # Определение пути сохранения
        original_file = os.path.join(INPUT_DIR, file_info.file_path.split("/")[-1])
        with open(original_file, 'wb') as new_file:
            new_file.write(downloaded_file)

        # Проверка расширения
        extension = os.path.splitext(original_file)[-1].lower().lstrip(".")
        if extension not in SUPPORTED_FORMATS:
            bot.reply_to(message, f"Формат {extension} не поддерживается.")
            os.remove(original_file)
            return

        # Конвертация в MP4, если требуется
        input_path = original_file
        temp_path = os.path.join(TEMP_DIR, f"{message.chat.id}_temp.mp4")
        if extension != 'mp4':
            clip = VideoFileClip(original_file)
            clip.write_videofile(temp_path, codec="libx264")
            input_path = temp_path
            clip.close()

        # Путь для сохранения обработанного видео
        output_path = os.path.join(OUTPUT_DIR, f"{message.chat.id}_output.mp4")

        # Обработка видео
        process_video(input_path, output_path, "C:/Users/Mickic/Documents/HACKTON/tg_bot/video_processor.exe")

        # Отправка обработанного видео
        with open(output_path, 'rb') as video:
            bot.send_video(message.chat.id, video)

        # Удаление файлов
        os.remove(original_file)
        if os.path.exists(temp_path):
            os.remove(temp_path)
        os.remove(output_path)

    except Exception as e:
        bot.reply_to(message, f"Произошла ошибка: {e}")

@bot.message_handler(func=lambda message: True)
def echo_all(message):
    bot.reply_to(message, "Я понимаю только видео или документы, содержащие видеофайлы. Отправь мне что-то подходящее!")

# Запуск бота
bot.polling()
