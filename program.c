#include <stdio.h>


void handle_error(int cpu_state[], int error_code, int *interrupt_active);


int main(){
    int memory[256];
    int cpu_state[8];
    int memory_size = 0;
    int interrupt_active = 0;
    int i;

    // Обнуляем регистры процессора
    for (i = 0; i < 8; i++){
        cpu_state[i] = 0;
    }

    // Загрузка программы в память
    while (memory_size < 256 && scanf("%d", &memory[memory_size]) == 1){
        memory_size++;
    }

    // Программа содержит хотя бы одну команду
    if (memory_size <= 6)
    {
        printf("Ошибка: недостаточно данных\n");
        return 1;
    }

    // Инициализизация процессора
    cpu_state[0] = 6;           // PC
    cpu_state[1] = 0;           // ACC
    cpu_state[2] = 0;           // IR
    cpu_state[3] = memory[0];   // Вектор прерываний
    cpu_state[4] = 0;           // Флаг остановки
    cpu_state[5] = 0;           // Код ошибки

    // Проверка загрузки
    printf("Загружено ячеек памяти: %d\n", memory_size);
    // for (i = 0; i < memory_size; i++){
    //     printf("memory[%d] = %d\n", i, memory[i]);
    // }

    
    // Цикл выполнения команд процессора
    while (cpu_state[4] == 0){
        int current_pc = cpu_state[0];
        int operand = -1;
        int has_operand = 0;

        // Ошибка адреса команды, код:1
        if (current_pc < 6 || current_pc >= memory_size)
        {
            handle_error(cpu_state, 1, &interrupt_active);
            continue;
        }

        cpu_state[2] = memory[cpu_state[0]];

        switch (cpu_state[2]){
            // Остановка процессора
            case 0:
                cpu_state[4] = 1;
                break;
        
            // Вывод аккумулятора
            case 1:
                printf("Значение аккумулятора: %d\n", cpu_state[1]);
                cpu_state[0]++;
                break;
            
            // Загрузка значения из памяти
            case 2:
                // Ошибка наличия операнда, код:2
                if (current_pc + 1 >= memory_size)
                {
                    handle_error(cpu_state, 2, &interrupt_active);
                    continue;
                }

                operand = memory[current_pc + 1];
                has_operand = 1;

                // Ошибка адреса операнда, код: 3
                if (operand <= 0 || operand >= memory_size)
                {
                    handle_error(cpu_state, 3, &interrupt_active);
                    continue;
                }

                cpu_state[1] = memory[operand];
                cpu_state[0] += 2;
                break;

            // Умножение аккумулятора
            case 3:
                // Ошибка наличия операнда, код:2
                if (current_pc + 1 >= memory_size)
                {
                    handle_error(cpu_state, 2, &interrupt_active);
                    continue;
                }

                operand = memory[current_pc + 1];
                has_operand = 1;

                // Ошибка адреса операнда, код: 3
                if (operand <= 0 || operand >= memory_size)
                {
                    handle_error(cpu_state, 3, &interrupt_active);
                    continue;
                }

                cpu_state[1] *= memory[operand];
                cpu_state[0] += 2;
                break;
            
            // Безусловный переход
            case 4:
                // Ошибка наличия операнда, код:2
                if (current_pc + 1 >= memory_size)
                {
                    handle_error(cpu_state, 2, &interrupt_active);
                    continue;
                }

                operand = memory[current_pc + 1];
                has_operand = 1;

                // Ошибка адреса операнда, код: 3
                if (operand < 6 || operand >= memory_size)
                {
                    handle_error(cpu_state, 3, &interrupt_active);
                    continue;
                }

                cpu_state[0] = operand;
                break;

            // Условный переход
            case 5:
                // Ошибка наличия операнда, код:2
                if (current_pc + 1 >= memory_size)
                {
                    handle_error(cpu_state, 2, &interrupt_active);
                    continue;
                }

                operand = memory[current_pc + 1];
                has_operand = 1;

                // Ошибка адреса операнда, код: 3
                if (operand < 6 || operand >= memory_size)
                {
                    handle_error(cpu_state, 3, &interrupt_active);
                    continue;
                }

                if (cpu_state[1] == 0){
                    cpu_state[0] = operand;
                }
                else{
                    cpu_state[0] += 2;
                }
                break;

            // Ошибка: Неизвестная команда, код:4
            default:
                handle_error(cpu_state, 4, &interrupt_active);
                continue;
        }

        // Вывод информации о выполненной команде
        printf("pc=%d: команда %d, ", current_pc, cpu_state[2]);
        if (has_operand == 1){
            printf("operand=%d, ", operand);
        }
        else{
            printf("operand=нет, ");
        }
        printf("acc=%d\n", cpu_state[1]);
    }

    printf("Программа завершена.\n");
    return 0;
}


void handle_error(int cpu_state[], int error_code, int *interrupt_active)
{
    printf("Ошибка! Код: %d\n", error_code);
    cpu_state[5] = error_code;

    if (*interrupt_active == 0){
        *interrupt_active = 1;
        cpu_state[0] = cpu_state[3];
    }
    else{
        printf("Повторное прерывание!\n");
        cpu_state[4] = 1;
    }
}