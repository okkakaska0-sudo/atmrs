#!/bin/bash

# Быстрая синхронизация AutoTune Plugin с Mac
echo "🔄 Создаю обновленные файлы для Mac..."

# Создаем папку для синхронизации
mkdir -p sync_update/Source

# Копируем только нужные файлы
cp Source/*.cpp Source/*.h sync_update/Source/
cp CMakeLists_macos_working.txt sync_update/
cp build_simple.sh sync_update/
cp *.md sync_update/

# Создаем скрипт для Mac
cat > sync_update/update_on_mac.sh << 'EOF'
#!/bin/bash
echo "🔄 Обновляю AutoTune Plugin..."

# Копируем файлы в проект
rsync -av Source/ /Users/marselmacevans/Downloads/atmrs/Source/
cp CMakeLists_macos_working.txt /Users/marselmacevans/Downloads/atmrs/
cp build_simple.sh /Users/marselmacevans/Downloads/atmrs/
cp *.md /Users/marselmacevans/Downloads/atmrs/

echo "✅ Файлы обновлены! Запускаю сборку..."
cd /Users/marselmacevans/Downloads/atmrs
chmod +x build_simple.sh
./build_simple.sh
EOF

chmod +x sync_update/update_on_mac.sh

# Упаковываем
tar -czf autotune_quick_sync.tar.gz sync_update/
rm -rf sync_update

echo "✅ Готово! Скачайте autotune_quick_sync.tar.gz"
echo ""
echo "На Mac распакуйте и запустите:"
echo "  tar -xzf autotune_quick_sync.tar.gz"
echo "  cd sync_update"
echo "  ./update_on_mac.sh"