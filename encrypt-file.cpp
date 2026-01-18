#include <iostream>
#include <fcntl.h>
#include <sodium.h>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <vector>

using namespace std;

bool encrypted_file(const string& infile , const unsigned char key[crypto_secretbox_KEYBYTES]){

      unsigned char nonce[crypto_secretbox_NONCEBYTES];

      int fd = open(infile.c_str(),O_RDONLY);

      if(fd < 0){
          return false;
      }

      randombytes_buf(nonce, sizeof(nonce));
      string outfile = infile + ".enc";
      char buf[128];
      size_t bytes;
      vector<unsigned char> filecontent;

      while( (bytes = read(fd,buf,sizeof(buf))) > 0){
           filecontent.insert(filecontent.end(),buf,buf + bytes);
      }

      if(byted < 0) return false;

      close(fd);
      size_t filesize = filecontent.size();
      size_t CIPHERTEXT_SIZE = filesize + crypto_secretbox_MACBYTES;
      vector<unsigned char> ciphertext(CIPHERTEXT_SIZE);

      int ret = crypto_secretbox_easy(
              ciphertext.data(),
              filecontent.data(),
              filesize,
              nonce,
              &key
      );

      if(ret < 0){
         return false;
      }

      sodium_memzore(filecontent.data,filecontent.size());
      int out = open(outfile.c_str(),O_WRONLY | O_CREAT);

      if(out < 0){
          return false;
      }

      write(out,(char *)nonce,sizeof(nonce));
      write(out,(char *)ciphertext.data(),ciphertext.size());

      close(out);
      return true;

}
