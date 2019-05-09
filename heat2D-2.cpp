#include<iostream>
#include<fstream>
#include<cmath>
#include<chrono>

#include<algorithm>

#include<omp.h>


//#define pi 4.0*atan(1.0)
#define pi 3.14159265

using namespace std;
using namespace std::chrono;

void init(int imax, int jmax, float dx, float dy,
	  float *a, float *b)
{
  int i,j;
#pragma omp parallel shared(a,b) private(i,j)
  {
#pragma omp for nowait
    for(i=0; i<imax; ++i){
    for(j=0; j<jmax; ++j){
      int index = i*jmax + j;
      a[index] = sin(i*pi*dx) * sin(j*pi*dy); //clang++ cannot offload math
      b[index] = 0.0;
    }
  }
  } //parallel region CPU

}


void prep(int &imax, int &jmax, int &itermax, int &interval,
	  float &dx, float &dy, float &dt,
	  float &kappa)
{
  cout<<"Enter imax, jmax: \n";
  cin>>imax;
  cin>>jmax;
  cout<<"Enter maximum iteration: \n";
  cin>>itermax;
  cout<<"Enter interval output: \n";
  cin>>interval;

  /*Define dx,dy,dt*/
  dx = 1.0 /(float)(imax-1);
  dy = 1.0 /(float)(jmax-1);
  dt = dx*dx*0.1;
  kappa = dt/(dx*dx);
}

void outputfile(string filename, int imax, int jmax, float dx,
		float dy, float *array)
{

  ofstream myfile;
  myfile.open(filename);
  myfile <<"x, y, z, Temp \n";

  int index;
  float xg,yg;
  
  for(int i=0; i<imax; ++i){
    for(int j=0; j<jmax; ++j){
      index = i*jmax + j;
      xg = i*dx;
      yg = j*dy;
      myfile<< xg<<", "<<yg<<", "
	    <<array[index]<<", "
	    <<array[index]<<"\n";
    }
  }
  
  myfile.close();
  
}

void solve(int imax, int jmax, int itermax,
	   int interval, float kappa,
	   float* T, float* Tnew)
{

  int iter =0;
  int ij;
  int i1,i2,i3,i4;

  float* tmp;

  int iterout = 0;
  /*Time loop*/
  do{

    for(iter=0; iter<interval; ++iter){
      
    #pragma omp target teams distribute parallel for simd collapse(2)
    for(int i=1; i<imax-1; ++i){
      for(int j=1; j<jmax-1; ++j){
	ij = i*jmax+j;
	i1 = ij + jmax; //(i+1,j)
	i2 = ij - jmax; //(i-1,j)
	i3 = ij + 1; //(i,j+1)
	i4 = ij - 1; //(i,j-1)
	Tnew[ij] = T[ij] + kappa*( T[i2]-2.0*T[ij]+T[i1]
				  +T[i4]-2.0*T[ij]+T[i3]);
      }
    }
    
    swap(T,Tnew);
    
    } //run loop within interval
    iterout = iterout + interval;
    cout<<"Iteration : "<<iterout<<endl;
    
  }while(iterout<itermax);
  
}

int main()
{
  int imax, jmax;
  int itermax, interval;
  float dx, dy, dt, kappa;
  float *T;
  float *Tnew;
  string myfile ="heat-gpu-openmp.csv";

  prep(imax, jmax, itermax, interval, dx, dy, dt, kappa);

  int nodes = imax*jmax;
  T = new float[nodes];
  Tnew = new float[nodes];
    
  init(imax, jmax, dx, dy, T, Tnew );
  
  high_resolution_clock:: time_point t1 = high_resolution_clock::now();

#pragma omp target enter data map(to:T[0:nodes], Tnew[0:nodes])

  solve(imax,jmax,itermax,
	interval, kappa,
	T, Tnew);

#pragma omp target exit data map(from:T[0:nodes], Tnew[0:nodes])

  high_resolution_clock:: time_point t2 = high_resolution_clock::now();

  duration<double> time_span = duration_cast<duration<double>>(t2-t1);
  double mytimeis=time_span.count();
  cout<<"Elapsed time: "<<mytimeis<<" secs"<<endl;
  
  outputfile(myfile, imax,jmax,dx,dy, T);

  
  
}
