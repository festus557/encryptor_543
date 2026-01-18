#include <iostream>
#include <cstdlib>
#include <sodium.h>
#include <vector>

using namespace std;

vector<unsigned char>  key_encrypt(const unsigned char publickey[crypto_box_PUBLICKEYBYTES]){

       vector<unsigned char> encrypted_key(
               crypto_box_SEALBYTES + crypto_secretbox_KEYBYTES
       );

       unsigned char key[crypto_secretbox_KEYBYTES];
       randombytes_buf(key,sizeof(key));

       int ret = crypto_box_seal(
              encrypted_key.data(),
              key,
              sizeof(key),
              publickey
      );

      if(ret != 0){
           vector<unsigned char> err;
           err.push_back("Failed");
           return err;
      }

      sodium_memzero(key,sizeof(key));

      return encrypted_key;

}
