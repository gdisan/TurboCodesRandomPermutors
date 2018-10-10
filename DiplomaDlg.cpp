// DiplomaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Diploma.h"
#include "DiplomaDlg.h"

#include "def.h"
#include "Turbo.h"
#include "BerrouGlavieuxPerm.h"
#include "RandomPerm.h"
#include "OurRandomPerm.h"
#include "NonRandomPerm.h"
#include "ModNonRandomPerm.h"
#include "GppPerm.h"
#include "DVBPerm.h"
#include "CSDSPerm.h"
#include "SRandomPerm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void Modeling( TURBO_CODEC, Perm*, double r );
void SetErrors( int, double );
CString GetPermName( int );
void SetData( TURBO_CODEC );
TURBO_CODEC GetData();
CString OctToStr( int );
int OctToDec( int );
CString GetDate();
CString GetTime();
void SaveResults( TURBO_CODEC, int );
int GetWeight( double , int  );
int GetTurboWeight( TURBO_CODEC , Perm*, double*, RSC* , RSC*   );

// Results of Modeling
CStdioFile f( "results.txt", CFile::modeCreate | 
			  CFile::modeWrite | CFile::typeText );

// Current Permutation Pattern for Our Random Permutor
CFile config;

CString enter; // enter = "\n" 

TURBO_CODEC tc[NPERM];

/* ------------- Settings of Errors Stream ---------- */
 
// Number of Errors Frames
const int NS = 100;

double SNRFix = -1.0;

double *info;
double *data_err[NS];
double *code1_err[NS];
double *code2_err[NS];

/* -------------------------------------------------- */

// Globals for more convience
double* Pe; 
OurRandomPerm ourperm;

// Modeling
void CDiplomaDlg::OnOK()
{

  TURBO_CODEC tc;
  Perm* perm;
  
  double NumOfSnr;
  int err = 0;

  tc = GetData();

  switch( tc.PermType )
  {
  case BERROU:
	  perm = new BerrouGlavieuxPerm; 
	  perm->GetPermPattern( tc.N );
	  break;
  case RND:
	  perm = new RandomPerm;
	  perm->GetPermPattern( tc.N );
	  break;
  case OURRND:
      if( !(ourperm.IsSet(tc.N)) )
	  {
	    AfxMessageBox("Search a New Permutor");
		err = 1;
	  }
	  else
	    perm = &ourperm;
      break;
  case CVETKOFF1:
	  perm = new NonRandomPerm;
	  perm->GetPermPattern( 996 );
	  break;
  case CVETKOFF2:
	  perm = new ModNonRandomPerm;
	  perm->GetPermPattern( 990 );
	  break;
  case GPP:
      perm = new ModNonRandomPerm;
	  perm->GetPermPattern( 1000 );
	  break;
  case DVBRCS:
	  perm = new DVBPerm;
	  perm->GetPermPattern( 880 );
	  break;
  case CCSDS:
	  perm = new CCSDSPerm;
	  perm->GetPermPattern( 1784 );
	  break;
  case SRND:
	  perm = new SRandomPerm;
	  perm->GetPermPattern( tc.N );
	  break;
  }

  if( !err )
  {
	  if( (tc.bFixed) && (SNRFix == -1.0) )
	  {
         AfxMessageBox("First,Set Errors Stream");
	     err = 1;
	  }

	  if( !err )
	  {
	    NumOfSnr = ((int)((tc.SNREnd - tc.SNRInit)/tc.SNRStep)) + 1;
	    Modeling( tc, perm, NumOfSnr );
		if( tc.bFixed ) NumOfSnr =1;
	    SaveResults( tc,  NumOfSnr ); 
	    delete[] Pe;
	  }
	  
	  if( tc.PermType != OURRND )
	    delete[] perm;
	  
  }
}

void Modeling( TURBO_CODEC tc, Perm* perm, double interval )
{
  int i,N;
  unsigned long Nerr; 
  CString res;

  RSC rsc1,rsc2;

  N = tc.N;

  double *data = new double[N+2*MAXV];
  double *data_init = new double[N+2*MAXV];
  double *code1 = new double[N+MAXV]; 
  double *code2 = new double[N+MAXV]; 
  double *code1_init = new double[N+MAXV]; 
  double *code2_init = new double[N+MAXV];
  double *soft = new double[N+MAXV];
  int *hard = new int[N]; 
  
  global_data = new int[N];

  double  sigma; 
  double  SNRdb; 
  double SNR; 
  double E; 
 
  E = 1.0;
  Nerr = 0; 

  double j; // Counter for SNR in an interval
  int l; // Counter for Error Vector in a Stream
  int limit,range;
 
  if( tc.bFixed )
  {
	  limit = NS;
	  tc.SNRInit = SNRFix;
	  tc.SNRStep = 1.0;
	  tc.SNREnd = SNRFix;
	  interval = 1; 
	  range =  NS + 1;
  }
  else
  {
	  range = interval + 1;
	  limit = 1;
  }

  Pe = new double[interval]; 

  pdlgMain->m_Line.SetRange( 0, range );
  pdlgMain->m_Line.SetStep( 1 );
  pdlgMain->m_Line.SetPos( 0 );

  int k = 0;  // Pe Counter

  /* ----------------- Get Data For Errors Stream of For an Interval of SNRs --------------- */
  
  if( !tc.bFixed)
  {
    srand( (unsigned)time( NULL ) );
    for( i = 0; i < N; i ++ )	data_init[i] = global_data[i] = rand()%2;
  }
  else
    for( i = 0; i < N; i ++ )	data_init[i] = global_data[i] = info[i];
    

  /* ----------------------- Turbo Encoder -------------------- */
  rsc1.Set( tc.v1, tc.G1  );
  rsc2.Set( tc.v2, tc.G2 );

  turbocoder( &rsc1, perm, &rsc2, data_init, N, code1_init, code2_init );
  pdlgMain->m_Line.StepIt();

CString str;
  
for( j = tc.SNRInit; j <= tc.SNREnd; j = j + tc.SNRStep )
{
 str.Format( _T("SNR = %2.1f"), tc.SNRInit );
 pdlgMain->GetDlgItem( IDC_STATIC666 )->SetWindowText(str);
  
 for( l = 0; l < limit; l++ )   
 {
 
  if( limit != 1 )
  {
	  str.Format( _T(" NS = %d/%d"), l, NS );
	  pdlgMain->GetDlgItem( IDC_STATIC777 )->SetWindowText(str);
  }

  /* ---------- Gaussian Channel ------------------------------ */
  SNRdb = j;
  SNR = pow( 10.0, (SNRdb/10.0) );
  sigma = sqrt( E/SNR ); 
  if( !tc.bFixed )
  {
    for ( i = 0; i < N + MAXV; i++ )
	{
	  data[i] = (data_init[i]*2.0 - 1.0) + randn( 0.0, 1.0 )*sigma;
 	  code1[i] = (code1_init[i]*2.0 - 1.0) + randn( 0.0, 1.0 )*sigma;
	  code2[i] = (code2_init[i]*2.0 - 1.0) + randn( 0.0, 1.0 )*sigma;
	}
    for ( i = N + MAXV; i < N + 2*MAXV; i++ )
	{
 	  data[i] = (data_init[i]*2.0 - 1.0) + randn( 0.0, 1.0 )*sigma;
	}
  }
  else
  {
    for ( i = 0; i < N + MAXV; i++ )
	{
	      data[i] = (data_init[i]*2.0 - 1.0) + data_err[l][i];
 	      code1[i] = (code1_init[i]*2.0 - 1.0) + code1_err[l][i];
	      code2[i] = (code2_init[i]*2.0 - 1.0) + code2_err[l][i];
	}

	for ( i = N + MAXV; i < N + 2*MAXV; i++ )
      data[i] = (data_init[i]*2.0 - 1.0) + data_err[l][i];
 	
  }


  /* -------------------Turbo Decoder --------------------------------------- */
  turbo_decoder( &rsc1, perm, &rsc2, data, N, code1, code2, sigma, hard, soft,
	            tc.bPunctured, tc.MaxNumOfIteration );
  pdlgMain->m_Line.StepIt();

  Nerr = Nerr + GetNerr( hard, N );
 

 } // for( Stream ) 
  
  if( !tc.bFixed )
     Pe[k] = GetPerr( hard, N );
  else
	 Pe[k] = (double)Nerr/(N*NS);
 
  res.Format( _T("%5.4f"),Pe[k]);
  pdlgMain->GetDlgItem( IDC_EDIT2 )->SetWindowText(res);

  k++;

} // for( SNR )

  delete[] global_data;

  delete[] data;
  delete[] code1;
  delete[] code2;
  delete[] data_init;
  delete[] code1_init;
  delete[] code2_init;
  delete[] hard;
  delete[] soft;
}

void SetErrors( int N, double SNRdb )
{
   	 double E = 1.0;
     double SNR = pow( 10.0, (SNRdb/10.0) );
     double sigma = sqrt( E/SNR ); 
	 int i,j;

     SNRFix = SNRdb;

	 pdlgMain->m_Line.SetRange( 0, NS );
     pdlgMain->m_Line.SetStep( 1 );
     pdlgMain->m_Line.SetPos( 0 );

	 srand( (unsigned)time( NULL ) );
	 delete[] info;
	 info = new double[N+2*MAXV];

     for( i = 0; i < N; i++ ) info[i] = rand()%2;

	 for( i = 0; i < NS; i++ )
	 {
	   delete[] data_err[i];
	   delete[] code1_err[i];
	   delete[] code2_err[i];
	   data_err[i] = new double[N+2*MAXV];
	   code1_err[i] = new double[N+MAXV];
	   code2_err[i] = new double[N+MAXV];

       for( j = 0; j < N + MAXV; j++ )
	   {
	      data_err[i][j] = randn( 0.0, 1.0 )*sigma;
 	      code1_err[i][j] = randn( 0.0, 1.0 )*sigma;
	      code2_err[i][j] = randn( 0.0, 1.0 )*sigma;
	   }

	   for( j = N + MAXV; j < N + 2*MAXV; j++ )
	      data_err[i][j] = randn( 0.0, 1.0 )*sigma;
 	 
	   pdlgMain->m_Line.StepIt();

	 }
}

CString GetPermName( int N )
{
  CString str;

  switch( N )
  {
      case BERROU:
		  str = "Berrou&Glavieux";
		  break;
	  case CVETKOFF1:
		  str = "Зяблов&Цветков Неслучайный";
		  break;
	  case CVETKOFF2:
		  str = "Зяблов&Цветков Мод. Неслучайный";
		  break;
	  case GPP:
		  str = "3GPP";
		  break;
      case DVBRCS:
		  str = "DVB-RCS";
	      break;
	  case RND:
		  str = "Случайный";
          break; 
	  case OURRND:
		  str = "Наш случайный";
          break; 
	  case CCSDS:
		  str = "CCSDS";
          break; 
	  case SRND:
		  str = "S - Случайный";
          break; 
}

  return str;
}

void SetData( TURBO_CODEC codec )
{
  
  if( codec.bPunctured )
  {
		pdlgMain->GetDlgItem( IDC_STATIC1 )->SetWindowText("TurboCoder R = 1/2");
		pdlgMain->GetDlgItem( IDC_STATIC11 )->SetWindowText("RSC1 R = 2/3");
		pdlgMain->GetDlgItem( IDC_STATIC13 )->SetWindowText("RSC2 R = 2/3");
  }
  else
  {
	    pdlgMain->GetDlgItem( IDC_STATIC1 )->SetWindowText("TurboCoder R = 1/3");
		pdlgMain->GetDlgItem( IDC_STATIC11 )->SetWindowText("RSC1 R = 1/2");
		pdlgMain->GetDlgItem( IDC_STATIC13 )->SetWindowText("RSC2 R = 1/2");
  }

  if( codec.PermType == OURRND )
	  pdlgMain->GetDlgItem( IDC_BUTTON1 )->EnableWindow( TRUE );
  else
      pdlgMain->GetDlgItem( IDC_BUTTON1 )->EnableWindow( FALSE );

  pdlgMain->m_v1 = codec.v1;
  pdlgMain->m_Len = codec.N;
  pdlgMain->m_v2 = codec.v2;
  pdlgMain->m_punc = codec.bPunctured;
  pdlgMain->m_fixed = codec.bFixed;

  pdlgMain->UpdateData(FALSE); 

  pdlgMain->GetDlgItem( IDC_EDIT1 )->SetWindowText(OctToStr(codec.G1[0]));
  pdlgMain->GetDlgItem( IDC_EDIT3 )->SetWindowText(OctToStr(codec.G1[1]));
  pdlgMain->GetDlgItem( IDC_COMBO1 )->SetWindowText(GetPermName(codec.PermType)); 
  pdlgMain->GetDlgItem( IDC_EDIT5 )->SetWindowText(OctToStr(codec.G2[0]));
  pdlgMain->GetDlgItem( IDC_EDIT6 )->SetWindowText(OctToStr(codec.G2[1]));
}

TURBO_CODEC GetData()
{

  TURBO_CODEC codec;
  int i;

  pdlgMain->UpdateData(TRUE);

  i = pdlgMain->m_perm.GetCurSel();
  if( i == -1 )
	  i = 0;

  tc[i].v1 = pdlgMain->m_v1;
  tc[i].G1[0] = OctToDec( pdlgMain->m_back1 );
  tc[i].G1[1] = OctToDec( pdlgMain->m_forw1 );

  tc[i].N = pdlgMain->m_Len;
  tc[i].PermType = i;

  tc[i].v2 = pdlgMain->m_v2;
  tc[i].G2[0] = OctToDec( pdlgMain->m_back2 );
  tc[i].G2[1] = OctToDec( pdlgMain->m_forw2 );

  if( pdlgMain->m_punc )
     tc[i].bPunctured = true;
  else
     tc[i].bPunctured = false;

  tc[i].SNRInit = pdlgMain->m_init;
  tc[i].SNRStep = pdlgMain->m_step;
  tc[i].SNREnd = pdlgMain->m_end;

  tc[i].MaxNumOfIteration = pdlgMain->m_max;

  if( pdlgMain->m_fixed )
     tc[i].bFixed = true;
  else
	 tc[i].bFixed = false;

  memcpy( &codec, &tc[i], sizeof(TURBO_CODEC) );

  return codec;
} 


CString OctToStr( int oct )
{
  char str[10];

  itoa( oct, str, 8 );

  return (CString)str;
}

int OctToDec( int oct )
{

  int rem,i = 0,out = 0;

  do
  {
    rem = oct%10;

    out = out + rem*pow( 8.0, i );

	oct = oct/10;

	i++;

  }while( oct != 0 );

  return out;
}


CString GetTime()
{ 
  
  SYSTEMTIME SystemTime;
  CString ret,tmp;
 
  
  GetLocalTime( &SystemTime );

  ret.Format( _T("%d:"), SystemTime.wHour );

  tmp.Format( _T("%d:"), SystemTime.wMinute );
  if( tmp.GetLength() == 2 )
    tmp = "0" + tmp;
  ret = ret + tmp;

  tmp.Format(_T("%d"), SystemTime.wSecond  );
  if( tmp.GetLength() == 1 )
    tmp = "0" + tmp;
  ret = ret + tmp;

  return ret;
}

CString GetDate()
{
  SYSTEMTIME SystemTime;
  CString ret,tmp;
  
  GetLocalTime( &SystemTime );

  ret.Format( _T("%d-"), SystemTime.wDay );

  tmp.Format( _T("%d-"), SystemTime.wMonth );
  if( tmp.GetLength() == 2 )
    tmp = "0" + tmp;
  ret = ret + tmp;

  tmp.Format(_T("%d"), SystemTime.wYear  );
 
  ret = ret + tmp;
 
  return ret;
}

void SaveResults( TURBO_CODEC tc,  int size )
{
   f.Open( "results.txt", CFile::modeWrite | CFile::typeText );
   f.SeekToEnd();

   CString str;

   str.Format( _T("RSC1  v = %d  G = ("), tc.v1 );
   str = str + OctToStr(tc.G1[0]) + "," + OctToStr(tc.G1[1]) + ")";
   
   f.WriteString( str );
   f.WriteString( enter );

   str.Format( _T(" N = %d"), tc.N );
   str = GetPermName(tc.PermType) + " Перемежитель " + str;
   
   f.WriteString( str );

   str.Format( _T(" NS = %d"), NS );
   f.WriteString( str );

   str.Format( _T(" NumOfIteration = %d"), tc.MaxNumOfIteration );
   f.WriteString( str );
   f.WriteString( enter );

   str.Format( _T("RSC2  v = %d  G = ("), tc.v2 );
   str = str + OctToStr(tc.G2[0]) + "," + OctToStr(tc.G2[1]) + ")";

   f.WriteString( str );
   f.WriteString( enter );
   f.WriteString( enter );

   if( tc.bFixed )
   {
	   str.Format( _T(" SNR = %2.1f"), SNRFix );
	   str = "  Fixed Stream Of Errors for " + str;
   }
   else
       str.Format( _T("  SNR = %2.1f : %2.1f : %2.1f"), tc.SNRInit, tc.SNRStep, tc.SNREnd );
   f.WriteString( str );
   f.WriteString( enter );

   CString tmp;
   str = "  Pe = [ ";
   for( int i = 0; i < size; i++ )
   {
     tmp.Format( _T("%10.9f "), Pe[i] );
	 str = str + tmp;
   }
   str = str + "]";
   f.WriteString( str );

   f.WriteString( enter );
   f.WriteString( enter );
   f.Close();
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiplomaDlg dialog

CDiplomaDlg::CDiplomaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDiplomaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiplomaDlg)
	m_pe = 0.0;
	m_punc = FALSE;
	m_back1 = 0;
	m_forw1 = 0;
	m_v1 = 0;
	m_v2 = 0;
	m_back2 = 0;
	m_forw2 = 0;
	m_Len = 0;
	m_init = 1.0;
	m_step = 0.5;
	m_end = 3.5;
	m_max = 8;
	m_df = 0;
	m_fixed = FALSE;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDiplomaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiplomaDlg)
	DDX_Control(pDX, IDC_PROGRESS1, m_Line);
	DDX_Control(pDX, IDC_COMBO1, m_perm);
	DDX_Text(pDX, IDC_EDIT2, m_pe);
	DDX_Check(pDX, IDC_CHECK1, m_punc);
	DDX_Text(pDX, IDC_EDIT1, m_back1);
	DDX_Text(pDX, IDC_EDIT3, m_forw1);
	DDX_Text(pDX, IDC_EDIT4, m_v1);
	DDX_Text(pDX, IDC_EDIT7, m_v2);
	DDX_Text(pDX, IDC_EDIT5, m_back2);
	DDX_Text(pDX, IDC_EDIT6, m_forw2);
	DDX_Text(pDX, IDC_EDIT8, m_Len);
	DDX_Text(pDX, IDC_EDIT9, m_init);
	DDX_Text(pDX, IDC_EDIT11, m_step);
	DDX_Text(pDX, IDC_EDIT12, m_end);
	DDX_Text(pDX, IDC_EDIT10, m_max);
	DDX_Text(pDX, IDC_EDIT13, m_df);
	DDX_Check(pDX, IDC_CHECK2, m_fixed);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDiplomaDlg, CDialog)
	//{{AFX_MSG_MAP(CDiplomaDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDCANCEL2, OnCancel2)
	ON_BN_CLICKED(IDOK2, OnOk2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiplomaDlg message handlers

BOOL CDiplomaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
    TURBO_CODEC tc75;
    
	tc75.v1 = 2;
	tc75.G1[0] = 07;
	tc75.G1[1] = 05;
	tc75.N = 1000;
	tc75.v2 = 2;
	tc75.G2[0] = 07;
	tc75.G2[1] = 05;
	tc75.bPunctured = true;
	tc75.PermType = -1;
    tc75.MaxNumOfIteration = 8;

	int i;

	i = BERROU;

	tc[i].v1 = 4;
	tc[i].G1[0] = 037;
	tc[i].G1[1] = 021;
	tc[i].N = 1024;
	tc[i].v2 = 4;
	tc[i].G2[0] = 037;
	tc[i].G2[1] = 021;
	tc[i].bPunctured = true;

    i = CVETKOFF1;

	memcpy( &tc[i], &tc75, sizeof(TURBO_CODEC) );
    tc[i].N = 996;
	tc[i].bPunctured = false;

	i = CVETKOFF2;

	memcpy( &tc[i], &tc75, sizeof(TURBO_CODEC) );
    tc[i].N = 990;
	tc[i].bPunctured = false;
	
    i = GPP;
      
	tc[i].v1 = 3;
	tc[i].G1[0] = 013;
	tc[i].G1[1] = 015;
	tc[i].N = 1000;
	tc[i].v2 = 3;
	tc[i].G2[0] = 013;
	tc[i].G2[1] = 015;
	tc[i].bPunctured = false;

    i = DVBRCS;
      
	tc[i].v1 = 2;
	tc[i].G1[0] = 07;
	tc[i].G1[1] = 05; 
	tc[i].N = 880;
	tc[i].v2 = 2;
	tc[i].G2[0] = 07;
	tc[i].G2[1] = 05;
	tc[i].bPunctured = true;
	 
    i = RND;
	 
	memcpy( &tc[i], &tc75, sizeof(TURBO_CODEC) );

    i = OURRND;
    
	tc[i].v1 = 2;
	tc[i].G1[0] = 07;
	tc[i].G1[1] = 05;
	tc[i].N = 1000;
	tc[i].v2 = 2;
	tc[i].G2[0] = 07;
	tc[i].G2[1] = 05;

    tc[i].bPunctured = true;

    i = CCSDS;

	memcpy( &tc[i], &tc75, sizeof(TURBO_CODEC) );
    tc[i].N = 1784;

	i = SRND;
	 
	memcpy( &tc[i], &tc75, sizeof(TURBO_CODEC) );

    for( i = 0; i < NPERM; i++ )
	{
        m_perm.AddString(  GetPermName( i ) );
        tc[i].PermType = i;
		tc[i].MaxNumOfIteration = 8;
	}

    SetData( tc[RND] );

    info = new double[1];
	for( i = 0; i < NS; i++ )
	 {
	   data_err[i] = new double[1];
	   code1_err[i] = new double[1];
	   code1_err[i] = new double[1];
	   
	 } 

	int N = tc[OURRND].N;
	int *mas = new int[N];

    config.Open( "config.per",  CFile::modeReadWrite ); 
    config.Seek( 0, CFile::begin );
    config.Read( mas, sizeof( int )*N ); 

	ourperm.SetCurPattern( mas, N );

    delete[] mas;
    config.Close();

	// Make a header of the results.txt file
     CString frame,str;

     for( i = 0; i < 50; i++ )
		 frame = frame + "=";
	 enter.Format( _T("\n") );

     f.WriteString( frame );
     f.WriteString( enter );

	 f.WriteString( " DIPLOMA  v.0.4" );
     f.WriteString( enter );

	 f.WriteString( " @ACDISa" );
	 f.WriteString( enter );
   
     f.WriteString( " Copyright(c)SUAI 2005." );
	 f.WriteString( enter );

	 f.WriteString( frame );
	 f.WriteString( enter );
	 f.WriteString( enter );

	 str = "Начало работы: " + GetDate() + " " + GetTime(); 
	 f.WriteString( str );
	 f.WriteString( enter );
	 f.WriteString( enter );

	 f.Close(); 
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDiplomaDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDiplomaDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CDiplomaDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDiplomaDlg::OnCheck1() 
{
    UpdateData(TRUE);
	if( m_punc )
	{
		GetDlgItem( IDC_STATIC1 )->SetWindowText("TurboCoder R = 1/2");
		GetDlgItem( IDC_STATIC11 )->SetWindowText("RSC1 R = 2/3");
		GetDlgItem( IDC_STATIC13 )->SetWindowText("RSC2 R = 2/3");
	}
	else
	{
	    GetDlgItem( IDC_STATIC1 )->SetWindowText("TurboCoder R = 1/3");
		GetDlgItem( IDC_STATIC11 )->SetWindowText("RSC1 R = 1/2");
		GetDlgItem( IDC_STATIC13 )->SetWindowText("RSC2 R = 1/2");
	}
}

void CDiplomaDlg::OnSelchangeCombo1() 
{
	SetData(tc[m_perm.GetCurSel()]);	
}

void CDiplomaDlg::OnButton1() // Search
{
   TURBO_CODEC tc = GetData();

   pdlgMain->m_Line.SetPos(0);
   pdlgMain->m_Line.SetRange(0,NP); 
   pdlgMain->m_Line.SetStep(2);
   pdlgMain->m_Line.StepIt();

   pdlgMain->GetDlgItem( IDC_STATIC666 )->SetWindowText("Search patterns...");
   ourperm.GetPermPattern(tc.N); 
   
   pdlgMain->m_Line.SetStep(NP-2);
   pdlgMain->m_Line.StepIt();

   SetErrors( tc.N, 2.0 );	

   double tmp_pe = 1.0;
   int index;

   tc.bFixed = true;
   CString str;

   for( int i = 0; i < NP; i++ )
   {
	 str.Format( _T("NP = %d/%d  NS = %d"), i, NP, NS );
	 pdlgMain->GetDlgItem( IDC_STATIC666 )->SetWindowText(str);
	 ourperm.SetCurPattern( i );
     Modeling( tc, &ourperm, 1.0 );
		
	 if( Pe[0] < tmp_pe )
	 {
		 tmp_pe = Pe[0];
		 index = i;
	 }
	 delete[] Pe;
   }
   
   ourperm.SetCurPattern( index );
   ourperm.Clear(); // Clear temporary patterns

   int *mas = new int[tc.N];

   ourperm.GetCurPattern( mas );

   config.Open( "config.per",  CFile::modeReadWrite );
   config.Seek( 0, CFile::begin );
   config.Write( mas, sizeof( int )*tc.N ); 
   config.Seek( 0, CFile::begin );

   delete[] mas;
   config.Close();

   CString res;
   res.Format( _T("%5.4f"),tmp_pe);
   pdlgMain->GetDlgItem( IDC_EDIT2 )->SetWindowText(res);
 
   AfxMessageBox("Permutor is found");
}

void CDiplomaDlg::OnButton2() // Set Errors Stream
{

 TURBO_CODEC tc = GetData();
 pdlgMain->GetDlgItem( IDC_STATIC666 )->SetWindowText("Set Errors Stream...");
 pdlgMain->GetDlgItem( IDC_STATIC777 )->SetWindowText("");
 SetErrors( tc.N, tc.SNRInit );
}

void CDiplomaDlg::OnCancel2() // Compare Permutors
{

    TURBO_CODEC codec;
    double snr,NumOfSnr;
    Perm* perm;
	int i,k,j = 0;
    CString str;

    codec = GetData();

/* ------------ Set Permutors to Compare --------------------------- */

    const int NPERMCMP = 1; // Number of Permutors to Compare
	int PermNum[NPERMCMP] = { OURRND }; // Numbers of Permutors to Compare

    int N = codec.N;

	RandomPerm rndperm;
	//rndperm.GetPermPattern( N );
	SRandomPerm srndperm;
	//srndperm.GetPermPattern( N );
	BerrouGlavieuxPerm berperm;
    //berperm.GetPermPattern( N );
	GppPerm gppperm;
	//gppperm.GetPermPattern( N );
	ModNonRandomPerm modnonrnd;
    //modnonrnd.GetPermPattern( N );
	NonRandomPerm nonrnd;
    //nonrnd.GetPermPattern( N );
	DVBPerm dvb;
    //dvb.GetPermPattern( N );
	CCSDSPerm ccsds;
	//ccsds.GetPermPattern( N );

/* ---------------------------------------------------------------- */

    double perr[NPERMCMP][15]; // Pe for each Perm & SNR

	NumOfSnr = ((int)((codec.SNREnd - codec.SNRInit)/codec.SNRStep)) + 1;

    for( snr = codec.SNRInit; snr <= codec.SNREnd; snr = snr + codec.SNRStep ) 
	{
        pdlgMain->GetDlgItem( IDC_STATIC666 )->SetWindowText("Set Errors Stream");
		SetErrors( N, snr ); // Set SNRFix to snr

		for( k = 0; k < NPERMCMP; k++ ) 
		{ 
		
	      i = PermNum[k];

	      tc[i].bFixed = true;
          tc[i].bPunctured = true;
		  tc[i].MaxNumOfIteration = codec.MaxNumOfIteration;

		  switch( i )
		  {
		  case BERROU:
			  perm = &berperm;
			  break;
		  case RND:
			  perm = &rndperm;
	          break;
          case OURRND:
              perm = &ourperm; 
              break;
          case CVETKOFF1:
			  perm = &nonrnd;
	          break;
          case CVETKOFF2:
			  perm = &modnonrnd;
	          break;
          case GPP:
			  perm = &gppperm;
	          break;
          case DVBRCS:
			  perm = &dvb;
	          break;
		  case CCSDS:
	          perm = &ccsds;
	          break;
		  case SRND:
			  perm = &srndperm;
	          break;
		  } 
	
          Modeling( tc[i], perm, 1.0 );
		  
		  perr[k][j] = Pe[0];

		  delete[] Pe;

	    } // Perm 

        j++;

	  } // SNR

	  for( k = 0; k < NPERMCMP; k++ )
	  {
		Pe = perr[k];
		i = PermNum[k];
		tc[i].bFixed = false;
		tc[i].SNRInit = codec.SNRInit;
		tc[i].SNRStep = codec.SNRStep;
		tc[i].SNREnd = codec.SNREnd;
	    SaveResults( tc[i], NumOfSnr );
	  } 

}

void CDiplomaDlg::OnOk2() // Min. Distance
{
   	TURBO_CODEC codec;

	CString str;

    Perm* perm;

    codec = GetData();
   
    RSC rsc1,rsc2;

    rsc1.Set( codec.v1, codec.G1  );
    rsc2.Set( codec.v2, codec.G2 );

	int N = codec.N;

    switch( codec.PermType )
	{
		  case BERROU:
			  perm = new BerrouGlavieuxPerm; 
	          break;
		  case RND:
	          perm = new RandomPerm;
	          break;
          case OURRND:
              perm = &ourperm; 
              break;
          case CVETKOFF1:
	          perm = new NonRandomPerm;
	          break;
          case CVETKOFF2:
	          perm = new ModNonRandomPerm;
	          break;
          case GPP:
              perm = new GppPerm;
	          break;
          case DVBRCS:
	          perm = new DVBPerm;
	     	  break;
		  case SRND:
	          perm = new SRandomPerm;
	          break;
		 case CCSDS:
	          perm = new CCSDSPerm;
	          break;
	} 

	if( codec.PermType != OURRND )
	   perm->GetPermPattern( N );

    int df = 10000;
	double *data = new double[N+2*MAXV];
    int i,w = 0;
	int i1,i2;

   	for( i = 0; i < N; i++ )
	{
	  data[i] = 0;
	}

    pdlgMain->m_Line.SetPos(0);
    pdlgMain->m_Line.SetRange(0,codec.N); 
    pdlgMain->m_Line.SetStep(1);
  
    pdlgMain->GetDlgItem( IDC_STATIC666 )->SetWindowText("w = 1");
	pdlgMain->GetDlgItem( IDC_STATIC777 )->SetWindowText("");

/* ----------------- Sequence with weight = 1 ----------------------- */
	for( i = 0; i < N; i++ )
	{
	  data[i] = 1;
	 
      w = GetTurboWeight( codec, perm, data, &rsc1, &rsc2 );
	  
      if( w < df )	  df = w;

	  data[i] = 0;
	}

    pdlgMain->m_Line.StepIt();
    
/* -------------------------------------------------------------------------- */


/* ----------------- Sequence with weight = 2  for (7,5) 10x01 -------------- 

    int limit,tmp,n;

	
    pdlgMain->GetDlgItem( IDC_STATIC666 )->SetWindowText("w = 2");

    for( i1 = 0; i1 < codec.N - 3; i1++ ) // Moving of first "1" 
	{
	  data[i1] = 1;
      limit = (int)((codec.N - i1)/3);
	  tmp = i1 + 3;
	  
	  for( n = 0; n < limit; n++ ) // Moving of second "1" 
	  {
		i2 = tmp + 3*n;
	    data[i2] = 1;
	    w = GetTurboWeight( codec, perm, data, &rsc1, &rsc2 );
	    if( w < df ) df = w;
        data[i2] = 0;
	  }
     
	  data[i1] = 0;
	 
	  str.Format( _T(" w = 2 i1 = %d"), i1 );
	  pdlgMain->GetDlgItem( IDC_STATIC666 )->SetWindowText(str);
      pdlgMain->m_Line.StepIt();
	} 

/* -------------------------------------------------------------------------- */


/* ------------ All Sequence with weight = 2 -------------------------- */

    pdlgMain->GetDlgItem( IDC_STATIC666 )->SetWindowText("w = 2");

    for( i1 = 0; i1 < codec.N -1; i1++ ) // Moving of first "1" 
	{
	  data[i1] = 1;

	  
	  for( i2 = i1 + 1; i2 < codec.N; i2++ ) // Moving of second "1" 
	  {
	    data[i2] = 1;
	    w = GetTurboWeight( codec, perm, data, &rsc1, &rsc2 );
	    if( w < df ) df = w;
	
	    data[i2] = 0;
	
	  }
     
	  data[i1] = 0;
	 
	  str.Format( _T(" w = 2 i1 = %d"), i1 );
	  pdlgMain->GetDlgItem( IDC_STATIC666 )->SetWindowText(str);
	  pdlgMain->m_Line.StepIt();
	} 

/* -------------------------------------------------------------------------- */

	
/* ----------------- Sequence with weight = 1+w(tail) --------------------- */

    pdlgMain->GetDlgItem( IDC_STATIC666 )->SetWindowText("w = 1 + w(tail)");

   	int wtail = 0;
	data[codec.N-1] = 1;
	w = GetTurboWeight( codec, perm, data, &rsc1, &rsc2 );
	w = 0;
	int v1 = codec.v1,k = 0,j = 0;
	double tail[MAXV];
	for( i = codec.N; i < codec.N+v1; i++ ) 
	{
       wtail = wtail + data[i];
	   tail[k] = data[i];
	   k++;
	}

	for( i = 0; i < codec.N; i++ )  data[i] = 0;
	
    for( i = 0; i < codec.N - v1; i++ )
	{
	  data[i] = 1;
	  j = 0;
	  for( k = i+1; k < i+1+v1; k++ ) 
	  {
		  data[k] = tail[j];
		  j++;
	  }
	  w = GetTurboWeight( codec, perm, data, &rsc1, &rsc2 );
	  if( w < df ) df = w;
	  
	  for( k = i; k < i+1+v1; k++ ) data[k] = 0; 
	}

/* ---------------------------------------------------------- */  

	pdlgMain->m_Line.StepIt();

    pdlgMain->m_df = df;
	pdlgMain->UpdateData( FALSE );

	delete[] data;
}

int GetWeight( double mas[], int len )
{
  double w = 0.0;
  for( int i = 0; i < len; i++ ) 
       w = w + mas[i];

  return (int)w;
}

int GetTurboWeight( TURBO_CODEC tc, Perm* perm, double* data, RSC* rsc1, RSC* rsc2 )
{

  int N = tc.N;

  double *code1 = new double[N+MAXV]; 
  double *code2 = new double[N+MAXV]; 
  
  turbocoder( rsc1, perm, rsc2, data, N, code1, code2 );

  int w =  GetWeight( data, N );
  w = w + GetWeight( code1, N );
  w = w + GetWeight( code2, N );

  delete[] code1;
  delete[] code2;

  return w;


}
