#include "../cn.h"

void de_capitalize(string &s){
    if(s.size() == 0) return;
    for(int i = 0; i < s.length(); i++)
        s[i] = tolower(s[i]);
}

int main() {

    string buff;

    getline(cin,buff);
    de_capitalize(buff);
    cout<<buff<<endl;
    
    exit(EXIT_SUCCESS);
    
}