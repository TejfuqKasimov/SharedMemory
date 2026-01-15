# Инструкция по использованию Makefile

## Для работы с Классом
```bash
cd shmWhileTouchClass/
```

### Сначала перейдите в папку example
```bash
cd example/
```

### И создайте папку build
```bash
mkdir build
```

### Сборка всех программ
```bash
make all
```
или просто:
```bash
make
```

### Сборка отдельных программ
```bash
make reader
make writer
```

## Очистка

### Удаление скомпилированных файлов
```bash
make clean
```
Удаляет файлы:
- `build/writer`
- `build/reader`

## Запуск программ

### Запуск reader
```bash
make run_reader
```

### Запуск writer
```bash
make run_writer
```

## Используемые параметры компиляции

| Параметр | Описание |
|----------|----------|
| `-std=c++11` | Стандарт C++11 |
| `-Wall -Wextra` | Все предупреждения компилятора |
| `-pthread` | Поддержка многопоточности |
| `-O2` | Оптимизация уровня 2 |
| `-lrt` | Библиотека реального времени |
| `-lpthread` | Потоковая библиотека |

## Генерируемые файлы

После успешной компиляции в директории `build/` создадутся:
- `reader` - исполняемый файл программы reader
- `writer` - исполняемый файл программы writer

## Примеры использования

### Полный цикл: сборка и запуск writer
```bash
make writer
make run_writer
```

### Полный цикл: сборка и запуск reader
```bash
make reader
make run_reader
```

### Пересборка с очисткой
```bash
make clean
make all
```
