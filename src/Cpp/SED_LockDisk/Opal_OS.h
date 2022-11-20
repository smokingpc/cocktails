#pragma once

//Opal OS is the OS wrapper for OPAL command sender/receiver.
//Windows and Linux use different command communicating with storage device.
//e.g. Discovery0 command in windows use SCSIOP_SECURITY_PROTOCOL_IN with opal packaet.
//     But in linux, it use NVME_SECURITY_RECV command with opal packet.

class COpalOS{
    
};

class COpalWin : COpalOS{};
class COpalLinux : COpalOS {};


