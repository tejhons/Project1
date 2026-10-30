#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <string>
#include <thread>

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

int main()
{
  WSADATA wsa{};
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    std::cerr << "WSAStartup Failed" << '\n';
    return 1;
  }

  std::string ip{};
  int port{};

  std::cout << "Server IP: ";
  std::getline(std::cin, ip);
  std::cout << "Server Port: ";
  std::cin >> port;
  std::cin.ignore(); //newline leftovers
  
  sockaddr_in server{};
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  if (inet_pton(AF_INET, ip.c_str(), &server.sin_addr) != 1) {
    std::cerr << "Invalid IP address" << '\n';
    return 1;
  }

  SOCKET sock{ socket(AF_INET, SOCK_DGRAM, 0) };  //UDP Socket
  
  std::thread receiver([&](){
    char buff[1024];
    while (true) {
      int n = recvfrom(sock, buff, sizeof(buff) - 1, 0, nullptr, nullptr);
      
      if (n < 0) {continue;}
      buff[n] = '\0';
      std::cout << "[server]" << buff << '\n';
    }

  });
  receiver.detach();

  std::string hello{ "(client joined)" };
  sendto(sock, hello.c_str(), (int)hello.size(), 0, (sockaddr *)&server, sizeof(server));
  std::cout << "Connected. Type a message and press enter." << '\n';

  std::string msg{};
  while (std::getline(std::cin, msg)) {
    sendto(sock, msg.c_str(), (int)msg.size(), 0, (sockaddr *)&server, sizeof(server));
  }
  closesocket(sock);
  WSACleanup();
  return 0;
}
