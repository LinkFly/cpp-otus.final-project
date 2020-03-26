# История разработки

- Описание, план

- Разработка через UML-диаграммы (пришлось отказаться, перейти в кодирование, а затем немного вернуться к диаграммам)

- Создание рабочей модели для проверки концепции (Proof Of Concept)

- Оперативная реализация заявленного функционала

- Большой рефакторинг и чистка проектаа

- Рефакторинг, описание и создание диаграмм

# Главные элементы репозитория

- Основные части репозитория

- Внутренная структура и работа системы

-------------------------
## Основные части проекта (на которых хочется сделать акцент)

- Readme.md, документация, диаграммы

- Тесты: - LispEngine/tests/tests.cpp, 

- Исходники: LispEngine/src


#### Readme.md, документация, диаграммы

- Директория docs
    - план проекта: project-plan.md
    - описание внутреннего устройства: internal.md
    - начало создания UML диаграм: docs/diagrams/LispEngine.vpp

#### Тесты
    
- Действующий функционал системы можно узнать посмотрев на тесты

#### Исходники

- Базовые элементы: base/

- Утилиты: utils/

- Интерфейсы: interfaces/

- Реализации интерфейсов: implements/

- Точка входа / обработка аргументов: main.cpp, Arguments.cpp

----------------------------

## Внутренная структура и работа системы

- Взаимосвязь интерфейсов

- Ключевые подсистемы

    - LispEngine

    - Repl

    - Reader

    - Evaluator

