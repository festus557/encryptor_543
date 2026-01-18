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

int client( const char * msg){

       SSL_CTX * ctx;
       SSL * ssl;
       int serverfd;

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

       int retries = 0;
       time_t sec = 10;
       int max_retries = 20;
       int high = 0;

       while(connect(serverfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) != 0){
                if(retries < max_retries){
                    sleep(sec);
                    sec += 10;
                    retries++;
                    continue;
                }
                sleep(900);
                high++;

                if(high == max_retries)
                    break;

                 continue;
       }

       ctx = SSL_CTX_new(TLS_client_method());
       ssl = SSL_new(ctx);

       SSL_set_fd(ssl,serverfd);

       if(SSL_connect(ssl) <= 0){
                goto cleanup;
       }
       else {

                if(SSL_write(ssl,msg,strlen(msg)) <= 0){
                      ERR_print_errors_fp(stderr);
                      goto cleanup;
                   }
       }
      cleanup:
        SSL_shutdown(ssl);
        SSL_CTX_free(ctx);
        SSL_free(ssl);
        close(serverfd);
        return 0;

}


int main(){
  return  client("Secrete message");
}
