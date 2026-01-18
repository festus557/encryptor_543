#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>

using namespace std;

vector<string> filesystem(string dirname){

      vector<string> tree;
      DIR * dir;
      struct dirent * fd;
      struct stat st;

      if((dir = opendir(dirname.c_str())) == NULL)
              return tree;

      while((fd = readdir(dir)) != NULL){

              if(strcmp(fd->d_name,".") == 0 || strcmp(fd->d_name,"..") == 0)

                        continue;

              string fullpath = dirname + "/" + fd->d_name;

              if(stat(fullpath.c_str(),&st) == 0){
                        if(S_ISDIR(st.st_mode)){

                             vector<string> subtree = filesystem(fullpath);
                             tree.insert(tree.end(),subtree.begin(),subtree.end());

                         }
                         else{
                             tree.push_back(fullpath);
                         }
              }
      }

      closedir(dir);
      return tree;

}


int main(int argc , char * argv[]){

   if(argc < 2){
      cout<<"[+]Please insert start directory"<<endl;
      return -1;
   }
   const char * startdir = argv[1];
   vector<string>  tree = filesystem(startdir);
   for(auto & file : tree)
      printf("%s\n",file.c_str());
   return 0;
}
