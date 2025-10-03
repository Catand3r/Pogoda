#include "LoggingTask.h"

void LoggingTask::Run()
{
    Logger::getInstance().Write();
}