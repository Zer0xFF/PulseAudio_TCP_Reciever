#pragma once

#include <vector>
#include <string>
#include <thread>
#include <deque>
#include "circular_buffer.h"

class CAudioNet
{
public:
    CAudioNet(std::string host, int port, bool auto_reconnect, int auto_reconnect_tries);
    ~CAudioNet();

    int Connect();
    int Start();
    void Render(int count, char*);
    void Close();


private:
    void Thread();
    void Read();


    std::mutex g_mutex;

    std::string _host;
    int _port;
    bool _auto_reconnect;
    int _auto_reconnect_tries;
    circular_buffer<char>* _buffer;

    int _tcp_socket = -1;
    std::thread _thread;
    bool _running = true;

};
