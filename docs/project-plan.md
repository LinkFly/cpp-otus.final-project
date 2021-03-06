# Интепретатор языка лисп (диалект Common Lisp)

### План
- Общее описание
  - Придумать название
  - [OK] Составить описание работы
  - [OK] Пояснение про семейство языков Лисп
  
- Возможности к реализации (перечень сущностей языка, которые планируется реализовать в 
проектной работе)
  - Реализация/выполнение run-time'а с базовыми функциями (работа ф-ии eval)
    - OK типы: symbol/atom/cons/number/function/string
    - ф-ии:
      - базовые:
          - OK управляющие вычислением: 
            - OK quote
            - OK eval
            - OK apply
          - OK операции с данными: car, cdr, cons
          - OK предикаты: symbolp, atomp, consp, numberp, functionp, stringp
          - OK работающие с контекстом вычисления:
            - OK let
            - OK lambda
          - OK управляющие:
            - OK if
            - OK tagbody
          - OK арифметические: +, -, :, *
          - OK деформирующие: setq          

  - OK Загрузка файла c программой в run-time (ф-ия load)
  - OK Поддержка работы с системой через консоль (Repl)
  - OK Выход из системы - (quit)
  - OK Консольный интерфейс
    - Ключ --eval,e "<выражение>" которое вычислит интепретатор
    - Ключ --load,l <файл> скрипт с лисп-кодом
    - <без_ключей) - активируется Repl

- Схемы работы
  ...
 
- Реализация
  - ...
  - Тесты
  
- Примеры
  - Пример загрузки скрипта с кодом, и вывода результатов
  - Пример определения ф-ии while, семантика (while <sexpr> <sexpr>*)

---------------------------------------------
- Дополнительные возможности (если останется время)
  - Определение макросов defun/defmacro (хотя бы с частью семантики передачи аргументов)
  - Всевозможные оптимизации
    - ...
  - Дополнения обеспечивающие вызов некоторых системных ф-ий
