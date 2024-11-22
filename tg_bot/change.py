import os
from moviepy import VideoFileClip

def process_video(input_path, output_path):
    """
    копирует видео
    """
    clip = VideoFileClip(input_path)
    clip.write_videofile(output_path, codec="libx264")
    clip.close()
