#include "sylar/sylar.h"
#include "sylar/iomanager.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();


int sock = 0;

void test_fiber() {
    SYLAR_LOG_INFO(g_logger) << "test_fiber sock=" << sock;

    //sleep(3);

    //close(sock);
    //sylar::IOManager::GetThis()->cancelAll(sock);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "10.0.2.83", &addr.sin_addr.s_addr);

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if(errno == EINPROGRESS) {
        SYLAR_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::READ, []() {
            SYLAR_LOG_INFO(g_logger) << "read callback";
            });
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::WRITE, [](){
            SYLAR_LOG_INFO(g_logger) << "write callback";
            //close(sock);
            sylar::IOManager::GetThis()->cancelEvent(sock, sylar::IOManager::READ);
            close(sock);
        });
    } else {
        SYLAR_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }

}

void test1() {
    std::cout << "EPOLLIN=" << EPOLLIN
              << " EPOLLOUT=" << EPOLLOUT << std::endl;
    sylar::IOManager iom(2, false);
    //sylar::IOManager iom;
    iom.schedule(&test_fiber);
}

sylar::Timer::ptr s_timer;
void test_timer() {
    sylar::IOManager iom(1);
    s_timer = iom.addTimer(1000, [](){
        static int i = 0;
        SYLAR_LOG_INFO(g_logger) << "hello timer i=" << i;
        if(++i == 3) {
            s_timer->reset(2000, true);
            //s_timer->cancel();
        }
    }, true);
}
extern "C" {

typedef unsigned int (* sleepfun)(unsigned int seconds);

}
unsigned int call (unsigned int seconds){
    std::cout << "call"<<std::endl;
    return 0;
}




int main(int argc, char** argv) {
    //test1();
<<<<<<< HEAD
    //test_timer();
    sleepfun s_call = call;
    s_call(1);
    std::cout << "main hello world"<<std::endl;
=======
    test_timer();
>>>>>>> f4ffd8f6a9edf51febcc35fadf3649a9c9b5bda3
    return 0;
}













//
//int sock = 0;
//
//void test_fiber() {
//    SYLAR_LOG_INFO(g_logger) << "test_fiber sock=" << sock;
//
//    //sleep(3);
//
//    //close(sock);
//    //sylar::IOManager::GetThis()->cancelAll(sock);
//
//    sock = socket(AF_INET, SOCK_STREAM, 0);
//    fcntl(sock, F_SETFL, O_NONBLOCK);
//
//    sockaddr_in addr;
//    memset(&addr, 0, sizeof(addr));
//    addr.sin_family = AF_INET;
//    addr.sin_port = htons(80);
//    inet_pton(AF_INET, "115.239.210.27", &addr.sin_addr.s_addr);
//
//    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
//    } else if(errno == EINPROGRESS) {
//        SYLAR_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);
//        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::READ, [](){
//            SYLAR_LOG_INFO(g_logger) << "read callback";
//        });
//        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::WRITE, [](){
//            SYLAR_LOG_INFO(g_logger) << "write callback";
//            //close(sock);
//            sylar::IOManager::GetThis()->cancelEvent(sock, sylar::IOManager::READ);
//            close(sock);
//        });
//    } else {
//        SYLAR_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
//    }
//
//}
//
//void test1() {
//    std::cout << "EPOLLIN=" << EPOLLIN
//              << " EPOLLOUT=" << EPOLLOUT << std::endl;
//    sylar::IOManager iom(2, false);
//    iom.schedule(&test_fiber);
//}
//
//sylar::Timer::ptr s_timer;
//void test_timer() {
//    sylar::IOManager iom(2);
//    s_timer = iom.addTimer(1000, [](){
//        static int i = 0;
//        SYLAR_LOG_INFO(g_logger) << "hello timer i=" << i;
//        if(++i == 3) {
//            s_timer->reset(2000, true);
//            //s_timer->cancel();
//        }
//    }, true);
//}
//
//int main(int argc, char** argv) {
//    //test1();
//    test_timer();
//    return 0;
//}

//void test_fiber() {
//    SYLAR_LOG_INFO(g_logger) << "test_fiber" ;
//}
//
//void test1() {
//    sylar::IOManager iom;
//    iom.schedule(&test_fiber);
//
//}
//int main(int argc, char** argv) {
//    test1();
//    return 0;
//}