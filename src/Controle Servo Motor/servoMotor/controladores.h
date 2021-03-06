#define kp 1.0651
#define kd 1.0246
#define ki 0.0011
#define kw 1
#include <math.h>
#define pi 3.1415
double dErro_dt (double erroAtual)
{
  static        double   erroAnterior      = 0;
  double dErro = (erroAtual - erroAnterior);
  erroAnterior   = erroAtual;
  return dErro;
}

double iErro_dt (double erroAtual)
{
  static        double  iErro             = 0;
  iErro += erroAtual;
  return iErro;
}

int controlador_PID(double erro, double ScA)
{
  static double anti_windup   =      0; //Valor inicial do anti-windup
  #ifdef ERROP
    double erroP;//Erro permissivel
    if ((erro<1)&&(erro>-1)) erroP=0;
    else                     erroP=erro;
    double Sc= kp*erroP + kd*dErro_dt(erro) + ki*iErro_dt(erroP+anti_windup*kw);//Não pode passar erroP para o derivativo
  #else
    double Sc= kp*erro + kd*dErro_dt(erro) + ki*iErro_dt(erro+anti_windup*kw);
  #endif
  anti_windup = ScA - Sc;
  return Sc;
    
    
    
}


int avanc_atra_calc(int erro){
  double u;
  int ua=0, erro_a=0;
  u=0.9025*erro-0.8084*erro_a+0.8114*ua;
  ua=(int)u;
  erro_a=erro;
  return (int) u;
}






/* |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-*/







int controlador_SS(double referencia, double saidaPlanta, double ScA)//ScA tem que ser SATURADO !
{
  static double EstadosA[2]   = {0, 0};  //Vetor de estados estimados
  static double integrador    =      0; //Valor do integrador
  static double anti_windup   =      0; //Valor do anti-windup

  const  double A[2][2] = {{0.704294786861096 , 0 } , { 0.016870530849940 , 1 }};//Matriz A
  const  double B[2]    = {0.269928493599033        ,   0.002856672307105     }; //Matriz B
  const  double C[2]    = {0                        ,   9.163685739927198     }; //Matriz C
  const  double L[2]    = {0.610653949032228        ,   0.097788322092752     }; //Ganho do Obserdavor
  const  double K[2]    = {2.741950191395121        ,  65.124363946536870     }; //Ganho Realimentação de Estados
  const  double Ka      =                               0.913877825841094/2      ; //Ganho do Integrador
  const  double Kw      =                               0                    ; //Ganho do Anti-WindUp

  double saidaEstimada  = C[0]*EstadosA[0]+C[1]*EstadosA[1];
  EstadosA[0]           = A[0][0]*EstadosA[0]+A[0][1]*EstadosA[1]       +        B[0]*ScA        +        L[0]*(saidaPlanta-saidaEstimada); //Observador
  EstadosA[1]           = A[1][0]*EstadosA[0]+A[1][1]*EstadosA[1]       +        B[1]*ScA        +        L[1]*(saidaPlanta-saidaEstimada); //Observador
         
  integrador += (referencia - saidaPlanta) + anti_windup*Kw;      //Integra o erro em função do tempo e soma ação anti-windup
  integrador = min(max(integrador,-10000),10000);
  double Sc = integrador*Ka - K[0]*EstadosA[0] -  K[1]*EstadosA[1];      //Controlador retorna Sc
  anti_windup = ScA - Sc;
  //Serial.print(EstadosA[0]);
  //Serial.print("\t");
  //Serial.print(EstadosA[1]);
  //Serial.print("\t");
  //Serial.print(referencia - saidaPlanta);
  //Serial.print("\t");
  //Serial.print(integrador);
  //Serial.print("\t");
  //Serial.print(anti_windup);
  //Serial.print("\t");
  return (int)Sc;
}
