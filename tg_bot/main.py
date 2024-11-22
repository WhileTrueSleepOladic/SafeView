import telebot
import os
from config import BOT_TOKEN
from change import process_video

# Инициализация бота
bot = telebot.TeleBot(BOT_TOKEN)

# Папки для хранения видео
INPUT_DIR = "videos/input"
OUTPUT_DIR = "videos/output"
os.makedirs(INPUT_DIR, exist_ok=True)
os.makedirs(OUTPUT_DIR, exist_ok=True)

@bot.message_handler(commands=['start', 'help'])
def send_welcome(message):
    bot.reply_to(message, "Привет! Отправь мне видео, и я его обработаю!")

@bot.message_handler(content_types=['video'])
def handle_video(message):
    try:
        # Скачивание видео
        file_info = bot.get_file(message.video.file_id)
        downloaded_file = bot.download_file(file_info.file_path)

        input_path = os.path.join(INPUT_DIR, f"{message.chat.id}_input.mp4")
        output_path = os.path.join(OUTPUT_DIR, f"{message.chat.id}_output.mp4")

        # Сохранение файла
        with open(input_path, 'wb') as new_file:
            new_file.write(downloaded_file)

        # Обработка видео
        process_video(input_path, output_path)

        # Отправка обработанного видео
        with open(output_path, 'rb') as video:
            bot.send_video(message.chat.id, video)

    except Exception as e:
        bot.reply_to(message, f"Произошла ошибка: {e}")

@bot.message_handler(func=lambda message: True)
def echo_all(message):
    bot.reply_to(message, "Я понимаю только видео. Отправь мне видео!")

# Запуск бота
bot.polling()
