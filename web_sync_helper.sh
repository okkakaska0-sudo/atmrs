#!/bin/bash

# Скрипт для периодической синхронизации с веб-Replit
echo "🔄 Запускаю автосинхронизацию с веб-Replit..."

PROJECT_PATH="/Users/marselmacevans/Downloads/atmrs"
BACKUP_PATH="$PROJECT_PATH.backup"

# Создаем backup
if [ ! -d "$BACKUP_PATH" ]; then
    echo "📦 Создаю резервную копию проекта..."
    cp -r "$PROJECT_PATH" "$BACKUP_PATH"
fi

echo "📁 Папка проекта: $PROJECT_PATH"
echo "💾 Резервная копия: $BACKUP_PATH"
echo ""
echo "🔄 Для синхронизации:"
echo "1. В веб-Replit нажмите на название проекта 'atmrs'"
echo "2. Выберите 'Download as ZIP'"
echo "3. Сохраните в Downloads как 'atmrs-latest.zip'"
echo "4. Этот скрипт автоматически обновит проект"
echo ""
echo "⚡ Отслеживаю Downloads папку для новых архивов..."

# Отслеживание Downloads папки
fswatch -o ~/Downloads/ | while read num_changes; do
    # Проверяем новые ZIP файлы
    LATEST_ZIP=$(find ~/Downloads -name "*atmrs*.zip" -mmin -1 2>/dev/null | head -1)
    
    if [ -n "$LATEST_ZIP" ]; then
        echo "$(date '+%H:%M:%S') 📦 Найден новый архив: $(basename "$LATEST_ZIP")"
        
        # Создаем временную папку
        TEMP_DIR=$(mktemp -d)
        
        # Распаковываем
        unzip -q "$LATEST_ZIP" -d "$TEMP_DIR"
        
        # Ищем папку с исходниками
        SOURCE_DIR=$(find "$TEMP_DIR" -name "Source" -type d | head -1)
        
        if [ -n "$SOURCE_DIR" ]; then
            echo "$(date '+%H:%M:%S') 🔄 Обновляю исходные файлы..."
            
            # Синхронизируем исходники
            rsync -av "$SOURCE_DIR/" "$PROJECT_PATH/Source/"
            
            # Синхронизируем конфигурационные файлы
            for file in "CMakeLists_macos_working.txt" "build_simple.sh" "*.md"; do
                SOURCE_FILE=$(find "$TEMP_DIR" -name "$file" | head -1)
                if [ -n "$SOURCE_FILE" ]; then
                    cp "$SOURCE_FILE" "$PROJECT_PATH/"
                fi
            done
            
            echo "$(date '+%H:%M:%S') ✅ Файлы обновлены!"
            echo "$(date '+%H:%M:%S') 🔨 Запускаю сборку..."
            
            # Запускаем сборку
            cd "$PROJECT_PATH"
            chmod +x build_simple.sh
            ./build_simple.sh
            
            echo "$(date '+%H:%M:%S') ✅ Синхронизация завершена!"
        else
            echo "$(date '+%H:%M:%S') ❌ Не найдена папка Source в архиве"
        fi
        
        # Очищаем
        rm -rf "$TEMP_DIR"
        
        # Перемещаем обработанный архив
        mkdir -p ~/Downloads/processed_zips
        mv "$LATEST_ZIP" ~/Downloads/processed_zips/
    fi
done