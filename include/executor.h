#pragma once
#include "itask.h"

class Executor
{
  public:
    static Executor &getInstance();

    void execute(ITask &task);

  private:
    Executor() = default;
    Executor(const Executor &) = delete;
    Executor &operator=(const Executor &) = delete;
};
