#include <fcntl.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include <vector>

using namespace std;

const char * cert = "/data/data/com.termux/files/home/.keys/server.crt";
const char * privatekey = "/data/data/com.termux/files/home/.keys/server.key";

int server(){

        int serverfd,clientfd;
        SSL_CTX * ctx;
        SSL * ssl;

        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();

        ctx = SSL_CTX_new(TLS_server_method());
        if(SSL_CTX_use_certificate_file(ctx,cert,SSL_FILETYPE_PEM) <= 0){
              perror("cert: ");
              return -1;
        }

        if(SSL_CTX_use_PrivateKey_file(ctx,privatekey,SSL_FILETYPE_PEM) <= 0){
              perror("private: ");
              return -1;
        }

        
        struct sockaddr_in addr;
        unsigned short port = 50000;

        serverfd = socket(AF_INET,SOCK_STREAM , 0);

        if(serverfd < 0){
               cout<<"[+]Socket Creation Failed"<<endl;
               perror("sock: ");
               return -1;
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        if( (::bind(serverfd,(struct sockaddr*)&addr,sizeof(addr))) < 0){
               cout<<"[+]Binding Failed"<<endl;
               perror("bind: ");
               return -1;
        }

        listen(serverfd,5);

        cout<<"Lustening on: 0.0.0.0:50000"<<endl;

        while(1){
               clientfd = accept(serverfd,NULL,NULL);

               if(clientfd < 0){
                      cout<<"[+]Accepting Failed"<<endl;
                      perror("accept: ");
                      continue;
               }
               cout<<"Client connected"<<endl;

               //ctx = SSL_CTX_new(TLS_server_method());
               ssl = SSL_new(ctx);

               SSL_set_fd(ssl,clientfd);

               if(SSL_accept(ssl) <= 0){
                     ERR_print_errors_fp(stderr);
                     close(clientfd);
                     SSL_free(ssl);
                     return -1;
               }

               fd_set readfd;
               fd_set writefd;

               int fd = SSL_get_fd(ssl);

               int flag = fcntl(fd,F_GETFL,0);
               fcntl(fd,F_SETFL, flag | O_NONBLOCK);
               char buffer[1024];
               vector<char > full_request;
               char response[4096*2];

               while(1){
                     memset(buffer,0,sizeof(buffer));
                     memset(response,0,sizeof(response));
                     full_request.clear();

                     FD_ZERO(&readfd);
                     FD_ZERO(&writefd);

                     FD_SET(fd,&readfd);
                     FD_SET(fd,&writefd);

                     select(fd + 1,&readfd,&writefd,NULL,NULL);

                     if(FD_ISSET(fd,&readfd)){
                          int ret;
                          while((ret = SSL_read(ssl,buffer,sizeof(buffer))) > 0){
                                 full_request.insert(full_request.end(),(char * )buffer,buffer + ret);
                          }

                          cout<<"[+]REQ:\n"<<full_request.data()<<endl;
                          //handle much
                     }

                     if(FD_ISSET(fd,&writefd)){
                          int ret = SSL_write(ssl,response,sizeof(response));
                          if(ret > 0){
                             cout<<"RESP:\n"<<response<<endl;
                             //handling response
                          }
                          else{
                              int err = SSL_get_error(ssl,ret);
                              if(err == SSL_ERROR_ZERO_RETURN){
                                    cout<<"Connection closed"<<endl;
                                    perror("conn: ");
                                    close(clientfd);
                                    break;
                               }
                          }
                     }
               }
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(clientfd);
}


int main(){
   server();
}
