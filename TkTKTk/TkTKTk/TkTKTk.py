
import tkinter as tk
from tkinter import filedialog, ttk, messagebox
import subprocess
import threading
import os

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

# Функция для выбора файла
def select_file(entry_field, file_type):
    file_path = filedialog.askopenfilename(
        filetypes=[(file_type, "*.mp4"), ("Все файлы", "*.*")]
    )
    if file_path:
        entry_field.delete(0, tk.END)
        entry_field.insert(0, file_path)

# Функция для выбора папки
def select_output(entry_field):
    folder_path = filedialog.asksaveasfilename(
        defaultextension=".mp4",
        filetypes=[("MP4 файлы", "*.mp4")]
    )
    if folder_path:
        entry_field.delete(0, tk.END)
        entry_field.insert(0, folder_path)

# Функция запуска обработки
def start_processing(input_path, output_path, processor_path, progress_bar, start_button):
    if not os.path.isfile(input_path):
        messagebox.showerror("Ошибка", "Указан некорректный путь к входному видео.")
        return

    if not os.path.isfile(processor_path):
        messagebox.showerror("Ошибка", "Не найден исполняемый файл обработчика видео.")
        return

    if not output_path.endswith(".mp4"):
        messagebox.showerror("Ошибка", "Выходной файл должен иметь расширение .mp4.")
        return

    def run_processing():
        try:
            # Обновление прогресса
            progress_bar.start(10)
            success = process_video(input_path, output_path, processor_path)
            progress_bar.stop()
            if success is True:
                messagebox.showinfo("Успех", "Видео успешно обработано!")
            else:
                messagebox.showerror("Ошибка", f"Ошибка обработки: {success}")
        except Exception as e:
            messagebox.showerror("Ошибка", str(e))
        finally:
            progress_bar.stop()
            start_button.config(state=tk.NORMAL)

    # Блокировка кнопки, пока идет обработка
    start_button.config(state=tk.DISABLED)
    threading.Thread(target=run_processing, daemon=True).start()

# Основное окно Tkinter
def main():
    root = tk.Tk()
    root.title("Видео обработчик")
    root.geometry("500x300")

    # Входные данные
    tk.Label(root, text="Входное видео:").pack(anchor="w", padx=10, pady=5)
    input_path_entry = tk.Entry(root, width=50)
    input_path_entry.pack(anchor="w", padx=10, pady=5)
    tk.Button(root, text="Выбрать", command=lambda: select_file(input_path_entry, "MP4 файлы")).pack(anchor="w", padx=10)

    # Выходной файл
    tk.Label(root, text="Выходное видео:").pack(anchor="w", padx=10, pady=5)
    output_path_entry = tk.Entry(root, width=50)
    output_path_entry.pack(anchor="w", padx=10, pady=5)
    tk.Button(root, text="Сохранить как", command=lambda: select_output(output_path_entry)).pack(anchor="w", padx=10)

    # Путь к обработчику
    tk.Label(root, text="Путь к процессору видео:").pack(anchor="w", padx=10, pady=5)
    processor_path_entry = tk.Entry(root, width=50)
    processor_path_entry.pack(anchor="w", padx=10, pady=5)
    processor_path_entry.insert(0, "C:/Users/Mickic/Documents/HACKTON/tg_bot/video_processor.exe")

    # Прогресс-бар
    progress_bar = ttk.Progressbar(root, orient="horizontal", mode="indeterminate", length=400)
    progress_bar.pack(pady=20)

    # Кнопка "Начать"
    start_button = tk.Button(
        root, 
        text="Начать обработку", 
        command=lambda: start_processing(
            input_path_entry.get(),
            output_path_entry.get(),
            processor_path_entry.get(),
            progress_bar,
            start_button
        )
    )
    start_button.pack(pady=10)

    root.mainloop()

if __name__ == "__main__":
    main()
