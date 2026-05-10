# media_daemon

Небольшая утилита на C++, которая периодически сканирует домашний каталог пользователя и формирует JSON-файл со списком мультимедийных файлов.

## Формат выходного файла

Файл `~/.media_files` в формате:

```json
{
  "audio":  [ "path/to/file1.mp3", "path/to/file2.wav" ],
  "video":  [ "path/to/video1.mkv" ],
  "images": [ "path/to/img1.jpeg", "path/to/img2.png" ]
}