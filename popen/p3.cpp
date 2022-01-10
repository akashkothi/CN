#include "../cn.h"

int main() {

    string buff;

    while(1) {
        getline(cin, buff);
        cout<<"\n-------------- In process P3 -----------------\n\n";
        cout<<"Message received from P2 : "<<buff<<endl;
        cout<<endl;
    }

}