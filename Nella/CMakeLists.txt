# Собираем игру
add_executable(Nella src/main.cpp)

# Подключаем к игре наш движок
target_link_libraries(Nella PRIVATE AMGR_Engine)

# Автоматически копируем папку shaders к готовому .exe файлу
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})