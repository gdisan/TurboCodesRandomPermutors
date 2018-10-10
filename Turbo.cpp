

/* ------------------------------======= Turbo Codec ========--------------------------------- */

#include "stdafx.h"
#include "Turbo.h"

// Initial Data without Noise  
int *global_data;

void turbo_decoder( RSC* rsc1, Perm* perm, RSC* rsc2, double data[],
			        int N, double Y1[], double Y2[], double s,int Hard[], double Soft[], 
			        bool bPunctured, int MaxNumOfIteration )
{

   	int v1,v2;
	int t; // Number of States
	int Nerr=0; // Error Number Counter
	double sig_L1; 
    double sig_z = 1;
	int i,j;


	double *CodeSeq = new double[3*N+MAXV*3]; // Sequence to Decoders
	double *z = new double[N+MAXV];// Extrinsic Information

	
	double *NonUnIntCodeSeq = new double[N+MAXV]; // CodeSeq after Permutor
	double *NonUnIntdecode = new double[N+MAXV]; // Soft after Permutor
	double *NonUnIntz = new double[N+MAXV]; // Extrinsic Information before DePermutor

    double (*Gamma0Y)[MAXSTATES] = new double[N+MAXV+1][MAXSTATES];
	double (*Gamma1Y)[MAXSTATES] = new double[N+MAXV+1][MAXSTATES];

	double (*Gamma0Rk)[MAXSTATES] = new double[N+MAXV+1][MAXSTATES];
	double (*Gamma1Rk)[MAXSTATES] = new double[N+MAXV+1][MAXSTATES];

    double (*Alpha)[MAXSTATES] = new double[N+MAXV+1][MAXSTATES];

	double (*Betta)[MAXSTATES] = new double[N+MAXV+1][MAXSTATES];

	double (*Sigma0)[MAXSTATES] = new double[N+MAXV+1][MAXSTATES];
	double (*Sigma1)[MAXSTATES] = new double[N+MAXV+1][MAXSTATES];


    for( i=0; i < N; i++ ){ Hard[i] = 0; z[i]=0; }
	for( i=N; i < N+MAXV; i++ ){ z[i]=0; }
    v1 = rsc1->GetV();
	v2 = rsc2->GetV();

for( int KolIter = 0; KolIter < MaxNumOfIteration; KolIter++ )
{
   
/* ------------------------------- First Part------------------------------- */

	sig_L1=s;
	t = (int)pow( 2.0, v1 ); 

	for( i = 0; i <= N + v1; i++ )
	{
	   for( j = 0; j < t; j++ )
	   {
			Gamma0Y[i][j] = Gamma1Y[i][j] = Gamma0Rk[i][j] = Gamma1Rk[i][j] = 0.0;
			Sigma0[i][j] = Sigma1[i][j] = 0.0;
			Alpha[i][j] = 0.0;
			Betta[i][j] = 0.0;
		}
	}

    Gen_CodeSeq1( CodeSeq, data, Y1, z, N, bPunctured, v1 );

	if(KolIter == 0)
		VichGamma( Gamma0Rk, Gamma1Rk, CodeSeq, N, rsc1, t, v1, 2, s, sig_z, sig_L1 ); // 2 i.e. for Rk = Xk Yk
	else
		VichGamma( Gamma0Rk, Gamma1Rk, CodeSeq, N, rsc1, t, v1, 3, s, sig_z, sig_L1 ); // 3 i.e. for Rk = Xk Yk Zk

	VichGamma( Gamma0Y, Gamma1Y, CodeSeq, N, rsc1, t, v1, 1, s, sig_z, sig_L1 ); // 1 i.e. only for Yk

	VichAlpha( Alpha, Gamma0Rk, Gamma1Rk, N, t, rsc1 );
	VichBetta(Alpha, Betta, Gamma0Rk, Gamma1Rk, N, t, rsc1 );
	VichSigma(Sigma0, Sigma1, Alpha, Betta, Gamma0Y, Gamma1Y, N, t, rsc1 );

	Gen_deCodeSeq( Soft, data, z, Sigma0, Sigma1, N, t, s, sig_z, v1 );


    /* Get Number of Error */
	for( i = 0; i < N; i++ )
	{
	   if( (!( global_data[i] )) && (Soft[i]>0) )
		   Nerr++;
	   if( ( global_data[i] ) && (Soft[i]<0) )
		   Nerr++;

	   if( Soft[i] > 10 )
	     Soft[i] = 10;

	   if( Soft[i] < -10 )
	     Soft[i] = -10;
	}

	if( Nerr == 0 )
		break;
	Nerr=0;

/* -------------------- Second Part ------------------------------- */

	
	sig_L1 = sigma( Soft, N );
	t = (int)pow( 2.0, v2 ); 

	for( i=0; i <= N + v2; i++ )
		for( j=0; j < t; j++ )
			{
				Gamma0Y[i][j] = Gamma1Y[i][j] = Gamma0Rk[i][j] = Gamma1Rk[i][j] = 0.0;
				Sigma0[i][j] = Sigma1[i][j] = 0.0;
				Alpha[i][j] = 0.0;
				Betta[i][j] = 0.0;
			}

	perm->Interleave( Soft,NonUnIntCodeSeq );  

    for( i = 0; i < v2; i++ ) NonUnIntCodeSeq[N+i] = data[N+v1+i]; // Add RSC2 tail bits

	Gen_CodeSeq2( CodeSeq, NonUnIntCodeSeq, Y2, N, bPunctured, v2 );

	VichGamma(Gamma0Rk, Gamma1Rk, CodeSeq, N, rsc2, t, v2, 2, s, sig_z, sig_L1 );
	VichGamma(Gamma0Y, Gamma1Y, CodeSeq, N, rsc2, t, v2, 1, s, sig_z, sig_L1 );

	VichAlpha(Alpha, Gamma0Rk, Gamma1Rk, N, t, rsc2 );
	VichBetta(Alpha, Betta, Gamma0Rk, Gamma1Rk, N, t,rsc2 );
 	VichSigma(Sigma0, Sigma1, Alpha, Betta, Gamma0Y, Gamma1Y, N, t, rsc2 );

	Gen_deCodeSeq_on2decoder( NonUnIntdecode, NonUnIntz, NonUnIntCodeSeq,  Sigma0, Sigma1, N, t, sig_L1, v2 );

	perm->DeInterleave( NonUnIntdecode, Soft );   
	perm->DeInterleave( NonUnIntz, z ); 

    sig_z = sigma( z, N );

    /* Get Number of Error */
	for( i = 0; i < N; i++ )
	{

	   if( (!( global_data[i] )) && (Soft[i]>0) )
		   Nerr++;
	   if( ( global_data[i] ) && (Soft[i]<0) )
		   Nerr++;
	}

	if (Nerr == 0)
		break;
	Nerr=0;
	
} // End of Iteration

    // Get Hard Decisions
	for( i = 0; i < N; i++ )
	  if( Soft[i] > 0 )
		  Hard[i] = 1;
	  else
		  Hard[i] = 0;
		
    delete[] CodeSeq; 
	delete[] z;
	
	delete[] NonUnIntCodeSeq;
	delete[] NonUnIntdecode;
	delete[] NonUnIntz;

    delete[] Gamma0Y;
	delete[] Gamma1Y;

	delete[] Gamma0Rk;
	delete[] Gamma1Rk;

    delete[] Alpha;

	delete[] Betta;

	delete[] Sigma0;
	delete[] Sigma1;
}


void Gen_CodeSeq1(double CodeSeq[],double data[],double Y1[],double z[],int N, bool bPunctured, int v )
{	
	int i;

	for( i=0; i < N; i ++ )
	{
		CodeSeq[i*3] = data[i];
		if (!(i%2))
			CodeSeq[i*3+1] = Y1[i];
		else
		{
			if( bPunctured )
		      CodeSeq[i*3+1] = 0.0; // X-Y1-z-X-0-z-X-Y1-z  R = 1/2
			else
              CodeSeq[i*3+1] = Y1[i]; // X-Y1-z-X-Y1-z-X-Y1-z R = 1/3

		}
		CodeSeq[i*3+2] = z[i];
	}

	// Tail bits
	for( i=0; i < v; i++ )
	{
	  CodeSeq[N+i*3] = data[N+i];
      CodeSeq[N+i*3+1] = Y1[N+i];
	  CodeSeq[N+i*3+2] = 0.0; 
	}
}

void Gen_CodeSeq2(double CodeSeq[],double data[],double Y2[],int N, bool bPunctured, int v2 )
{
	int i;
 
	for( i=0; i< N; i ++ )
	{
		CodeSeq[i*3] = data[i];
		if ((i%2))                    
			CodeSeq[i*3+1] = Y2[i];
		else
		{
			if( bPunctured )
		      CodeSeq[i*3+1] = 0.0; // X-0-0-X-Y2-0-X-0-0-X-Y2-0  R = 1/2
			else
			  CodeSeq[i*3+1] = Y2[i]; // X-Y2-0-X-Y2-0-X-Y2-0-X-Y2-0 R = 1/3
		} 
		CodeSeq[i*3+2] = 0.0;
	}

	// Tail bits
	for( i=0; i < v2; i++ )
	{
	  CodeSeq[N+i*3] = data[N+i];
      CodeSeq[N+i*3+1] = Y2[N+i];
	  CodeSeq[N+i*3+2] = 0.0; 
	}
}

void Gen_deCodeSeq(double deCodeSeq[], double data[], double z[], double (*Sigma0)[MAXSTATES], 
				   double (*Sigma1)[MAXSTATES],int N, int t, double s, double sig_z, int v )
{
	int sost, NumIar;
	double Sigma_Sum0, Sigma_Sum1;
	const double log_2 = log(2.0);

	for (NumIar=1; NumIar<=N+v; NumIar++)
	{
		Sigma_Sum0 = Sigma_Sum1 = 0;
		for (sost=0; sost<t; sost++)
		{
			Sigma_Sum0=Sigma_Sum0+Sigma0[NumIar][sost];
			Sigma_Sum1=Sigma_Sum1+Sigma1[NumIar][sost];
		}

		deCodeSeq[NumIar-1] = (2*data[NumIar-1]/(s*s))+ ( log(Sigma_Sum1) - log(Sigma_Sum0) )/log_2;
	}

}

void Gen_deCodeSeq_on2decoder(double deCodeSeq[], double NonUnIntz[], double CodeSeq[],
							  double (*Sigma0)[MAXSTATES], double (*Sigma1)[MAXSTATES],int N,
							  int t, double s, int v )
{
	int sost, NumIar;
	double Sigma_Sum0, Sigma_Sum1;
	const double log_2 = log(2.0);

	for (NumIar=1; NumIar<=N+v; NumIar++)
	{
		Sigma_Sum0 = Sigma_Sum1 = 0;
		for (sost=0; sost<t; sost++)
		{
			Sigma_Sum0=Sigma_Sum0+Sigma0[NumIar][sost];
			Sigma_Sum1=Sigma_Sum1+Sigma1[NumIar][sost];
		}
		NonUnIntz[NumIar-1] = ( log(Sigma_Sum1) - log(Sigma_Sum0) )/log_2;
		deCodeSeq[NumIar-1] = (2*CodeSeq[NumIar-1]/(s*s)) + NonUnIntz[NumIar-1];
	}

}

void VichGamma( double (*Gamma0)[MAXSTATES], double (*Gamma1)[MAXSTATES], double y[], int N,
			   RSC* rsc, int t, int v, int n, double s, double sig_z, double sig_L1 )
{

	double vih[3];
	double a0,a1,a2;
	double const1 =  2*3.14*sqrt(2*3.14); // 1/( (2*pi)^2/3 )
    double const2 =  2*3.14;              // 1/2*pi
    double const3 =  sqrt(2*3.14);        // 1/( (2*pi)^1/2 )

	double x[3]; // Trellis OutPut = 3 bits - i.e. R = 1/2 + z

	int NextSost;
	int KolDostSost = 1;

    int free_sost[MAXSTATES];
	for( int u = 0; u < MAXSTATES; u++ ) free_sost[u] = 1; // All States are free
	free_sost[0] = 0;

	int *DostSost = new int[2*t];

	DostSost[0] = 0; // Available State  
	int limit;
	double temp_sig_L1 = sig_L1;

	limit = 3*N + v*3;

    for ( int shag_y = 0; shag_y < limit; shag_y = shag_y+3 )
	{
		int NumIar = int(shag_y)/3+1; // Look laba5

		if( shag_y == 3*N ) // tail bits
        {
			if( n == 3 ) n = 2;
            sig_L1 = s;  
		}

		vih[0] = y[shag_y];     // info bit
		vih[1] = y[shag_y+1];   // parity bit
		vih[2] = y[shag_y+2];   // extrinsic bit
		int i = KolDostSost;
		double temp;

		// x   - before Gaussian channel (from trellis)
		// vih - after Gaussian channel (with noise)

		for( int sost = 0; sost < KolDostSost; sost++ )
		{
			for( int vh_simbol = 0; vh_simbol < 2; vh_simbol++ ) // vh_simbol = i
			{
				toGauss( rsc->Out( DostSost[sost], vh_simbol ), x );
				NextSost = rsc->Next( DostSost[sost], vh_simbol );
				
				if(n == 1)
				{
					a1 = exp( ( -( pow( (vih[1]-x[1]),2) )  )/( 2*s*s ) );
					if(vih[1] != 0.0)
						temp =  0.5*( 1/(const3*s)*a1);
					else
						temp =  0.5 ;
				}

				if(n == 2)
				{
					a0 = exp( ( -( pow( (vih[0]-x[0]),2) )  )/( 2*sig_L1*sig_L1 ) );
					a1 = exp( ( -( pow( (vih[1]-x[1]),2) )  )/( 2*s*s ) );
					if(vih[1] != 0.0)
						temp =  0.5*( ( 1/(const2*s*s) )*a0*a1 );
					else
						temp =  0.5*( ( 1/(const3*s) )*a0  );
				}

				if(n == 3)
				{
					a0 = exp( ( -( pow( (vih[0]-x[0]),2) )  )/(   2*sig_L1*sig_L1 ) );
					a1 = exp( ( -( pow( (vih[1]-x[1]),2) )  )/(   2*s*s ) );
					if( vih[2] > 10 ) vih[2] = 10;
					if( vih[2] < -10 ) vih[2] = -10;
					a2 = exp( ( -( pow( (vih[2]-x[2]),2) )  )/(   2*sig_z*sig_z ) );
					if(vih[1] != 0.0)
						temp =  0.5*( ( 1/(const1*s*s*sig_z) )*a0*a1*a2 ) ;
					else
						temp =  0.5*( ( 1/(const2*s*sig_z) )*a0*a2  ) ;
				}

			    if(temp == 0)
					temp = 1e-200;
				if(vh_simbol==0)
					Gamma0[NumIar][ DostSost[sost] ] = temp;
                if(vh_simbol==1)
					Gamma1[NumIar][ DostSost[sost] ] = temp;
			
				if( KolDostSost < t )
				{
					if( free_sost[NextSost] )
					{
						DostSost[KolDostSost++] = NextSost;
						free_sost[NextSost] = 0;
					}
				}

			} // symbol
		} // sost

		if( KolDostSost < t)
			Sort( DostSost, KolDostSost);

	} // shag_y

	sig_L1 = temp_sig_L1;
	delete[] DostSost;
}

void Sort( int *array, int length )
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

void toGauss( int num, double x[] ) // to 2 and 1 - 1 0 - -1
{
	
	if (num==0) { x[0]=-1; x[1]=-1;}
	if (num==1) { x[0]=-1; x[1]= 1;}
	if (num==2) { x[0]= 1; x[1]=-1;}
	if (num==3) { x[0]= 1; x[1]= 1;}

	x[2] = x[0]; // for z

}

void VichAlpha( double (*Alpha)[MAXSTATES], double (*Gamma0)[MAXSTATES], double (*Gamma1)[MAXSTATES],
			    int N, int t, RSC* rsc )
{
    // from sost to drsost
	Alpha[0][0] = 1;
	int NumIar, sost, drsost;
	double A = 0 ;
	double B = 0 ;
	
	int limit = N + rsc->GetV(); 
	for (NumIar=1; NumIar<=limit; NumIar++)
	{
		A = 0;
		for (drsost=0; drsost<t; drsost++)
			for (sost=0; sost<t; sost++)
				A = A + Alpha[NumIar-1][sost]* ( Gamma0[NumIar][sost]+Gamma1[NumIar][sost] );
		for (drsost=0; drsost<t; drsost++)
		{
			B = 0;
	
			sost = rsc->Prev( drsost, 0 );
			B = B + Alpha[NumIar-1][sost]*Gamma0[NumIar][sost];

			sost = rsc->Prev( drsost, 1 );
			B = B + Alpha[NumIar-1][sost]*Gamma1[NumIar][sost];

			if (A!=0)
				Alpha[NumIar][drsost] = B/A;
			else
				Alpha[NumIar][sost] = 0;
		}
	}

}

void VichBetta( double (*Alpha)[MAXSTATES], double (*Betta)[MAXSTATES], double (*Gamma0)[MAXSTATES],
			   double (*Gamma1)[MAXSTATES], int N, int t, RSC* rsc )
{

	int NumIar, sost, drsost;
	double A = 0 ;
	double B = 0 ;
	
	int v = rsc->GetV();
	for (sost=0; sost<t; sost++)
		Betta[N+v][sost] = (double)1/t;
	
	for (NumIar = N+v-1;NumIar>=0;NumIar--)
	{
		A = 0;
		for (drsost=0; drsost<t; drsost++)
			for (sost=0; sost<t; sost++)
				A = A + Alpha[NumIar][sost]*(Gamma0[NumIar+1][sost]+Gamma1[NumIar+1][sost]);

		for (drsost=0; drsost<t; drsost++)
		{
			B = 0;

			sost = rsc->Next(drsost,0 );
			B = B + Betta[NumIar+1][sost]*Gamma0[NumIar+1][drsost];
			sost = rsc->Next( drsost, 1 );
			B = B + Betta[NumIar+1][sost]*Gamma1[NumIar+1][drsost];

			if (A!=0)
				Betta[NumIar][drsost] = B/A;
			else
				Betta[NumIar][drsost] = 0;
		}
	}

}

void VichSigma( double (*Sigma0)[MAXSTATES], double (*Sigma1)[MAXSTATES], double (*Alpha)[MAXSTATES],
			   double (*Betta)[MAXSTATES], double (*Gamma0)[MAXSTATES], double (*Gamma1)[MAXSTATES],
			   int N, int t, RSC* rsc )
{
    int NumIar, sost;
    int v = rsc->GetV();

	for (NumIar=1; NumIar<=N+v; NumIar++)
		for (sost=0; sost<t; sost++)
		{
			Sigma0[NumIar][sost] = Alpha[NumIar-1][sost]*Gamma0[NumIar][sost]*Betta[NumIar][rsc->Next( sost,0 )];
			Sigma1[NumIar][sost] = Alpha[NumIar-1][sost]*Gamma1[NumIar][sost]*Betta[NumIar][rsc->Next( sost, 1 )];
		}
		

}

void turbocoder( RSC* rsc1, Perm* perm, RSC* rsc2, double data[],
				   int N, double Y1[], double Y2[] )
{
   int i,v1,v2;

   v1 = rsc1->GetV();
   v2 = rsc2->GetV();

   double *tail = new double[MAXV];
   double *perm_data = new double[N];

   
   rsc1->Encode( data, N, Y1, tail );

   for( i = 0; i < v1; i++ ) data[N+i] = tail[i]; // Add RSC1 tail bits to data 
 
   perm->Interleave( data, perm_data );

   rsc2->Encode( perm_data, N, Y2, tail );

   for( i = 0; i < v2; i++ ) data[N+v1+i] = tail[i]; // Add RSC2 tail bits to data

 
   delete[] tail;
   delete[] perm_data;
}

double randn( double M, double D )
{
  // M - mean
  // D - dispersion
  // n = randn( 0, 1 ); // Standart normal distribution -3 < n < 3

  double U = 1.0,V,Y,Z,R = RAND_MAX;

  while( (U == 1.0)||(U == 0.0) ){ U = rand()/R; } //  0 < U < 1

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

double GetPerr( int decode_data[], int size )
{
  int Nerr = 0;

  for( int i = 0; i < size; i++ )  
	  if( global_data[i] != decode_data[i] )
	     Nerr++;
	  
  return (double)Nerr/size;
}

double GetNerr( int decode_data[], int size )
{
  int Nerr = 0;

  for( int i = 0; i < size; i++ )  
	  if( global_data[i] != decode_data[i] )
	     Nerr++;
	  
  return Nerr;
}
