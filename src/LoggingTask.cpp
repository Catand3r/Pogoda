#include "loggingtask.h"
#include "logger.h"

void LoggingTask::Run()
{
    Logger::getInstance().Write();
}
