#pragma once

enum class DO_ACTION {
    UNKNOWN = 0,
    INSTALL = 1,
    UNINSTALL = 2,
    CLI_MODE = 3,
};

enum class TARGET_TYPE {
    UNKNOWN = 0,
    PROC = 1,
    SVC = 2,
};
