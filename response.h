#ifndef RESPONSE_H
#define RESPONSE_H

#include <QString>

struct SResponse {
    QString Version = "";
    double U = 0;
    QString DeviceId = "";
    int CmdNumRsp = -1;
    int StatusRelay = -1;
    int Input = 0;
    int Relay3 = 0;
    int Relay2 = 0;
    int Relay1 = 0;
};
#endif // RESPONSE_H
