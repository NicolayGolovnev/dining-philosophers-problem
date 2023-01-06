#include <iostream>
#include "ServerDaemon.h"
#include "Philosopher.h"

using namespace std;

int main() {
    // Для отображения русских символов в консоли
    system("chcp 65001");

    int philosophersCount = 5;

    cout << "Задача \"Голодающие философы\": " << endl;

    ServerDaemon *server = new ServerDaemon(philosophersCount);
    std::thread serverThread(&ServerDaemon::run, server);
    this_thread::sleep_for(chrono::milliseconds(1500));

    for (int i = 0; i < philosophersCount; i++) {
        Philosopher *philosopher = new Philosopher(i, server);
        std::thread philosopherThread(&Philosopher::run, philosopher);
        philosopherThread.detach();
    }

    serverThread.join();
    return 0;
}
