
// Turbo Codec v.12.06.04
// @ACDISa
// @RSE

//!!!Memory failed if v > 3 and M > 4

// Standart C++
#include <iostream>
#include <cmath>
#include <stdio.h>
#include <time.h>
using namespace std;
#define log_2          log(2.0);
#define MIN            1e-200;

double Size_of_Block = 512*512;//262144;


double *CodeSeq = new double[3*256*256+1]; //sequence that being given on decoders
double *z = new double[256*256+1];// extrinsic information
	
double *NonUnIntCodeSeq = new double[256*256+1]; //code after interliver
double *NonUnIntdecode = new double[256*256+1]; //soft after interliver
double *NonUnIntz = new double[256*256+1]; //extrinsic information before deinterliver

double (*Gamma0Y)[32] = new double[256*256+1][32];
double (*Gamma1Y)[32] = new double[256*256+1][32];

double (*Gamma0Rk)[32] = new double[256*256+1][32];
double (*Gamma1Rk)[32] = new double[256*256+1][32];

double (*Alpha)[32] = new double[256*256+1][32];

double (*Betta)[32] = new double[256*256+1][32];

double (*Sigma0)[32] = new double[256*256+1][32];
double (*Sigma1)[32] = new double[256*256+1][32];



int *dataPerr = new int[300*300]; // Data before encoder ( for error probability calculating )
int KolIter;
double sigma( double*, int );
double sig_L1;

double limit = 1;
double count_mul = 0;
double count_sum = 0;
double count_all_sum = 0;
double count_all_mul = 0;

typedef struct TRELLIS
{
   int (*Next)[2];
   int (*Prev)[2];
   int (*Out)[2];

}trellis;


	
// Generator of random numbers with normal distribution.
// Likewise randn() in Matlab.
double randn( double , double ); 

void turbocoder( double*, double*, double*, int, double*, double* );
void turbo_decoder( double*, double*, double*, int, double*, double*, double, double* );
void Gen_CodeSeq1(double*,double*,double*,double*,int);
void Gen_CodeSeq2(double*,double*,double*,int);
void Gen_deCodeSeq(double*, double*,double*, double (*Sigma0)[32], double (*Sigma1)[32],int, int, double, double);
void Gen_deCodeSeq_on2decoder(double*, double*, double*, double (*Sigma0)[32], double (*Sigma1)[32],int, int,double);
void VichGamma( double (*Gamma0)[32], double (*Gamma1)[32], double*, int, trellis , int, int, int, double, double);
void VichBetta( double (*Alpha)[32], double (*Betta)[32], double (*Gamma0)[32],double (*Gamma1)[32], int, int,trellis);
void VichAlpha( double (*Alpha)[32], double (*Gamma0)[32],double (*Gamma1)[32], int, int, trellis);
void VichSigma( double (*Sigma0)[32], double (*Sigma1)[32], double (*Alpha)[32], double (*Betta)[32], double (*Gamma0)[32], double (*Gamma1)[32], int , int, trellis); 
int GetV( double* ); // Get Constraint Length
trellis GetTrellis( double* );
void RSCEncoder( trellis, double*, int, double* );
void Transposition( int, int* ); // Get Transposition for Interleaving
void NonUnInt( double*, int, int*, double* ); // Non Uniform Interleaving
void DeNonUnInt( double*, int, int*, double* ); // Non Uniform Deinterleaving
int SetBit( int, int, int );
int HemWeight( int number ); 
void output(double*, int );
void toGauss(int num, double[3]);
int *DelRepeatElems(int *, int, int &);
void Sort(int *, int );


void main()
{
  int M; // // Max:128 M-by-M = size of interleaving matrix and data size
  double *code1; // Control bits from elementary encoder 1
  double *code2; // Control bits from elementary encoder 2
  double *data; // Data before encoder and data after channel
  int    *code1Perr;
  int    *code2Perr;
  double Perr; // Error probability
  int Nerr = 0; // Number of Error
  double *soft; // Soft Decisions
  
 
  cout<<"\n_____TURBO CODEC___________________________________________v.2.01.05\n";
  cout<<"\n @Nickolaev\n @Rastaturin\n  gr.3908\n\n";

  cout<<"Input M (4,8,16,32,64,128): ";
  cin>>M;
  cout<<"\nLength of data: "<<M*M<<"\n\n";

  cout<<"Kolter = ";
  cin>>KolIter;
  cout<<"\n";

  code1 = new double[M*M+1];
  code2 = new double[M*M+1];
  code1Perr = new int[M*M+1];
  code2Perr = new int[M*M+1];
  data = new double[M*M+1];
  soft = new double[M*M+1];

  // Get Data
  srand( (unsigned)time( NULL ) );
  for( int i = 0; i < M*M; i ++ )
	  data[i] = rand()%2;

  // Полиномы прямой и обратной связи 1-го элементарного кодера
  int v1 = 4;
  double G1[2] = { 31, 17 }; // 0..2^(v1+1)-1 31 17 13 11
  G1[0] = SetBit( (int)G1[0], v1, 1 ); // Связь с входом кодера
  G1[1] = SetBit( (int)G1[1], v1, 1 ); //  обязательна

  // Полиномы прямой и обратной связи 2-го элементарного кодера
  int v2 = 4;
  double G2[2] = { 31, 17 }; // 0..2^(v1+1)-1
  G2[0] = SetBit( (int)G2[0], v2, 1 ); // Связь с входом кодера
  G2[1] = SetBit( (int)G2[1], v2, 1 ); //  обязательна
 
  turbocoder( G1, G2, data, M, code1, code2 );
  for (i = 0; i < M*M; i ++ )
  {
	  dataPerr[i] = int(data[i]);
	  code1Perr[i] = int(code1[i]);
	  code2Perr[i] = int(code2[i]);
  }
 
  //---------- Gaussian Channel-----------------------------------------

  double  sigma; // sigma^2 = Noise Dispersion
  double  SNRdb; // Signal Noise Ratio in Db
  double SNR; // Signal Noise Ratio in times
  double E; // Signal Energy

  E = 1;
 // cout<< "Input SNRdb: ";
 // cin >> SNRdb;
for (SNRdb = 1; SNRdb <4.5; SNRdb +=0.5)
{
  SNR = pow( 10.0, (SNRdb/10) );
  sigma = sqrt( E/SNR ); 

//  cout<<"------Data without noise-----"<<"\n";
 // output(data,M*M);
 // cout<<"\n";

int count = Size_of_Block/(M*M);
for (int aaa = 0; aaa < count; aaa++)
{
  for (i = 0; i < M*M; i ++ )
  {
	//  if (data[i]< -100000000)
	//		cout<<"POMAGITE!!!"<<i;
	  data[i]=dataPerr[i]*2  -1 + randn( 0, 1 )*sigma;
 	  code1[i]=code1Perr[i]*2-1 + randn( 0, 1 )*sigma;
	  code2[i]=code2Perr[i]*2-1 + randn( 0, 1 )*sigma;
  }

  
 //-------------end of Gaus Chanel---------------------------------------------------

  turbo_decoder( G1, G2, data, M, code1, code2, sigma, soft );

  //---------- Get Error Probability---------------------
  for( i = 0; i < M*M; i++ )  
	{

	   if ( ( dataPerr[i] == 0 ) && (soft[i]>0) )
		   Nerr++;
	   if ( ( dataPerr[i] == 1 ) && (soft[i]<0) )
		   Nerr++;
	}
	  
  
 //--------------------------------------------------------
}//for aaa
  cout<<Nerr<<"\n";
  Perr = (double)Nerr/(Size_of_Block);

  cout<<"SNRdb = "<<SNRdb<<"\n";
  cout<<"Error probability with noise = "<<Perr<<"\n\n";

  cout<<"Count of multi now = "<< count_mul<<"\n" ;
  cout<<"Count of sum now   = "<< count_sum<<"\n" ;
  cout<<"Count of multi was = "<< count_all_mul<<"\n" ;
  cout<<"Count of sum was   = "<< count_all_sum<<"\n" ;
  count_mul = count_sum = count_all_mul = count_all_sum = 0;
  Nerr = 0;
  
}//fo SNRdb
getchar();
getchar();
}

void turbo_decoder( double G1[], double G2[], double data[], 
				   int M, double Y1[], double Y2[], double s, double Soft[] )
{
	//return hard decision 
	//G1 G2 are the two encoder generators expressed in decimal form
	//data[] - array of systematic symbols (length = M*M)
	//Y1[] - array of redudant symbols (generated by first encoder)
	//Y2[] - array of redudant symbols (generated by second encoder)
	// length Y1 Y2 = M*M (if without puncturing)
	// s - variance of transmitted sequence
	// soft - soft decision 

	trellis trl; // trellis :)))
	

		
	int v ; // length constraint max = 5
	int t ; // number of state max = 32
	int Nerr=0;

	double sig_z = 1;

//	double (*Gamma)[32];
//	Gamma = (double(*)[32][32])malloc(sizeof(double)*(M*M+1)*32*32); 
//	Gamma = new double[M*M+1][32]; 
	
	int i,j;

	
//--------------------first part-------------------------------
  
for (i=0; i<=M*M; i++)
		z[i]=0;
		
	

for (int Iter=0; Iter<KolIter; Iter++)
{
   //----initialisation-------------------------------------------
	limit = -1;
	sig_L1=s; 
	if (sig_L1==0) {cout<<"++++++++++++++++++++++++++++++";}
	trl = GetTrellis(G1);
	v = GetV(G1); // length constraint max = 5
	t = pow( 2.0, v ); // number of state max = 32

	for (i=0; i<=M*M; i++)
	{
		for (j=0; j<t; j++)
		{
			Gamma0Y[i][j] = Gamma1Y[i][j] = Gamma0Rk[i][j] = Gamma1Rk[i][j] = 0.0;
			Sigma0[i][j] = Sigma1[i][j] = 0.0;
			Alpha[i][j] = 0.0;
			Betta[i][j] = 0.0;
		}
	}
   //----end of initialisation--------------------------------------
	Gen_CodeSeq1(CodeSeq,data,Y1,z,M);//seqence after first RSK
	if (Iter == 0)
	{
		VichGamma(Gamma0Rk, Gamma1Rk, CodeSeq, M, trl, t, v, 2, s, sig_z);//2 t.k. for Rk = Xk Yk
		count_all_sum = count_all_sum + 8*M*M*t*2;
		count_all_mul = count_all_mul + 13*M*M*t*2;
	}
	else
	{
		VichGamma(Gamma0Rk, Gamma1Rk, CodeSeq, M, trl, t, v, 3, s, sig_z);//3 t.k. for Rk = Xk Yk Zk
		count_all_sum = count_all_sum + 12*M*M*t*2;
		count_all_mul = count_all_mul + 18*M*M*t*2;
	}

	VichGamma(Gamma0Y, Gamma1Y, CodeSeq, M, trl, t, v, 1, s, sig_z);//1 t.k. only for y
	count_all_sum = count_all_sum + 4*M*M*t*2;
	count_all_mul = count_all_mul + 6*M*M*t*2;

	VichAlpha(Alpha, Gamma0Rk, Gamma1Rk, M, t, trl);
	VichBetta(Alpha, Betta, Gamma0Rk, Gamma1Rk, M, t,trl);
	VichSigma(Sigma0, Sigma1, Alpha, Betta, Gamma0Y, Gamma1Y, M, t, trl);
	Gen_deCodeSeq(Soft, data, z, Sigma0, Sigma1, M, t, s, sig_z);


//	cout<<"------Data after first decode-----"<<"\n";	
//	output(deCodeSeq1,M*M);
//	cout<<"\n";
//	sig_L1 = sigma(Soft,M*M);	
	for( i = 0; i < M*M; i++ )  
	{
	   if ( ( dataPerr[i] == 0 ) && (Soft[i]>0) )
		   Nerr++;
	   if ( ( dataPerr[i] == 1 ) && (Soft[i]<0) )
		   Nerr++;

	   if (Soft[i]>10)
			{  Soft[i] = 10;}
		if (Soft[i]< -10)
			{  Soft[i] = -10;}
	   
		
	}
	
	//output(decode,M*M);
//	cout<<Nerr<<" errors last after 1 decoder of turbo\n";
	
	Nerr=0;
//	getchar();

//----------end of first part-------------------------

//--------------------Sekond part-------------------------------
  //-----initialisation-------------------------
	limit=-1;
	sig_L1 = sigma(Soft,M*M);
	trl = GetTrellis(G2);
	v = GetV(G2); // length constraint max = 5
	t = pow( 2.0, v ); // number of state max = 32

	for (i=0; i<=M*M; i++)
		for (j=0; j<t; j++)
			{
				Gamma0Y[i][j] = Gamma1Y[i][j] = Gamma0Rk[i][j] = Gamma1Rk[i][j] = 0.0;
				Sigma0[i][j] = Sigma1[i][j] = 0.0;
				Alpha[i][j] = 0.0;
				Betta[i][j] = 0.0;
			}

	int *tr = new int[M*M]; 
    Transposition( M, tr ); // Get Data for Interleaving
  //-----end of initialisation-------------------------

	NonUnInt( Soft, M, tr, NonUnIntCodeSeq ); // Interleaving

	Gen_CodeSeq2(CodeSeq, NonUnIntCodeSeq, Y2, M);//seqence after second RSK

	VichGamma(Gamma0Rk, Gamma1Rk, CodeSeq, M, trl, t, v, 2, s, sig_z);
	count_all_sum = count_all_sum + 8*M*M*t*2;
	count_all_mul = count_all_mul + 13*M*M*t*2;
	VichGamma(Gamma0Y, Gamma1Y, CodeSeq, M, trl, t, v, 1, s, sig_z);
	count_all_sum = count_all_sum + 4*M*M*t*2;
	count_all_mul = count_all_mul + 6*M*M*t*2;

	VichAlpha(Alpha, Gamma0Rk, Gamma1Rk, M, t, trl);
	VichBetta(Alpha, Betta, Gamma0Rk, Gamma1Rk, M, t,trl);
 	VichSigma(Sigma0, Sigma1, Alpha, Betta, Gamma0Y, Gamma1Y, M, t, trl);
	Gen_deCodeSeq_on2decoder(NonUnIntdecode, NonUnIntz, NonUnIntCodeSeq,  Sigma0, Sigma1, M, t, sig_L1);

	DeNonUnInt( NonUnIntdecode, M, tr, Soft ); // Deinterleaving
	DeNonUnInt( NonUnIntz, M, tr, z ); // Deinterleaving
	delete[] tr;

//	cout<<"------Data after second decode-----"<<"\n";	
//	output(decode,M*M);
//	cout<<"\n";

	sig_z = sigma(z,M*M);

//	for( i = 0; i < M*M; i++ )  
//	{

//	   if ( ( dataPerr[i] == 0 ) && (Soft[i]>0) )
//		   Nerr++;
//	   if ( ( dataPerr[i] == 1 ) && (Soft[i]<0) )
//		   Nerr++;

	   if (z[i]>10)
	     z[i] = 10;
	   if (z[i]< -10)
	    z[i] = -10;
	   
	
//	}


	
//	output(z,M*M);
		
/*	cout<<Nerr<<" errors last after 2 decoder of turbo\n";
	if (Nerr == 0)
		Iter = 15;
	double Perr = (double)Nerr/(M*M);
	cout<<"After "<<(Iter+1)<<" Iteration Perr ="<<Perr<<"\n\n";
	Nerr=0;
	cout<<"Count of multi now = "<< count_mul<<"\n" ;
  cout<<"Count of sum now = "<< count_sum<<"\n" ;
  cout<<"Count of multi was = "<< count_all_mul<<"\n" ;
  cout<<"Count of sum was = "<< count_all_sum<<"\n" ;
	cout<<"Press any key... \n\n";
	getchar();*/

}//Konec Iteraciam!!!!!

//	delete[] CodeSeq;
//	delete[] z;
//	delete[] NonUnIntCodeSeq; 
//	delete[] NonUnIntdecode; 
//	delete[] NonUnIntdecode;

/*	delete[] Gamma0Y;
	delete[] Gamma1Y;
	delete[] Gamma0Rk;
	delete[] Gamma1Rk;

	delete[] Alpha;
	delete[] Betta;

	delete[] Sigma0;
	delete[] Sigma1;*/

	
//----------end of second part-------------------------	

	

}

void output(double string[], int length)
{
	int i=0;
	while (i<length)
	{
		cout<<' ';
		cout<<string[i++];
	}
	cout<<"\n";
}


void Gen_CodeSeq1(double CodeSeq[],double data[],double Y1[],double z[],int M)
{
	int i;
	for( i=0; i< M*M; i ++ )
	{
		CodeSeq[i*3] = data[i];
		if (data[i]< -100000000)
			cout<<"MAGITE!!!"<<i<<"Data="<<dataPerr[i];
		if (!(i%2))
			CodeSeq[i*3+1] = Y1[i];
		else
		    CodeSeq[i*3+1] = 0.0;
		CodeSeq[i*3+2] = z[i];
	}

//X-Y1-z-X-0-z-X-Y1-z
}

void Gen_CodeSeq2(double CodeSeq[],double data[],double Y2[],int M)
{
	int i;
	for( i=0; i< M*M; i ++ )
	{
		CodeSeq[i*3] = data[i];
		if (i%2)
			CodeSeq[i*3+1] = Y2[i];
		else
		    CodeSeq[i*3+1] = 0.0;
		CodeSeq[i*3+2] = 0.0;
	}

//X-0-0-X-Y2-0-X-0-0-X-Y2-0
}

void Gen_deCodeSeq(double deCodeSeq[], double data[], double z[], double (*Sigma0)[32], double (*Sigma1)[32],int M, int t, double s, double sig_z)
{
	int sost, NumIar;
	double Sigma_Sum0, Sigma_Sum1;
	for (NumIar=1; NumIar<=M*M; NumIar++)
	{
		Sigma_Sum0 = Sigma_Sum1 = 0;
		for (sost=0; sost<t; sost++)
		{
			Sigma_Sum0=Sigma_Sum0+Sigma0[NumIar][sost];
			Sigma_Sum1=Sigma_Sum1+Sigma1[NumIar][sost];
		}
		if ( (Sigma_Sum1 == 0)&&(Sigma_Sum0 != 0) )
			deCodeSeq[NumIar-1] = (2*data[NumIar-1]/(s*s))+ (2*z[NumIar-1]/(sig_z*sig_z))-10;
		if ( (Sigma_Sum1 != 0)&&(Sigma_Sum0 == 0) )
			deCodeSeq[NumIar-1] = (2*data[NumIar-1]/(s*s))+ (2*z[NumIar-1]/(sig_z*sig_z))+10;
		if ( (Sigma_Sum1 == 0)&&(Sigma_Sum0 == 0) )
			deCodeSeq[NumIar-1] = (2*data[NumIar-1]/(s*s))+ (2*z[NumIar-1]/(sig_z*sig_z));
		if ( (Sigma_Sum1 != 0)&&(Sigma_Sum0 != 0) )
			deCodeSeq[NumIar-1] = (2*data[NumIar-1]/(s*s))+ (2*z[NumIar-1]/(sig_z*sig_z)) + ( log(Sigma_Sum1) - log(Sigma_Sum0) )/log_2;
	//	deCodeSeq[NumIar-1] =  (log(Sigma_Sum1/Sigma_Sum0))/log(2);
	}

}

void Gen_deCodeSeq_on2decoder(double deCodeSeq[], double NonUnIntz[], double CodeSeq[], double (*Sigma0)[32], double (*Sigma1)[32],int M, int t, double s)
{
	int sost, NumIar;
	double Sigma_Sum0, Sigma_Sum1;
	for (NumIar=1; NumIar<=M*M; NumIar++)
	{
		Sigma_Sum0 = Sigma_Sum1 = 0;
		for (sost=0; sost<t; sost++)
		{
			Sigma_Sum0=Sigma_Sum0+Sigma0[NumIar][sost];
			Sigma_Sum1=Sigma_Sum1+Sigma1[NumIar][sost];
		}
		if ( (Sigma_Sum1 == 0)&&(Sigma_Sum0 != 0) )
			NonUnIntz[NumIar-1] = -10;
		if ( (Sigma_Sum1 != 0)&&(Sigma_Sum0 == 0) )
			NonUnIntz[NumIar-1] = 10;
		if ( (Sigma_Sum1 == 0)&&(Sigma_Sum0 == 0) )
			NonUnIntz[NumIar-1] = 0;
		if ( (Sigma_Sum1 != 0)&&(Sigma_Sum0 != 0) )
			NonUnIntz[NumIar-1] = ( log(Sigma_Sum1) - log(Sigma_Sum0) )/log_2;

		deCodeSeq[NumIar-1] = (2*CodeSeq[NumIar-1]/(sig_L1*sig_L1)) + NonUnIntz[NumIar-1];
	}

}

void VichGamma( double (*Gamma0)[32], double (*Gamma1)[32], double y[], int M, trellis trl, int t, int v, int n, double s, double sig_z)
{

//**********************************************************************
//**  Gamma0[0][0] - Переходим из 0 яруса из 0 состояния, получив 0 .  **
//**  Gamma1[2][0] - Переходим из 2 яруса из 0 состояния, получив 1 .  **
//**********************************************************************


// t - number of state
// v - length constraint
// M*M - length of info
// y - code 3*M*M - length

	limit = -1;
	double vih[3];
	double a0,a1,a2;
	double sum_max = 0;
	int k = 19;
	double const1 =  2*3.14*sqrt(2*3.14);// 1/( (2*pi)^2/3 )
    double const2 =  2*3.14;             // 1/2*pi
    double const3 =  sqrt(2*3.14);       // 1/( (2*pi)^1/2 )

	double x[3]; //vihod po trellis 3- t.k. R=1/2 + z

	
	double (*temp_Gamma)[32] = new double[M*M+1][32];
	for (int i=0; i<=M*M; i++)
		for (int j=0; j<t; j++)
			temp_Gamma[i][j] = -100000000000;
	temp_Gamma[0][0] = 0;

		
	//-------calculate GAMMA------------
	int NextSost;
	int KolDostSost = 1;

	// DENIS_______________________________________________________
	int free_sost[64];
	for (int u=0; u<64; u++)
		free_sost[u] = 1;    // vse sost-ia svobodni!!!
	free_sost[0] = 0;
	//__________________________________________________________________

	int *DostSost = new int[2*t];

	DostSost[0]=0;// available state
	for (int shag_y=0; shag_y<3*M*M; shag_y=shag_y+3)
	{
		double average = 0;
		double max = -100000000000;		
		int flag = 0;
		int NumIar = int(shag_y)/3;// !!!!!!!!!!!!!!! start = 0 state ( nixt 1)!!!!!!!!!!!!!!!!!!!!!!!!

		vih[0] = y[shag_y];     //info
		vih[1] = y[shag_y+1];   //prover
		vih[2] = y[shag_y+2];   //z
		
		int i = KolDostSost;
		double temp = 0;

	//	if (NumIar == 50000)
	//		cout<<"50000";

		//x   - before Gaussian channel (trellis)
		//vih - after Gaussian channel (with noise)

		for (int sost=0; sost<KolDostSost; sost++)
		{
	//		cout<<"Nomer iarusa = "<<NumIar<<"sost = "<<DostSost[sost]<<"temp = "<<temp_Gamma[ NumIar ][ DostSost[sost] ];
		//	getchar();
	//		if (temp_Gamma[ NumIar ][ DostSost[sost] ] > limit)  // if state survive....
			for (int vh_simbol=0; vh_simbol<2; vh_simbol++) //vh_simbol = i 
			{
				
				toGauss( (trl.Out[ DostSost[sost] ][vh_simbol]), x);
				NextSost = trl.Next[ DostSost[sost] ][vh_simbol];
				
				if (n == 1)
				{
					a1 =  -( (vih[1]-x[1])*(vih[1]-x[1]) ) /( 2*s*s ); // redudance
					if (vih[1] != 0.0)
						temp =  log(1/(2*const3*s))+a1  ;
					else
						temp =  log(0.5) ;

					count_sum = count_sum + 4;
					count_mul = count_mul + 6;

				}

				if (n == 2)
				{
					a0 = -(  (vih[0]-x[0])*(vih[0]-x[0]) ) /(   2*sig_L1*sig_L1 );//info
					a1 = -(  (vih[1]-x[1])*(vih[1]-x[1]) ) /(   2*s*s );		   //redudance
					if (vih[1] != 0.0)
						temp =  log(1/(2*const2*s*s))+a0+a1;
					else
						temp =  log(1/(2*const3*s))+a0 ;

					count_sum = count_sum + 8;
					count_mul = count_mul + 13;
				}

				if (n == 3)
				{
					a0 =  -( (vih[0]-x[0])*(vih[0]-x[0]) ) /( 2*s*s );			// info
					a1 =  -( (vih[1]-x[1])*(vih[1]-x[1]) ) /( 2*s*s );          // redudance 
					if (vih[2] >10)
						vih[2] = 10;
					if (vih[2] <-10)
						vih[2] = -10;
					a2 =  -( (vih[2]-x[2])*(vih[2]-x[2]) ) /( 2*sig_z*sig_z );  // extrinsic
					
					if (vih[1] != 0.0)
						temp =  log( 1/(2*const1*s*s*sig_z) )+a0+a1+a2 ;
					else
						temp =  log( 1/(2*const2*s*sig_z) )+a0+a2;

					count_sum = count_sum + 12;
					count_mul = count_mul + 18;
				}

				
			//	if ( (temp < limit)&&(NumIar<M*M) )
			//		Survive_States[NumIar+1][NextSost] -=1;

				if (temp_Gamma[NumIar+1][NextSost] < temp + temp_Gamma[NumIar][ DostSost[sost] ] )
					temp_Gamma[NumIar+1][NextSost] = temp + temp_Gamma[NumIar][ DostSost[sost] ];

				temp = exp(temp);

			    if (temp == 0)
				{
	//				cout<<"temp = 0";
					temp = MIN;
				}
				if (vh_simbol==0)
					Gamma0[NumIar][ DostSost[sost] ] = temp;
				if (vh_simbol==1)
					Gamma1[NumIar][ DostSost[sost] ] = temp;
		//_____________________________________DENIS______________________________		
				if (KolDostSost<t) 
					if (free_sost[NextSost])
					{
						DostSost[KolDostSost++] = NextSost;
						free_sost[NextSost] = 0;
					}
		//_________________________________________________________________________

			}
//			if sostoiania net -> nado sled 2 sostoiania ubavit`

		
		} //end of sost

			
//		if (flag != 0)
		for (int u = 0; u < t; u++)
			if (temp_Gamma[NumIar+1][u]>max)
				max = temp_Gamma[NumIar+1][u];

//		limit = (-3)*(NumIar+1) - k;
		if (max == 0) cout<<"-------------------------------";
		limit = max-13;

//		cout<<NumIar;
//		for (int state=0; state<KolDostSost; state++)
//		{ 
//			int s = DostSost[state];
//			average = average + temp_Gamma[NumIar][s]; 
//		}
//		average /= KolDostSost;
//		cout<<" sum_max  "<<sum_max;
//		getchar();

		if (KolDostSost<t) 
			Sort( DostSost, KolDostSost);
			


		
	}// end of NumIar
	delete[] DostSost;
	delete[] temp_Gamma;

}

void Sort(int *array, int length)
{

	int k,i,temp;

	for (k=0; k<length/2+1; k++)
		for (i=k; i<length-1; i++)
			if (array[i] > array[i+1])
			{
				temp = array[i];
				array[i] = array[i+1];
				array[i+1] = temp;
			}
}


int *DelRepeatElems(int *array, int length_old, int &length_new)
{
	int k=1, i;
	int* result = (int*)malloc(sizeof(int)*length_old);
	short flag=0;

	result[0] = array[0];
	for (i=1;i<length_old;i++)
	{
		for (int j=0; j<k; j++)
			if (result[j] == array[i])
				flag=1;
		if (flag==0)
			result[k++]=array[i];
		else
			flag=0;
	}

	length_new=k;
	return result;
}

void toGauss(int num, double x[3])// to 2 and 1 - 1 0 - -1
{
	//double* x = (double*)malloc(sizeof(double)*3);

	if (num==0) { x[0]=-1; x[1]=-1;}
	if (num==1) { x[0]=-1; x[1]= 1;}
	if (num==2) { x[0]= 1; x[1]=-1;}
	if (num==3) { x[0]= 1; x[1]= 1;}

	x[2] = x[0]; //for z

}

void VichAlpha( double (*Alpha)[32], double (*Gamma0)[32], double (*Gamma1)[32], int M, int t, trellis trl)
{
    // iz sost v drsost
	Alpha[0][0] = 1;
	int NumIar, sost, drsost;
	double A = 0 ;
	double B = 0 ;
	
	for (NumIar=1; NumIar<=M*M; NumIar++)
	{
		A = 0;
		for (drsost=0; drsost<t; drsost++)
			for (sost=0; sost<t; sost++)
				A = A + Alpha[NumIar-1][sost]* ( Gamma0[NumIar-1][sost]+Gamma1[NumIar-1][sost] );
		for (drsost=0; drsost<t; drsost++)
		{
			B = 0;
	
			sost = trl.Prev[drsost][0];// pereshli iz sost v drsost poluchiv 0
			B = B + Alpha[NumIar-1][sost]*Gamma0[NumIar-1][sost];

			sost = trl.Prev[drsost][1];
			B = B + Alpha[NumIar-1][sost]*Gamma1[NumIar-1][sost];

			if (A!=0)
				Alpha[NumIar][drsost] = B/A;
			else
				Alpha[NumIar][sost] = 0;
		}
	}

}

void VichBetta( double (*Alpha)[32], double (*Betta)[32], double (*Gamma0)[32], double (*Gamma1)[32], int M, int t, trellis trl)
{

	int NumIar, sost, drsost;
	double A = 0 ;
	double B = 0 ;
	
	for (sost=0; sost<t; sost++)
		Betta[M*M][sost] = (double)1/t;
	
	for (NumIar = M*M-1;NumIar>=0;NumIar--)
	{
		A = 0;
		for (drsost=0; drsost<t; drsost++)
			for (sost=0; sost<t; sost++)
				A = A + Alpha[NumIar][sost]*(Gamma0[NumIar][sost]+Gamma1[NumIar][sost]);

		for (drsost=0; drsost<t; drsost++)
		{
			B = 0;

			sost = trl.Next[drsost][0];// pereshli iz drsost v sost poluchiv 0
			B = B + Betta[NumIar+1][sost]*Gamma0[NumIar][drsost];
			sost = trl.Next[drsost][1];
			B = B + Betta[NumIar+1][sost]*Gamma1[NumIar][drsost];

			if (A!=0)
				Betta[NumIar][drsost] = B/A;
			else
				Betta[NumIar][drsost] = 0;
		}
	}

}

void VichSigma( double (*Sigma0)[32], double (*Sigma1)[32], double (*Alpha)[32], double (*Betta)[32], double (*Gamma0)[32], double (*Gamma1)[32], int M, int t, trellis trl)
{
	int NumIar, sost;

	for (NumIar=1; NumIar<=M*M; NumIar++)
		for (sost=0; sost<t; sost++)
		{
			Sigma0[NumIar][sost] = Alpha[NumIar-1][sost]*Gamma0[NumIar-1][sost]*Betta[NumIar][trl.Next[sost][0]];
			Sigma1[NumIar][sost] = Alpha[NumIar-1][sost]*Gamma1[NumIar-1][sost]*Betta[NumIar][trl.Next[sost][1]];
		}
		

}


void turbocoder( double G1[], double G2[], double data[], 
				   int M, double Y1[], double Y2[] )
{
   trellis trl;
   int *tr = (int*)malloc(sizeof(int)*M*M); 
   double *newdata = (double*)malloc(sizeof(double)*M*M);
   
   Transposition( M, tr ); // Get Data for Interleaving

   // First RSC Encoder
   trl = GetTrellis( G1 );
   RSCEncoder( trl, data, M*M, Y1 ); // Only Control bit

   NonUnInt( data, M, tr, newdata ); // Interleaving

   // Second RSC Encoder
   trl = GetTrellis( G2 );
   RSCEncoder( trl, newdata, M*M, Y2 ); // Only Control bit

}

int GetV( double G[] )
{
  // G[0] = 0...01XXXX... i.e. !!!Recurcive!!! Systematic Code 
  int limit,g1 = (int)G[0],test,ret, i;

  limit = sizeof(int)*8;
  test = 0x80000000;
  for( i = 0; i < limit; i++ )
  {
	  if( g1&(test>>i) )
	  {
        ret = sizeof(int)*8 - i - 1;
		break;
	  }
  }
  return ret;
}

trellis GetTrellis( double G[] )
{
  trellis trl;
  int NumOfStates,state,input,st,exit,nextstate,code,v;

  v = GetV( G );
  NumOfStates = (int)pow( 2.0, v );

  trl.Next = (int(*)[2])malloc(sizeof(int)*NumOfStates*2);
  trl.Prev = (int(*)[2])malloc(sizeof(int)*NumOfStates*2);
  trl.Out = (int(*)[2])malloc(sizeof(int)*NumOfStates*2);

  for( state = 0; state < NumOfStates; state++ ) 
  {
	  for( input = 0; input < 2; input++ )
	  {
        st = SetBit( state, v, input ); // [ input state ]
	    exit = (HemWeight( st&((int)G[0]) ))%2 ; // Bit from recursion summator
		nextstate = SetBit( (state>>1), v-1, exit );
		st = SetBit( state, v, exit ); // [ exit state ]
		exit = (HemWeight( st&(int)G[1] ))%2; // Control bit

		code = SetBit( exit, 1, input ); // [ input code ]

        trl.Next[state][input] = nextstate;
		trl.Prev[nextstate][input] = state;
		trl.Out[state][input] = code;
	  }
  }

  return trl;
}

void RSCEncoder( trellis trl, double data[], int DataSize, double code[] )
{
   int state = 0,out, i;
   
   for( i = 0; i < DataSize; i++ )
   {
     out = trl.Out[state][(int)data[i]];

     out = out&1; // 0-bit = Control bit

     code[i] = out;

     state = trl.Next[state][(int)data[i]];
   }
}

void Transposition( int M, int transp[] )
{
	//int P[8] = { 1, 3, 5, 7, 9, 11, 13, 15 };
	int P[8] = { 17, 37, 19, 29, 41, 23, 13, 7 };
    int eps,i,j,ir,jr;

    for( i = 0; i < M; i++ )
      for( j = 0; j < M; j++ )
	  {
        ir = ( ( M/2 + 1 )*( i + j ) )%M;
        eps = (i + j)%8;
        jr = ( (P[eps]*(j+1)) - 1 )%M;
		transp[i*M + j] = ir*M + jr; 
      }
}

void NonUnInt( double data_in[], int M, int transp[], double data_out[] )
{
  for( int i = 0; i < M*M; i++ )
	  data_out[i] = data_in[transp[i]];
}

void DeNonUnInt( double data_in[], int M, int transp[], double data_out[] )
{
   for( int i = 0; i < M*M; i++ )
	  data_out[transp[i]] = data_in[i];
}

int SetBit( int number, int bitnum, int bit )
{
  int ret;

  if( bit )
    ret = number|(1<<bitnum);
  else
	ret = number&(~(1<<bitnum));

  return ret;
}

int HemWeight( int number )
{
  int i,count = 0;

  for( i = 0; i < ( (sizeof(int))*8 ); i++ )
   count += ( (number>>i) & 1 );

  return count;
}

double randn( double M, double D )
{
  // M - mean
  // D - dispersion
  // n = randn( 0, 1 ); // Standart normal distribution -3 < n < 3
  
  double U = 1.0,V,Y,Z,R = RAND_MAX;
   
  while( (U == 1.0)||(U == 0.0) ){ U = rand()/R; } //  0 =< U < 1

  if( U < 0.5 )
	  V = U;
  else
	  V = 1 - U;

  Y = sqrt( -2*log( V ) );
  Z = (2.515517 + Y*(0.802853 + Y*0.010328))/(1 + Y*(1.432788 + Y*(0.189269 + Y*0.001308))) - Y;

  if( U >= 0.5 ) Z = -Z;

  
  return sqrt(D)*Z + M;
}   

double sigma( double mas[], int size )
{
  double variance = 0,v;
  double mean = 0;

  for( int k = 0; k < size; k++ )
	  mean = mean + fabs(mas[k]);

  mean = mean/size;

  for( k = 0; k < size; k++ )
  {
	  v = (fabs(mas[k]) - mean);
	  variance = variance + v*v;
  }

  variance = sqrt(variance/size);

  return variance;

}