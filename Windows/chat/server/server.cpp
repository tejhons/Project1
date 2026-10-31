#include <winsock2.h>
#include <ws2tcpip.h>

#include <atomic>
#include <iostream>
#include <string>
#include <thread>

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

/* Find IP address used on local network */
std::string getIP(){
  
  std::string ip{ "127.0.0.1" };
  SOCKET s{ socket(AF_INET, SOCK_DGRAM, 0) };
  sockaddr_in dns{};

  dns.sin_family = AF_INET;
  dns.sin_port = htons(80);
  inet_pton(AF_INET, "8.8.8.8", &dns.sin_addr);

  if (connect(s, (sockaddr *)&dns, sizeof(dns)) == 0) {
    sockaddr_in me{};
    int len{ sizeof(me) };
    getsockname(s, (sockaddr *)&me, &len);
    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &me.sin_addr, buf, sizeof(buf));
    ip = buf;
  }
  closesocket(s);
  return ip;
}

/*--------------------------MAIN-------------------------*/
int main()
{
  WSADATA wsa{};
  if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
    std::cerr << "WSAStartup Failed" << '\n';
    return 1;
  }

  SOCKET sock{ socket(AF_INET, SOCK_DGRAM, 0) };  //UDP Socket
  
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = 0;  //port zero means OS will choose a free port
  if (bind(sock, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
    std::cerr << "bind failed: " << WSAGetLastError() << '\n';
    return 1;
  }

  int len{ sizeof(addr) };
  getsockname(sock, (sockaddr *)&addr, &len);
  
  std::cout << "Server running at IP " << getIP() << ", port" << ntohs(addr.sin_port) << '\n';
  std::cout << "Waiting for the client to send the first message..." << '\n';

  sockaddr_in clientAddr{}; //learned from client's first message
  std::atomic<bool> haveClient{ false }; 

  std::thread receiver([&]() {
    char buf[1024];
    while (true) {
      sockaddr_in from{};
      int flen{ sizeof(from) };
      int n{ recvfrom(sock, buf, sizeof(buf) - 1, 0, (sockaddr *)&from, &flen) };
      if (n < 0) {continue;}
      buf[n] = '\0';
      clientAddr = from;
      haveClient = true;
      std::cout << "[client]" << buf << '\n';
    }
  });
  receiver.detach();

  std::string msg{};
  while (std::getline(std::cin, msg)) {
    if (!haveClient) {
      std::cout << "(no client has connected yet)" << '\n';
      continue;
    }

    sendto(sock, msg.c_str(), (int)msg.size(), 0, (sockaddr *)&clientAddr, sizeof(clientAddr));
  }
  closesocket(sock);
  WSACleanup();
  return 0;
}
