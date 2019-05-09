#include<iostream>

using namespace std;

int main()
{

  int itermax=1000;
  int iter;
  int interval = 100;
  int iterout;

  iterout =0;
  iter = 0;
  int dummy;
  do{

    for(iter=0; iter<interval; ++iter){
      // cout<<"Iteration : "<<iter<<endl;
      dummy = iter;
     }
    cout<<"Dummy: "<<dummy<<endl;
    iterout = iterout + interval;   
    cout<<"Iterout : "<<iterout<<endl;
    
  }while(iterout<itermax);
  
}
