#!/bin/bash

# Скрипт для автоматической синхронизации файлов с Mac
# Запускается в Replit для отправки изменений на Mac

echo "🔄 Синхронизация AutoTune Plugin с Mac..."

# Создаем архив только исходных файлов
tar -czf autotune_sync.tar.gz \
    Source/ \
    CMakeLists_macos_working.txt \
    build_simple.sh \
    replit.md \
    README_ASSISTANT.md

echo "📦 Архив создан: autotune_sync.tar.gz"
echo ""
echo "📁 Для синхронизации с Mac:"
echo "1. Скачайте файл autotune_sync.tar.gz из Files панели"
echo "2. На Mac выполните:"
echo "   cd /Users/marselmacevans/Downloads"
echo "   tar -xzf autotune_sync.tar.gz -C atmrs --strip-components=0"
echo "   cd atmrs && ./build_simple.sh"
echo ""
echo "✅ Готово к скачиванию!"