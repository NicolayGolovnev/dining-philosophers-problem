//
// Created by kolya on 06.11.2022.
//

#ifndef DINING_PHILOSOPHERS_PROBLEM_SEMAPHORE_H
#define DINING_PHILOSOPHERS_PROBLEM_SEMAPHORE_H

#include <windows.h>
#include <winbase.h>
#include <string>

class Semaphore {
private:
    HANDLE sem;
public:
    /**
     * Конструктор создания семафора
     *
     * @param name названия семафора с приставкой "SEM_"
     * @param startState начальное состояние: 1 - открыт, 0 - закрыт
     */
    Semaphore(const char *name, bool startState) {
        std::string buf = "SEM_" + std::string(name);
        const char *newName = buf.c_str();

        // LPCSTR - длинный указатель на постоянную строку
        this->sem = OpenSemaphoreA(
                SEMAPHORE_ALL_ACCESS,
                true,
                (LPCSTR) newName
                );
        if (this->sem == nullptr){
            this->sem = CreateSemaphoreA(nullptr, startState, 1, (LPCSTR) newName);
//            printf("Create semaphore %s\n", newName);
        }
    }
    ~Semaphore() = default;

    /** Ожидаем открытия семафора для выполнения действий */
    long P(long milliseconds = INFINITE) { return WaitForSingleObject(this->sem, milliseconds); }

    /** Освобождаем семафор для лругих потоков */
    void V() { ReleaseSemaphore(this->sem, 1, nullptr); }
};

#endif //DINING_PHILOSOPHERS_PROBLEM_SEMAPHORE_H
