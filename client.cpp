#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int client(){

       SSL_CTX * ctx;
       SSL * ssl;
       int serverfd;
       char buf[4096];
       char buffer[4096];

       string serverip = "127.0.0.1";
       unsigned short port = 50000;

       SSL_library_init();
       SSL_load_error_strings();

       struct sockaddr_in serveraddr;

       serverfd = socket(AF_INET,SOCK_STREAM,0);

       if(serverfd < 0){
                return -1;
       }

       serveraddr.sin_family = AF_INET;
       serveraddr.sin_addr.s_addr = inet_addr(serverip.c_str());
       serveraddr.sin_port = htons(port);

       while(connect(serverfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) != 0){
                sleep(10);
       }

       ctx = SSL_CTX_new(TLS_client_method());
       ssl = SSL_new(ctx);

       SSL_set_fd(ssl,serverfd);

       if(SSL_connect(ssl) <= 0){
                return -1;
       }
       else {

                int fd = SSL_get_fd(ssl);
                int flag = fcntl(fd,F_GETFL,0);
                fcntl(fd,F_SETFL, flag | O_NONBLOCK);

                fd_set reads , writes;

                while(1){

                       memset(buffer,0,sizeof(buffer));
                       memset(buf,0,sizeof(buf));

                       FD_ZERO(&reads);
                       FD_ZERO(&writes);

                       FD_SET(fd,&reads);
                       FD_SET(fd,&writes);

                       if(select(fd + 1,&reads,&writes,NULL,NULL) < 0){
                               return -1;
                       }
                       else {
                              if(FD_ISSET(fd,&reads)){
                                  int ret = SSL_read(ssl,buffer,sizeof(buffer));
                                  if(ret > 0){
                                              buffer[ret] = '\0';
                                              //handle response
                                  }
                                  else {
                                         int err = SSL_get_error(ssl,ret);
                                         if(err == SSL_ERROR_ZERO_RETURN)
                                                  break;

                                  }
                              }

                              if(FD_ISSET(fd,&writes)){
                                  int ret = SSL_write(ssl,buf,sizeof(buf));
                                  if(ret < 0){
                                        int err = SSL_get_error(ssl,ret);
                                        if(err == SSL_ERROR_ZERO_RETURN)
                                              break;
                                  }
                                  //handle requests
                              }
                       }

                }
       }

return 0;

}
