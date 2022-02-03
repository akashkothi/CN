#include <iostream>
using namespace std;

int main() {

    string buff;
    getline(cin,buff);
    
    for(char &c : buff)
        c = toupper(c);
    
    cout<<buff<<endl;

}