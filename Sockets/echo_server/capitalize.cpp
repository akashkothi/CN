#include "../cn.h"

void capitalize(string &s){
    if(s.size() == 0) return;
    for(int i = 0; i < s.length(); i++)
        s[i] = toupper(s[i]);
}

int main() {

    string buff;

    getline(cin,buff);
    capitalize(buff);
    cout<<buff<<endl;
    
    exit(EXIT_SUCCESS);
}