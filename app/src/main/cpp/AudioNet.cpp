#include "AudioNet.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include  <mutex>

#if defined(__ANDROID__) && !defined(NDEBUG)

#include <android/log.h>

#define TAG "AudioNet"


#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    TAG, __VA_ARGS__)

#else

#define LOGE(...) printf(__VA_ARGS__)
#define LOGI(...) printf(__VA_ARGS__)
#define LOGW(...) printf(__VA_ARGS__)
#define LOGD(...) printf(__VA_ARGS__)

#endif


CAudioNet::CAudioNet(std::string host, int port, bool auto_reconnect, int auto_reconnect_tries)
{
    _host = host;
    _port = port;
    _auto_reconnect = auto_reconnect;
    _auto_reconnect_tries = auto_reconnect_tries;
    _buffer = new std::vector<char>;
    _buffer_size = 0;
}

CAudioNet::~CAudioNet()
{
    _running = false;
    if(_thread.joinable())
        _thread.join();

    if(_buffer)
        delete _buffer;
}

int CAudioNet::Connect()
{
    _tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_tcp_socket < 0)
    {
        LOGE("ERROR CREATING SOCKET");
        return -1;
    }

    int flag = 1;
    int result = setsockopt(_tcp_socket, IPPROTO_TCP, 1 /* TCP NO DELAY */, (char *) &flag, sizeof(int));

    struct sockaddr_in server_tcp_addr;
    server_tcp_addr.sin_family = AF_INET;
    server_tcp_addr.sin_port = htons(_port);
    server_tcp_addr.sin_addr.s_addr = inet_addr(_host.c_str());
    socklen_t slen = sizeof(server_tcp_addr);
    if(connect(_tcp_socket,(struct sockaddr*)&server_tcp_addr, slen) < 0)
    {
        close(_tcp_socket);
        LOGE("ERROR CONNECTING TO SERVER");
        return -2;
    }
    return 0;
}

int CAudioNet::Start()
{
    _thread = std::thread(&CAudioNet::Thread, this);
    return 0;
}

void CAudioNet::Thread()
{
    if(_auto_reconnect)
    {
        for(int retries = 0; retries < _auto_reconnect_tries && _running; ++retries)
        {
            Read();
            int is_connected = -1;
            while((is_connected = Connect()) != 0 && retries < _auto_reconnect_tries && _running)
            {
                ++retries;
                LOGD("Connection lost retry count: %d", retries);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            if(is_connected && _running)
            {
                retries = 0;
                LOGD("Connection resumed!");
            }
        }
    }
    else
    {
        Read();
    }
}
void CAudioNet::Read()
{
    unsigned int buffer_size = 1024;
    ssize_t recv_len = 0;
    std::vector<char> recv_buff;
    recv_buff.resize(buffer_size);
    while((recv_len = recv(_tcp_socket, &recv_buff[0], buffer_size, 0)) > 0 && _running)
    {
        //no point in writing/locking if we'll clear it anyway
        if(_buffer_size > 1024 * 24)
        {
            continue;
        }
        std::lock_guard<std::mutex> lock(g_mutex);
        _buffer->insert(_buffer->end(), std::make_move_iterator(recv_buff.begin()), std::make_move_iterator(recv_buff.begin() + recv_len));
        _buffer_size += recv_len;
    }
}


std::vector<char>* CAudioNet::GetBuffer()
{
    return _buffer;
}

void CAudioNet::Render(int count, char* out_buffer)
{
    static int call_count = 0;
    int copy_out = count;

    if(copy_out > _buffer_size)
    {
        // current buffer is smaller than what audio device is asking for
        LOGI("_buffer_size: %d, copy_out: %d", _buffer_size.load(), copy_out);
        copy_out = _buffer_size;
    }

    // locking is not needed during read,
    // we can allow the buffer to populate as we're reading data from it
    // since read is thread safe (and we only ever erase it this thread)
    for(int i = 0; i < copy_out; ++i)
    {
        out_buffer[i] = std::move((*_buffer)[i]);
    }

    // if buffer size is small, we silence the remaining bits
    for(int i = copy_out; i < count; ++i)
    {
        out_buffer[i] = 0;
    }

    // if the buffer is getting too large ~ 240ms delay, we'll clear it to catch up
    std::unique_lock<std::mutex> lock(g_mutex);
    if(_buffer_size - copy_out > 1024 * 24)
    {
        _buffer->clear();
        _buffer->reserve(1024 * 24);
        _buffer_size = 0;
        LOGI("CLEAR");
    }
    else
    {
        _buffer->erase(_buffer->begin(),_buffer->begin() + copy_out);
        _buffer_size -= copy_out;
    }
    // Manually unlocking, to prevent further delay as LOGI itself is slow
    lock.unlock();

    call_count += 1;
    if(call_count % 200 == 0)
        LOGI("_buffer_size: %d", _buffer_size.load());


}

void CAudioNet::Close()
{
    if(_tcp_socket > 0)
        close(_tcp_socket);
}
