#include "../cn.h"

void capitalize(string &s){
    if(s.size() == 0) return;
    for(int i = 0; i < s.length(); i++)
        s[i] = tolower(s[i]);
}

int main() {

    while(1) {
        string buff;
        getline(cin,buff);
        capitalize(buff);
        cout<<buff<<endl;
    }
    
    exit(EXIT_SUCCESS);
}