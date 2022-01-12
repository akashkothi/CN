#include "../cn.h"

int count = 0;

void handler(int sig) {
	count++;
    cout<<"Ctrl-C pressed ..."<<endl;
	if(count == 5)
		signal(SIGINT,SIG_DFL);
}

int main(){
	signal(SIGINT,handler);
	while(1);
}