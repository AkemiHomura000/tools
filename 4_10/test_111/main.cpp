#include<iostream>
#include <QApplication>
#include <spdlog/spdlog.h>

#include "mainwindow.h"

int main(int argc, char* argv[])
{
    spdlog::set_level(spdlog::level::debug);

    // spdlog::debug("This is a debug message.");
    // spdlog::info("This is an info message.");
    // spdlog::warn("This is a warning message.");
    // spdlog::error("This is an error message.");
    // spdlog::critical("This is a critical message.");

    QApplication a(argc, argv);
    MainWindow w;
    w.setGeometry(100, 100, 1200, 600);
    w.show();
    return a.exec();
}
