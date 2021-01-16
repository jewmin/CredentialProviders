#pragma once

#include "IWinlogon.h"

class Gina {
public:

public:
    Gina(IWinlogon * winlogon);
    ~Gina();

private:
    IWinlogon * winlogon_;
};
