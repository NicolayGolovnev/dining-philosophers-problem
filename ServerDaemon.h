//
// Created by kolya on 06.01.2023.
//

#ifndef DINING_PHILOSOPHERS_PROBLEM_SERVERDAEMON_H
#define DINING_PHILOSOPHERS_PROBLEM_SERVERDAEMON_H

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <sstream>

#include "utils/Semaphore.h"
#include "utils/DefinesUtil.h"
#include "utils/RequestType.h"

class ServerDaemon {
private:
    std::queue<std::pair<RequestType, int>> requestPool; // <"Запрос", "Номер философа">
    std::mutex sync;

    int forksCount;
    std::vector<int> busyForksByPhilosophers;
    std::vector<Semaphore *> permissions;

    std::vector<int> hungryPhilosophers;

    // Поиск голодающего философа по его идентификатору
    int isPhilosopherStarvation(int philosopherNumber) {
        for (int i = 0; i < this->hungryPhilosophers.size(); i++)
            if (philosopherNumber == hungryPhilosophers[i]) return i;

        return -1;
    }

    // Вывод цельного сообщения в консоль (для правильного вывода сообщения в рамках одной строки)
    void log(std::string message) {
        std::stringstream msg;
        msg << "[SERVER##]\t" << message << "\n";
        std::cout << msg.str();
    }

public:
    ServerDaemon(int philosophersCount) {
        this->forksCount = philosophersCount;
        for (int i = 0; i < this->forksCount; i++) {
            busyForksByPhilosophers.push_back(0);
            permissions.push_back(new Semaphore((PHILOSOPHER_NAME + std::to_string(i)).c_str(), false));
        }

        this->sync.unlock();
    }

    // Метод на добавление запроса в общий пул
    void request(RequestType request, int numberOfPhilosopher) {
        sync.lock();
        requestPool.push(std::make_pair(request, numberOfPhilosopher));
        sync.unlock();
    }

    void run() {
        while (true) {
            if (!this->requestPool.empty()) {
                sync.lock();
                std::pair<RequestType, int> request = requestPool.front();

                switch (request.first) {
                    case RequestType::GET_FORK:
                        // Если вилок в данным момент вообще нету - ожидаем их появление позже
                        if (forksCount < 1) {
                            int index = this->isPhilosopherStarvation(request.second);
                            if (index == -1)
                                hungryPhilosophers.push_back(request.second);
                            requestPool.push(request);

                            this->log("Философ #" + std::to_string(request.second) +
                                      " не получает вилку, так как их нету!");
                        }
                        // Если есть еще одна вилка, но философ не начнет кушать - предотвращаем deadlock
                        else if (forksCount < 2 && busyForksByPhilosophers[request.second] == 0) {
                            int index = this->isPhilosopherStarvation(request.second);
                            if (index == -1)
                                hungryPhilosophers.push_back(request.second);
                            requestPool.push(request);

                            this->log("Философ #" + std::to_string(request.second) + " не получает вилку (предотвратили deadlock)!");
                        } else {
                            forksCount--;
                            this->log("Философ #" + std::to_string(request.second) + " получил вилку!");

                            // Если у философа 2 вилки - можно приступать к трапезе
                            if (++busyForksByPhilosophers[request.second] == 2) {
                                permissions[request.second]->V();
                                this->log("Философ #" + std::to_string(request.second) + " начинает кушать!");
                            }
                        }
                        break;

                    case RequestType::RETURN_FORKS:
                        if (busyForksByPhilosophers[request.second] == 2) {
                            busyForksByPhilosophers[request.second] = 0;
                            int index = this->isPhilosopherStarvation(request.second);
                            if (index != -1)
                                hungryPhilosophers.erase(hungryPhilosophers.begin() + index);
                            forksCount += 2;
                            this->log("Философ #" + std::to_string(request.second) + " вернул 2 вилки!");
                        } else
                            this->log("Невозможно вернуть вилки, которых не существует!");
                        break;

                    default:
                        this->log("Не найдено подходящего запроса в списке всех запросов");
                        break;
                }
                requestPool.pop();
                sync.unlock();
            } else this->log("Очередь запросов на сервер пустует...");

            this->log("Голодающих философов: " + std::to_string(hungryPhilosophers.size()));
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        }
    }
};


#endif //DINING_PHILOSOPHERS_PROBLEM_SERVERDAEMON_H
