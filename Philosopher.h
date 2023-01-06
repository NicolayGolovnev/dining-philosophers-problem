//
// Created by kolya on 06.01.2023.
//

#ifndef DINING_PHILOSOPHERS_PROBLEM_PHILOSOPHER_H
#define DINING_PHILOSOPHERS_PROBLEM_PHILOSOPHER_H

#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include "utils/DefinesUtil.h"
#include "ServerDaemon.h"

namespace {
    int sleep_for_random_time() {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(300, 1000);

        return dist(mt);
    }
}

class Philosopher {
private:
    int number;
    ServerDaemon *stuard;
    Semaphore *permission;

    // Вывод цельного сообщения в консоль (для правильного вывода сообщения в рамках одной строки)
    void log(std::string message) {
        std::stringstream msg;
        msg << "[PHILOS#" << this->number << "]\t" << message << "\n";
        std::cout << msg.str();
    }

    // Обдумывание философа
    void think() {
        this->log("Я думаю...");

        std::chrono::milliseconds duration(sleep_for_random_time());
        std::this_thread::sleep_for(duration);
    }

    // Запрос вилки для начала трапезы философом
    void getFork() {
        this->log("Прошу вилку для трапезы");
        stuard->request(RequestType::GET_FORK, this->number);

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    // Процесс трапезы философом и возврат вилок
    void eat() {
        permission->P();
        this->log("Начинаю свою трапезу");
        this->think();

        stuard->request(RequestType::RETURN_FORKS, this->number);
        this->log("Закончил трапезу, отдам я вилки другим");

        std::this_thread::sleep_for(std::chrono::milliseconds(350));
    }

public:
    Philosopher(int numberThread, ServerDaemon* server) {
        this->number = numberThread;
        this->stuard = server;
        this->permission = new Semaphore((PHILOSOPHER_NAME + std::to_string(this->number)).c_str(), false);
    }

    ~Philosopher() = default;

    void run() {
        this->think();
        for (int i = 0; i < 2; i++)
            this->getFork();
        this->eat();
    }
};

#endif //DINING_PHILOSOPHERS_PROBLEM_PHILOSOPHER_H
