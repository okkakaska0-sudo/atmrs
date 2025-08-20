#!/bin/bash

# Скрипт для Mac - автоматическое скачивание обновлений из Replit
# Сохраните этот файл на Mac и запускайте когда нужно обновиться

PROJECT_PATH="/Users/marselmacevans/Downloads/atmrs"
DOWNLOADS_PATH="/Users/marselmacevans/Downloads"

echo "🔄 AutoTune Plugin - Быстрое обновление из Replit"
echo ""

# Ищем последний скачанный архив
LATEST_SYNC=$(find "$DOWNLOADS_PATH" -name "*sync*.tar.gz" -mtime -1 | head -1)

if [ -n "$LATEST_SYNC" ]; then
    echo "📦 Найден архив: $(basename "$LATEST_SYNC")"
    
    # Распаковываем во временную папку
    TEMP_DIR=$(mktemp -d)
    tar -xzf "$LATEST_SYNC" -C "$TEMP_DIR"
    
    # Копируем файлы в проект
    if [ -d "$TEMP_DIR/sync_update" ]; then
        echo "🔄 Обновляю файлы проекта..."
        
        # Копируем исходники
        rsync -av "$TEMP_DIR/sync_update/Source/" "$PROJECT_PATH/Source/"
        
        # Копируем конфигурацию
        cp "$TEMP_DIR/sync_update/CMakeLists_macos_working.txt" "$PROJECT_PATH/"
        cp "$TEMP_DIR/sync_update/build_simple.sh" "$PROJECT_PATH/"
        cp "$TEMP_DIR/sync_update/"*.md "$PROJECT_PATH/" 2>/dev/null || true
        
        # Делаем скрипты исполняемыми
        chmod +x "$PROJECT_PATH/build_simple.sh"
        
        echo "✅ Файлы обновлены!"
        echo "🔨 Запускаю сборку..."
        
        cd "$PROJECT_PATH"
        ./build_simple.sh
        
    else
        echo "❌ Неправильный формат архива"
    fi
    
    # Очищаем
    rm -rf "$TEMP_DIR"
    
else
    echo "❌ Не найден архив синхронизации в Downloads"
    echo "💡 Скачайте autotune_quick_sync.tar.gz из Replit"
fi