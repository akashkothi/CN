#include "../cn.h"
int arr1[BUFFSIZE], arr2[BUFFSIZE];
char buff[1];

void helper(int fd, int* arr, int &size, int n = 0) {
    
    while(read(fd,buff,1) > 0) {
        if((*buff == ' '||*buff == '\n') && n > 0)
            arr[size++] = n, n = 0;
        else n = n*10 + *buff-48;
    }

    if(n > 0) arr[size++] = n;
    
}

void merge(int fd, int size1, int size2) {
    int i = 0, j = 0;
    while (i < size1 && j < size2) {
        if (arr1[i] < arr2[j]) { 
            string s = to_string(arr1[i++]);
            char const *num = s.c_str();
            write(fd,num,strlen(num));
        }
        else{
            string s = to_string(arr2[j++]);
            char const *num = s.c_str();
            write(fd,num,strlen(num));
        }
        write(fd," ",1);
    }
 
    while (i < size1) {
        string s = to_string(arr1[i++]);
        char const *num = s.c_str();
        write(fd,num,strlen(num));
        write(fd," ",1);
    }
    while (j < size2) {
        string s = to_string(arr2[j++]);
        char const *num = s.c_str();
        write(fd,num,strlen(num));
        write(fd," ",1);
    }
    return;
}

int main() {
    int fd1, fd2, fd3, size1 = 0, size2 = 0;
    
    if((fd1 = open("./merge_1.txt",R)) == -1)
        perror("open error");

    if((fd2 = open("./merge_2.txt",R)) == -1)
        perror("open error");

    if((fd3 = open("./merge_result.txt",W)) == -1)
        perror("write error");

    helper(fd1,arr1,size1);
    helper(fd2,arr2,size2);

    merge(fd3,size1,size2);
}