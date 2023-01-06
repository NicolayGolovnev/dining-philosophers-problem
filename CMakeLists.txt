cmake_minimum_required(VERSION 3.23)
project(dining_philosophers_problem)

set(CMAKE_CXX_STANDARD 14)

add_executable(dining_philosophers_problem main.cpp
        utils/Semaphore.h
        Philosopher.h ServerDaemon.h utils/DefinesUtil.h utils/RequestType.h)
