///////////////////////////////////////////////////////////////////////////////
////     PROGRAMA PARA MÓDULO CONTROLADOR DE IRRIGAÇÃO   v.1.1       //////////
////     ALAN KARDEK RÊGO SEGUNDO                                    //////////
///////////////////////////////////////////////////////////////////////////////

// DEFINIÇÃO DAS VARIÁVEIS INTERNAS DO PIC - arquivo fornecido pela microchip
#include   <18f25K80.h>  
//declaração de fuses apas para simulação
//#fuses HSH, NOWDT, PROTECT, NOPUT, NOBROWNOUT, CPD, NOWRT, NOMCLR
#priority rda, timer0

// incluir constante aqui
//#define LOADER_END 0x4FF  //DEFINIR VALOR GRANDE E OBSERVAR A MEMÓRIA ROM. IR ABAIXANDO ATÉ O PROGRAMA COMPILAR.
#FUSES WDT1024                   //Watch Dog Timer uses 1:128 Postscale
#FUSES VREGSLEEP_SW             //Ultra low-power regulator is enabled - Economiza mais energina no modo sleep. Requer cap de 10uF em VDDCORE
#FUSES INTRC_LP                 //LF-INTOSC in Low-Power mode during Sleep
#FUSES SOSC_DIG                 //Low-power SOSC circuit is selected
//#FUSES NOXINST                  //Extended set extension and Indexed Addressing mode disabled (Legacy mode)
#FUSES INTRC_IO                 //Internal RC Osc, no CLKOUT
#FUSES BROWNOUT                 //No brownout reset
#FUSES WDT_NOSLEEP              //Watch Dog Timer, disabled during SLEEP
#FUSES NOMCLR                   //Master Clear pin used for I/O
#FUSES NOPLLEN    
//#include <bootloader.h>

#include <PIC18F25K80_registers.h>


//#include <bootloader.h>
#use delay(clock=16M, restart_wdt)


#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

//#use I2C(MASTER, sda=PIN_C4, scl=PIN_C3, STREAM = DS1307_STREAM, FORCE_HW)
#use I2C(MASTER,scl=PIN_C3, sda=PIN_C4,FORCE_HW)
//#use I2C(MASTER, sda=PIN_C4, scl=PIN_C3, FORCE_HW)

#bit RBPU = getenv("BIT:RBPU")
#byte WPUB = getenv("SFR:WPUB") 

// Constantes
#define t_filtro 5     // filtro para tratamento do bounce dos botÃƒÂµes
#define n_sen 1         // nÃƒÂºmero de sensores
#define n_tentativa 2   //(n+1) tentativas de transmissÃƒÂ£o de dadossem fio
#define amostragem 1    // altera tempo de pedido dos sinais dos sensores (seg.)

// biblioteca 
//#include <usb_cdc.h>


float //leitura dos sensores
   sensor[4]={5 5 5 5};
// VariÃƒÂ¡veis 
static int
   // XBee API
   int8 sd = 0x7E, msb = 0, lsb = 6, ft = 1, fid = 1, ad_msb = 0, ad_lsb = 0, op = 0, data = 0x00, cs = 0x00,
   

   // contadores
   i,
   j,
   m,// endereÃƒÂ§o do sensor e posiÃƒÂ§ÃƒÂ£o do vetor que armazena os dadosdos sensores
   // variÃƒÂ¡veis relacionadas com os botÃƒÂµes
   pagina=0, programa=0,
   filtro=t_filtro, filtro2=t_filtro, filtro3=t_filtro,filtro4=t_filtro, 
   filtro6=t_filtro,     //filtros dos botÃƒÂµes
   // variÃƒÂ¡veis relacionadas com tempo e data
   posicao0, ajuste_data, dia_1=1, mes_1=1, ano_1=23, dia_da_semana=1, cont_val, hora_sup=0, hora_inf=0, segundos,
   minutos, horas, cont_semana,dia_teste,mes_teste,ano_teste,
   tempo_lcd, h_val[4], m_val[4], mt_val[4], ht_val[4], hi_val, mi_val, seg,  
   min_temp,
   // relacionadas com a navegaÃƒÂ§ÃƒÂ£o no lcd
   posicao1, posicao6;
   // outras variÃƒÂ¡veis
   
static long int // inteiro de 16 bits
   t=0, cont=0, limite_sup=0,
   limite_inf=0;
static  boolean  // flags
   // relacionadas com as vÃƒÂ¡lvulas
   val[4]={0 0 0 0},   
   // relacionadas com a navegaÃƒÂ§ÃƒÂ£o no lcd
   posicao2, posicao3, posicao4, posicao5, 
   // relacionadas com os botÃƒÂµes
   botao_direito, botao_esquerdo, botao_acima, botao_abaixo, botao_aum_pagina,
   // relacionadas com os dias da semana
   dia[7]={1 1 1 1 1 1 1}, 
   // outras
   leu = 0;
  
char 
   dia_da_semana_str[3],
   entrada[80], // vetor que armazena os dados de entrada da transmissÃƒÂ£o serial
   *ptr;    //ponteiro utilizado na funÃƒÂ§ÃƒÂ£o de conversÃƒÂ£o de inteiro para caracter   
   
// Modo de inicializaÃƒÂ§ÃƒÂ£o das portas do PIC
#use   fast_io(a)
#use   fast_io(b)
#use   fast_io(c)



// EndereÃƒÂ§o das portas do PIC
#byte   porta = 0xf80
#byte   portb = 0xf81
#byte   portc = 0xf82


// AssociaÃƒÂ§ÃƒÂ£o dos pinos das portas com nomes para facilitar programaÃƒÂ§ÃƒÂ£o
#bit bomba = porta.0    // relÃƒÂ© do conjunto moto-bomba
#bit val1 = porta.1    // relÃƒÂ© da vÃƒÂ¡lvula 1
#bit val2 = porta.2     // relÃƒÂ© da vÃƒÂ¡lvula 2
#bit val3 = porta.3     // relÃƒÂ© da vÃƒÂ¡lvula 3
#bit val4 = porta.5    // relÃƒÂ© da vÃƒÂ¡lvula 4
#bit esquerdo = portb.3      // botÃƒÂ£o "esquerdo"
#bit acima = portb.5   // botÃƒÂ£o "cima"
#bit liga = portb.6    // botÃƒÂ£o "liga"
#bit abaixo = portb.4   // botÃƒÂ£o "baixo"
#bit aum_pagina = portb.7     // botÃƒÂ£o para aumentar pÃƒÂ¡gina
//#bit dim_pagina = portb.6    // botÃƒÂ£o para diminuir pÃƒÂ¡gina


// ARQUIVOS INCLUÃƒÂDOS

#include <stdlib.h>     // biblioteca com funÃƒÂ§ÃƒÂµes de tratamento de strings
#include   <lcdi2c.c>
#include <DS1307.c>    // biblioteca do relÃƒÂ³gio de tempo real DS1307

RTC_Time *mytime;


// FunÃƒÂ§ÃƒÂ£o que transforma um inteiro em um vetor de caracteres de quatro posiÃƒÂ§ÃƒÂµes
char * inttochar4(long int inteiro,char *c)
{
   char t[4];
   itoa(inteiro,10,t);
   if (inteiro<10)
   {
      c[0]='0';
      c[1]='0';
      c[2]='0';
      c[3]=t[0];
   }
   if (inteiro>9)
   {
      c[0]='0';
      c[1]='0';
      c[2]=t[0];
      c[3]=t[1];      
   }
   if (inteiro>99)
   {
      c[0]='0';
      c[1]=t[0];
      c[2]=t[1];
      c[3]=t[2];      
   }
   if (inteiro>999)
   {
      c[0]=t[0];
      c[1]=t[1];
      c[2]=t[2];
      c[3]=t[3];      
   }
   return c;
}

#int_timer0
void trata_tmr0 ()
{
   set_timer0(131 + get_timer0());
   cont++;     
   if(cont>499)   // se "cont" ÃƒÂ© igual a 625, logo passou 1 segundo. (consertar para cristal interno de 16MHz)
   { 
      cont=0;
      seg++;  
      if (seg>29)
      {
         seg = 0;
         // envia status sobre o sistema
         // estado da bomba
         // valvula 1
         // valvula 2
         // valvula 3
         // valvula 4
         // programa
         // minuto valvula 1
         // hora valvula 1
         // minuto valvula 2
         // hora valvula 2
         // minuto valvula 3
         // hora valvula 3
         // minuto valvula 4
         // hora valvula 4
         // minuto de ligar
         // hora de ligar
         // limite inf sensor
         // limite sup sensor
         // hora inicial nÃƒÂ£o funcionar
         // hora final nÃƒÂ£o funcionar
         
       msb = 0;
       lsb = 25;      
       ad_msb = 0;
       ad_lsb = 0;
       ft = 1; 
       fid = 0; // nÃƒÂ£o recebe status
       
       cs = 0xFF - make8(ft + fid + ad_msb + ad_lsb + op + bomba + val[0] + 
       val[1] + val[2] + val[3] + hi_val + mi_val + programa + h_val[0] +
       h_val[1] + h_val[2] + h_val[3] +  m_val[0] + m_val[1] + m_val[2] + 
       m_val[3] + limite_inf + limite_sup + hora_inf + hora_sup, 0);
       
       putc(sd);
       putc(msb);
       putc(lsb);
       putc(ft);
       putc(fid);
       putc(ad_msb);
       putc(ad_lsb);
       putc(op);      
       putc(bomba);
       putc(val[0]);
       putc(val[1]);
       putc(val[2]);
       putc(val[3]);
       putc(hi_val);
       putc(mi_val);
       putc(programa);
       putc(h_val[0]);
       putc(h_val[1]);
       putc(h_val[2]);
       putc(h_val[3]);
       putc(m_val[0]);
       putc(m_val[1]);
       putc(m_val[2]);
       putc(m_val[3]);
       putc(limite_inf);
       putc(limite_sup);
       putc(hora_inf);
       putc(hora_sup);
       putc(cs);
         
      }
   }      
}

void atualizaModulo(byte dados[])
{
   
         // liga bomba e vÃƒÂ¡lvula 1   
         switch (dados[8])
         {
         case 111:
         {
            bomba = 1;
            val[0] = 1;
            val[1] = 0;
            val[2] = 0;
            val[3] = 0;
            i = 0;
         }    
         break;
         // liga bomba e vÃƒÂ¡lvula 2   
         case 112:
         {
            bomba = 1;
            val[0] = 0;
            val[1] = 1;
            val[2] = 0;
            val[3] = 0;
            i = 1;
         }   
         break;
         // liga bomba e vÃƒÂ¡lvula 3   
         case 113:
         {
            bomba = 1;
            val[0] = 0;
            val[1] = 0;
            val[2] = 1;
            val[3] = 0;
            i = 2;
         }   
         break;
         // liga bomba e vÃƒÂ¡lvula 4   
         case 114:
         {
            bomba = 1;
            val[0] = 0;
            val[1] = 0;
            val[2] = 0;
            val[3] = 1;
            i = 3;
         }   
         break;
         // desliga bomba e vÃƒÂ¡lvulas  
         case 117:
         {
            bomba = 0;
            val[0] = 0;
            val[1] = 0;
            val[2] = 0;
            val[3] = 0;            
            i = 0;
            programa = 0;
         }   
         break;
      }
         // mudanÃƒÂ§a de programa quando a bomba estiver desligada
         // manual
         if (!bomba)
         {
            switch (dados[8])
            {
            case 120: 
            {
               programa = 0;
            }
            break;
         // timer
            case 121:
            {
               programa = 1;
            }
            break;
         // auto
            case 122:   
            {
               programa = 2;
            }
            break;
         // smart
            case 123:
            {
               programa = 3;
            }                 
            break;
            }
         }
         if(dados[8] < 100)
         {
           // sensor[0] = dados[8];
         }
      
}

int8 day_of_week(int8 day,int8 mth,int8 yr) 
{
   int8 val;
   const int8 table[12]= {6,2,2,5,0,3,5,1,4,6,2,4}; 
   //// yr is last two digits Ex 2007 is 07 
   ///  mth is month Jan=1.....Dec=12 
   val=yr+yr/4; /// leap year adj good 2007 to 2099 
   val=val+table[mth-1]; /// table contains modulo 7  adjustments for mths 
   val=val+day; /// actual day 1..31 
   if((yr%4==0) &&(mth<3))val=val-1; /// adjust jan and feb down one for leap year 
   val=val%7;  /// modulo 7 
   if(val==0)val=7; 
   
   /// val is now the day of week 1=Mon .......7=Sun 
   return(val); 
}

// RECOMPÃƒâ€¢E VARIÃƒÂVEL DE 16 BITS (SIGNED INT)
signed long combine(int byte1, int byte2) {
  // This code assumes that byte1 is in range, but allows for the possibility
  // that the values were originally in a signed char and so is now negative.
  if (byte1 >= 128) byte1 -= 256;
  return byte1 * 256 + byte2;
}


// InterrupÃƒÂ§ÃƒÂ£o da porta serial
//////////ALTERAR PARA SWICH CASE////////////



  //restart_wdt();
  //leu=1;                // flag que indica o inÃƒÂ­cio da leitura
  //entrada[0]=getc();    // sd
  //entrada[1]=getc();    // msb
  //entrada[2]=getc();    // lsb
  
  // RECEPÃƒâ€¡ÃƒÆ'O DE DADOS

#int_rda
void rda_isr()
{//7E 00 __ 81 00 ou 7E 00 __ 01 00  
   while(RCIF==0); // Wait until RCIF gets low
   entrada[0] = RXREG1;
   if (entrada[0] == 0x7E)
   {
      while(RCIF==0); // Wait until RCIF gets low
      entrada[1] = RXREG1;
      while(RCIF==0); // Wait until RCIF gets low
      entrada[2] = RXREG1;
      for (int i = 3; i < (entrada[2] + 4); i++)
      {
         while(RCIF==0); // Wait until RCIF gets low
         entrada[i] =  RXREG1;
      }  
      leu = 1;  
   }
   else
   {
      return;
   }


  switch (entrada[2])
  {
     case 77:
     {
        sensor[0] = (float) (combine(entrada[32], entrada[33])) / 10.0;
     }
     break;

     case 6:
     {
        atualizaModulo(entrada);  // incluir funÃƒÂ§ÃƒÂ£o para passar vetor
     }
     break;
     // verificaÃƒÂ§ÃƒÂ£o do tamanho do pacote (20 bytes)
     // atualiza status do sistema
     case 25:
     {
        //entrada[3]=getc();    // ft
        //entrada[4]=getc();    // fid
        //entrada[5]=getc();    // ad_msb
        //entrada[6]=getc();    // ad_lsb
        //entrada[7]=getc();    // op
        
        bomba = entrada[8];
        val[0] = entrada[9];
        val[1] = entrada[10];
        val[2] = entrada[11];
        val[3] = entrada[12];
        hi_val = entrada[13];
        mi_val = entrada[14];
        programa = entrada[15];
        h_val[0] = entrada[16];
        h_val[1] = entrada[17];
        h_val[2] = entrada[18];
        h_val[3] = entrada[19];
        m_val[0] = entrada[20];
        m_val[1] = entrada[21];
        m_val[2] = entrada[22];
        m_val[3] = entrada[23];
        limite_inf = entrada[24];
        limite_sup = entrada[25];
        hora_inf = entrada[26];
        hora_sup = entrada[27];
        entrada[28] = getc();         
     }
     break;   
  }
  leu=0;       // flag que indica o fim da leitura 
}


// liga bomba e valvula 0
void ligaBomba()
{
       msb = 0;
       lsb = 6;      
       ad_msb = 0;
       ad_lsb = 0; // endereÃƒÂ§o do computador 0x00
       
       if (val[0])
       {
         data = 0x11;
       }
       if (val[1])
       {
         data = 0x12;
       }
       if (val[2])
       {
         data = 0x13;
       }
       if (val[3])
       {
         data = 0x14;
       }
       
       ft = 1;
       fid = 0; // nÃƒÂ£o recebe o status
       cs = 0xFF - make8(ft+fid+ad_msb+ad_lsb+op+data,0);            
       putc(sd);
       putc(msb);
       putc(lsb);
       putc(ft);
       putc(fid);
       putc(ad_msb);
       putc(ad_lsb);
       putc(op);      
       putc(data);         
       putc(cs);
      
      //teste
      /*
      lcd_gotoxy(1,1);         // posiciona o cursor na linha 0, coluna 0
      printf(lcd_putc,"%X %X %X %X %X", sd, msb, lsb, ft, fid);
      lcd_gotoxy(1,2);        
      printf(lcd_putc,"%X %X %X %X %X", ad_msb, ad_lsb, op, data, cs);
      delay_ms(1000);
      */


}

// desliga bomba e valvulas
void desligaBomba()
{
       msb = 0;
       lsb = 6;      
       data = 0x17;          
       ad_msb = 0;
       ad_lsb = 0;
       ft = 1; 
       fid = 0; // nÃƒÂ£o recebe status
       cs = 0xFF - make8(ft+fid+ad_msb+ad_lsb+op+data,0);            
       putc(sd);
       putc(msb);
       putc(lsb);
       putc(ft);
       putc(fid);
       putc(ad_msb);
       putc(ad_lsb);
       putc(op);      
       putc(data);         
       putc(cs);
      
      //teste
      /*
      lcd_gotoxy(1,1);         // posiciona o cursor na linha 0, coluna 0
      printf(lcd_putc,"%X %X %X %X %X", sd, msb, lsb, ft, fid);
      lcd_gotoxy(1,2);        
      printf(lcd_putc,"%X %X %X %X %X", ad_msb, ad_lsb, op, data, cs);
      delay_ms(1000);
      */

}
void programaModo()
{
   msb = 0;
       lsb = 6;   
       // Manual
       if (programa == 0)
       {
         data = 0x20;          
       }
       // Timer
       if (programa == 1)
       {
         data = 0x21;          
       }
       // Automatico
       if (programa == 2)
       {
         data = 0x22;          
       }
       // Smart
       if (programa == 3)
       {
         data = 0x23;          
       }
       ad_msb = 0;
       ad_lsb = 0;
       ft = 1; 
       fid = 0; // nÃƒÂ£o recebe status
       cs = 0xFF - make8(ft+fid+ad_msb+ad_lsb+op+data,0);            
       putc(sd);
       putc(msb);
       putc(lsb);
       putc(ft);
       putc(fid);
       putc(ad_msb);
       putc(ad_lsb);
       putc(op);      
       putc(data);         
       putc(cs);
}
void main() {
   setup_oscillator(OSC_16MHz);
   // configura os TRIS
   set_tris_a(0b00000000);      // configuraÃƒÂ§ÃƒÂ£o dos pinos de I/O
   set_tris_b(0b11111000);
   set_tris_c(0b10011000);
     
   // inicializa as portas
   porta=0x00;         // limpa porta a
   portb=0x00;         // limpa porta b
   portc=0x00;         // limpa porta c
   
   RBPU = 0;  // habilitando pull-up da porta B
   WPUB = 0b11111000; //ativa pull-up dos pinos RB7 RB6 RB4 RB5 RB3
 
   // inicializa lcd
   lcd_init(0x4E);
   delay_ms(100);
   // inicializa ds1307
   //ds1307_init();
   mytime = RTC_Get();
   OSC_Enable();
   SQWE_Set(OUT_1Hz);
   delay_ms(500);
   //ds1307_get_time(horas,minutos,segundos);
   //delay_ms(100);
   //ds1307_get_date(dia_1, mes_1, ano_1, dia_da_semana);
   //delay_ms(100);
   // configuraÃƒÂ§ÃƒÂ£o do TIMER0
   setup_timer_0 (RTCC_INTERNAL | RTCC_DIV_64 | RTCC_8_BIT);
   set_timer0(131);
   
   // configuraÃƒÂ§ÃƒÂ£o das interrupÃƒÂ§ÃƒÂµes
   enable_interrupts (int_timer0);
   enable_interrupts (INT_RDA);
   enable_interrupts(GLOBAL);
   
   // carrega dados de configuraÃƒÂ§ÃƒÂ£o gravados na memÃƒÂ³ria EEPROM
   hi_val   = read_eeprom(0);
   mi_val   = read_eeprom(1);
   programa  = read_eeprom(2);    
   h_val[0] = read_eeprom(3);  
   h_val[1] = read_eeprom(4);
   h_val[2] = read_eeprom(5);
   h_val[3] = read_eeprom(6);
   m_val[0] = read_eeprom(7);
   m_val[1] = read_eeprom(8);
   m_val[2] = read_eeprom(9);
   m_val[3] = read_eeprom(10);
   limite_inf = read_eeprom (11);
   limite_sup = read_eeprom (12);
   hora_inf = read_eeprom (13);
   hora_sup = read_eeprom (14);
   dia[0]   =  read_eeprom(15);   
   dia[1]   =  read_eeprom(16);   
   dia[2]   =  read_eeprom(17);   
   dia[3]   =  read_eeprom(18);   
   dia[4]   =  read_eeprom(19);   
   dia[5]   =  read_eeprom(20);   
   dia[6]   =  read_eeprom(21);   


   // AnimaÃƒÂ§ÃƒÂ£o inicial do programa
   // Nome do programa no LCD
   {
   const char line1[] = "SIST. AUTOMATICO";
   lcd_gotoxy(1, 1);
   for (int x = 0; line1[x] != '\0'; x++) {
     printf(lcd_putc, "%c", line1[x]);
     delay_ms(100);
   }
   const char line2[] = "DE IRRIGACAO";
   lcd_gotoxy(1, 2);
   for (int y = 0; line2[y] != '\0'; y++) {
     printf(lcd_putc, "%c", line2[y]);
     delay_ms(100);
   }
   delay_ms(1000);
   lcd_gotoxy(1, 1);
   lcd_putc("                ");
   lcd_gotoxy(1,2);
   lcd_putc("                ");
   }
  
   // Loop principal
   while(TRUE) 
   {
     RESTART_WDT();    // dispositivo que reinicia o programa caso ele trave
///////////////////////////////////////////////////////////////////////////////   
//1. TELA INICIAL -> EXECUÃƒâ€¡ÃƒÆ'O, RELÃƒâ€œGIO E OPÃƒâ€¡ÃƒÆ'O MANUAL/TIMER/AUTOMATICO/SMART //
///////////////////////////////////////////////////////////////////////////////
      SWITCH(pagina)
      {
         case 0:
         // variÃƒÂ¡vel para verificar se passou um minuto
         min_temp = minutos;
         
         // carrega informaÃƒÂ§ÃƒÂµes do DS1307         
         //ds1307_get_time(horas,minutos,segundos);
         mytime = RTC_Get();
         horas = mytime->hours;
         minutos = mytime->minutes;
         segundos = mytime->seconds;
         
         // decremento no tempo das vÃƒÂ¡lvulas apos acionamento
         if (minutos > min_temp)    // passou um minuto?
         {  // sim
            if(programa&&val[i]&&bomba)    // o programa ÃƒÂ© diferente do manual,           
            {                            // a valvula e a bomba estÃƒÂ£o acionadas?
               m_val[i]--;
               if(m_val[i]==255&&h_val[i])   // decrementa hora apÃƒÂ³s 0 min.
               {
                  m_val[i]=59;
                  h_val[i]--;
               }             
            }                                    
         }
         
         // escreve informaÃƒÂ§ÃƒÂµes no LCD
         lcd_gotoxy(1,1);         // posiciona o cursor na linha 0, coluna 0
         printf(lcd_putc,"%1u.%02u:%02u   ", pagina+1, horas, minutos);
         lcd_gotoxy(1,2); 
         printf(lcd_putc,"St%1u ", i+1);
         
         // escreve informaÃƒÂ§ÃƒÂµes do sensor
         // se nÃƒÂ£o houver sensor, imprime "---".
         if (i < n_sen)
         {
            lcd_gotoxy(5,2);
            printf (lcd_putc,"%04.1f", sensor[i]);
            lcd_putc(0x25); //%            
         }
         else 
         {
            lcd_gotoxy(5,2);
            printf (lcd_putc,"---");
         }
         
         // escreve tempo restante de cada vÃƒÂ¡lvula
         if (!programa)
         {
            lcd_gotoxy(10,2); 
            printf (lcd_putc,"     ");
         }
         else
         {
            lcd_gotoxy(12,2); 
            printf (lcd_putc,"%02u:%02u", h_val[i],m_val[i]);
         }
         // estado das valvulas
            lcd_gotoxy(10,2); 
            printf (lcd_putc,"%1u", val[i]);
                
         //CONTROLE DO BACKLITH LCD   
         if(!liga||!esquerdo||!acima||!abaixo||!aum_pagina)
         {
            tempo_lcd=minutos;
           // //lcd=1
         }
         if(minutos==(tempo_lcd+2))
         {
           // //lcd=0
            pagina=0;
         } 
         // Hora de ligar a bomba para os modos timer e auto
         // se estiver no programa timer ou auto, se for dia da semana de ligar, 
         // se horas e minutos forem iguais a hora de ligar, se o conjunto moto-
         // bomba estiver desligado, se o tempo de todas as vÃƒÂ¡lvulas for 
         // diferente de zero, entÃƒÂ£o, liga o conjunto moto-bomba.
         if(((programa==1)||(programa==2))&&dia[cont_semana-1]&&horas==hi_val&&
         minutos==mi_val&&!bomba&&(m_val[0]||h_val[0]||m_val[1]||h_val[1]
         ||m_val[2]||h_val[2]||m_val[3]||h_val[3]))
         {
               bomba=1;     
         }
         
         // Relaciona as vÃƒÂ¡lvulas com o vetor val[] e forÃƒÂ§a ligaÃƒÂ§ÃƒÂ£o sequencial.
         if(bomba)
         {
            if (val[0]) 
            {
               val1=1;
               val2=0;
               val3=0;
               val4=0;
            }
            if (val[1]) 
            {
               val1=0;
               val2=1;
               val3=0;
               val4=0;
            }
            if (val[2]) 
            {
               val1=0;
               val2=0;
               val3=1;
               val4=0;
            }
            if (val[3]) 
            {
               val1=0;
               val2=0;
               val3=0;
               val4=1;
            }
         }   
         if(!bomba)
         {
            val[0]=0;
            val[1]=0;
            val[2]=0;
            val[3]=0;
            val1=0;
            val2=0;
            val3=0;
            val4=0;
         }
         
         // botao de mudanÃƒÂ§a de pÃƒÂ¡gina
         if (!aum_pagina&&!bomba) 
         {
            if (!botao_aum_pagina)
            {
               if (filtro6)
               {
                  filtro6--;
               }
               else {
                  botao_aum_pagina=1;         
                  pagina = 1;
               }
            }
         }
         else 
         {
            botao_aum_pagina=0;
            filtro6 = t_filtro;
         }
         
         //BOTÃƒÆ'O PARA SELEÃƒâ€¡ÃƒÆ'O DE PROGRAMA: MANUAL, TIMER, AUTO E SMART.
         if (!esquerdo&&!bomba) //nÃƒÂ£o pode mudar de programa durante o funcionamento
         { 
            if (!botao_esquerdo)
            {
               if (filtro2)
               {
                  filtro2--;
               }
               else 
               {
                  botao_esquerdo=1;
                  programa++;
                  i=0;
                  if (programa>=4) 
                  {
                     programa=0;
                  }
                  programaModo();
               }
            }
         }
         else 
         {
            botao_esquerdo=0;
            filtro2 = t_filtro;
         }
         
         // Progamas do mÃƒÂ³dulo controlador: Manual, Timer, Auto e Smart.
         switch(programa)
         {
         ////////////////////////// MANUAL ////////////////////////////////////
            case 0:  
               // escreve no lcd
               lcd_gotoxy(10,1); // posiciona o cursor na linha 1, coluna 14
               printf (lcd_putc, " Manual");      // imprime mensagem no lcd
               lcd_gotoxy(14,2);
               printf (lcd_putc, "   ");
               //BOTÃƒÆ'O LIGA/DESLIGA RELÃƒâ€° DA BOMBA
               if (!liga)
               {
                  if (!botao_direito)
                  {
                     if (filtro)
                     {
                        filtro--;
                     }
                     else
                     {
                        botao_direito=1;
                        bomba = !bomba;  
                        if (bomba)  
                        {
                           val[i]=1;
                           ligaBomba();
                        }
                        else
                        {
                           desligaBomba();
                        }
                        
                     }
                  }
               }
               else 
               {
                  botao_direito=0;
                  filtro = t_filtro;
               }
               if(!acima)
               {
                  if (!botao_acima)
                  {
                     if (filtro3)
                     {
                        filtro3--;
                     }
                     else 
                     {
                        botao_acima=1;
                        val[i]=0;
                        i++;
                        if (i>=4)  
                        { 
                           i=0;
                        }
                        val[i]=1;
                        if (bomba) 
                        {
                           ligaBomba();
                        }
                     }
                  }
               }
               else 
               {
                    botao_acima=0;
                    filtro3 = t_filtro;
               }
               if(!abaixo)
               {
                  if (!botao_abaixo)
                  {
                     if (filtro4)
                     {
                        filtro4--;
                     }
                     else 
                     {
                         botao_abaixo=1;
                         val[i]=0;
                         i--;
                         if (i==255)
                         {
                              i=3;
                         }
                         val[i]=1;
                         if (bomba)
                         {
                           ligaBomba();
                         }
                     }
                  }
               }
               else {
                   botao_abaixo=0;
                   filtro4 = t_filtro;
               }
               break;
               
            ////////////////////////////// TIMER //////////////////////////////   
            case 1:              
               // escreve no lcd
               lcd_gotoxy(10,1);  // posiciona o cursor na linha 1, coluna 14
               printf (lcd_putc, "  Timer");      // imprime mensagem no lcd
               
               //vÃƒÂ¡vula com tempo 0 nao entra
               while (!m_val[i]&&!h_val[i]&&i<=4)   
               { 
                  val[i]=0;
                  i++;
               }
               if(i>=4)
               {
                  i=0;
               }
               
               //liga a valvula e comeÃƒÂ§a sequencia
               if((m_val[i]||h_val[i])&&bomba)
               {
                  val[i]=1;
                  
               } 
                                           
               if(!m_val[0]&&!h_val[0]&&!m_val[1]&&!h_val[1]&&!m_val[2]
               &&!h_val[2]&&!m_val[3]&&!h_val[3])
               {
                  
                  bomba=0;
                  i=0;
                  //renova os tempos de valvulas apos o dia ter passado.
                  m_val[0]=mt_val[0];
                  h_val[0]=ht_val[0];
                  m_val[1]=mt_val[1];
                  h_val[1]=ht_val[1];
                  m_val[2]=mt_val[2];
                  h_val[2]=ht_val[2];
                  m_val[3]=mt_val[3];
                  h_val[3]=ht_val[3];
               } 
               
               // Desliga tudo ao apertar "direito"
               if ((!liga)&&bomba)
               {
                  if (!botao_direito)
                  {
                     if (filtro)
                     {
                        filtro--;
                     }
                     else 
                     {
                        botao_direito=1;
                        val[0]=0;
                        val[1]=0;
                        val[2]=0;
                        val[3]=0;
                        i=0;
                        bomba=0;
                        programa=0;
                        desligaBomba();
                     }
                  }
               }
               else  
               {
                  botao_direito=0;
                  filtro=t_filtro;
               } 
            break;     
            case 2:  
               // escreve no lcd
              
               lcd_gotoxy(10,1);          // posiciona o cursor na linha 1,
                                          //coluna 14
               printf (lcd_putc, "  Auto ");      // imprime mensagem no lcd
                     //se o limite superior do sensor for atingido, 
                     //muda de vÃƒÂ¡lvula no prÃƒÂ³ximo minuto.
               while (!m_val[i]&&!h_val[i]&&i<=4)
               { //vÃƒÂ¡vula com tempo 0 nao entra
                  val[i]=0;
                  i++;
               }
               if(i>=4){
                  i=0;
               }
               if((m_val[i]||h_val[i])&&(sensor[i]<=limite_sup)&&bomba)
               {//liga a valvula e decresce o seu tempo
                  val[i]=1;                  
               }
               if((sensor[i]>limite_sup)&&bomba)
               {
                  m_val[i]=0;
                  val[i]=0;                  
               }          
               if(!m_val[0]&&!h_val[0]&&!m_val[1]&&!h_val[1]&&!m_val[2]
               &&!h_val[2]&&!m_val[3]&&!h_val[3])
               {                  
                  bomba=0;
                  i=0;
                  //renova os tempos de valvulas apos o dia ter passado.
                  m_val[0]=mt_val[0];
                  h_val[0]=ht_val[0];
                  m_val[1]=mt_val[1];
                  h_val[1]=ht_val[1];
                  m_val[2]=mt_val[2];
                  h_val[2]=ht_val[2];
                  m_val[3]=mt_val[3];
                  h_val[3]=ht_val[3];
               }
               if (!liga&&bomba)
               {
                  if (!botao_direito)
                  {
                     if (filtro)
                     {
                        filtro--;
                     }
                     else {
                        botao_direito=1;
                        val[0]=0;
                        val[1]=0;
                        val[2]=0;
                        val[3]=0;
                        i=0;
                        bomba=0;
                        programa=0;   
                        desligaBomba();
                     }
                  }
               }
               else 
               {
                  botao_direito=0;
                  filtro = t_filtro;
               } 
            break;  
            case 3:  //////////////////////MODO SMART///////////////////////////
                     // escreve no lcd
                     lcd_gotoxy(10,1);         // posiciona o cursor 
                     printf (lcd_putc, "  Smart");      // imprime mensagem 
                     // no lcd
                     // condiÃƒÂ§ÃƒÂµes para desligar a valvula--> valvula sem tempo, 
                     // setor umido, exclusao de horario e dia da semana. 
                     if(((!m_val[0]&&!h_val[0])||(sensor[0]>limite_sup)
                     ||(!dia[cont_semana])))
                     {                        
                        val[0]=0;               
                     }
                     if((sensor[0]<limite_inf)&&(m_val[0]||h_val[0])&&(!val[0]
                     &&!val[1]&&!val[2]&&!val[3])&&(horas<hora_inf
                     ||horas>=hora_sup))
                     { //sensor seco -> liga setor e bomba                        
                        bomba=1;
                        val[0]=1;
                        i=0;    
                     }
                     if(((!m_val[1]&&!h_val[1])||(sensor[1]>limite_sup)
                     ||(!dia[cont_semana-1])))
                     {
                        val[1]=0;                            
                     }
                     if((sensor[1]<limite_inf)&&(m_val[1]||h_val[1])&&(!val[0]
                     &&!val[1]&&!val[2]&&!val[3])&&(horas<hora_inf
                     ||horas>=hora_sup))
                     { //sensor seco -> liga setor e bomba
                        bomba=1;
                        val[1]=1;
                        i=1;                            
                     }
                     if(((!m_val[2]&&!h_val[2])||(sensor[2]>limite_sup)
                     ||(!dia[cont_semana-1])))
                     {
                        val[2]=0;                             
                     }
                     if((sensor[2]<limite_inf)&&(m_val[2]||h_val[2])&&(!val[0]
                     &&!val[1]&&!val[2]&&!val[3])&&(horas<hora_inf
                     ||horas>=hora_sup))
                     { //sensor seco -> liga setor e bomba
                        bomba=1;
                        val[2]=1;
                        i=2;                            
                     }
                     if(((!m_val[3]&&!h_val[3])||(sensor[3]>limite_sup)
                     ||(!dia[cont_semana-1])))
                     {
                        val[3]=0;                           
                     }
                     if((sensor[3]<limite_inf)&&(m_val[3]||h_val[3])&&(!val[0]
                     &&!val[1]&&!val[2]&&!val[3])&&(horas<hora_inf
                     ||horas>=hora_sup))
                     { //sensor seco -> liga setor e bomba
                        bomba=1;
                        val[3]=1;
                        i=3;                            
                     }            
                     if(!val[0]&&!val[1]&&!val[2]&&!val[3])
                     { //parar a ultima vÃƒÂ¡lvula e a bomba
                        bomba=0;
                     }
                     if ((!liga)&&bomba)
                     {
                        if (!botao_direito)
                        {
                           if (filtro)
                           {
                              filtro--;
                           }
                           else 
                           {
                              botao_direito=1;
                              val[0]=0;
                              val[1]=0;
                              val[2]=0;
                              val[3]=0;
                              i=0;
                              bomba=0;
                              programa=0; 
                              desligaBomba();
                           }
                        }
                     }
                     else 
                     {
                        botao_direito=0;
                        filtro = t_filtro;
                     }            
            break;
            }   
          break;
          
///////////////////////////////////////////////////////////////////////////////
//                       2. TELA DE AJUSTE DA DATA                          //
///////////////////////////////////////////////////////////////////////////////
       case 1:
         mytime = RTC_Get();
         
         dia_teste = mytime->day;
         mes_teste = mytime->month;
         ano_teste = mytime->year;
         
         if (mytime->dow==1){
            dia_da_semana_str = "DOM";
         }
         if (mytime->dow==2){
            dia_da_semana_str = "SEG";
         }
         if (mytime->dow==3){
            dia_da_semana_str = "TER";
         }
         if (mytime->dow==4){
            dia_da_semana_str = "QUA";
         }
         if (mytime->dow==5){
            dia_da_semana_str = "QUI";
         }
         if (mytime->dow==6){
            dia_da_semana_str = "SEX";
         }
         if (mytime->dow==7){
            dia_da_semana_str = "SAB";
         }
    
   
         // escreve no LCD
         lcd_gotoxy(1,1); 
         printf (lcd_putc, "%1u. AJUSTAR DATA ", pagina+1);   
         lcd_gotoxy(1,2); 
         //printf (lcd_putc, "   %02u/%02u/%02u-%s    ", mytime->day, mytime->month, mytime->year, dia_da_semana_str); 
         printf (lcd_putc, "   %02u/%02u/%02u-%s    ", dia_teste, mes_teste, ano_teste, dia_da_semana_str);         
         
         //CONTROLE DO BACKLITH LCD   
         if(!liga||!esquerdo||!acima||!abaixo||!aum_pagina)
         {
            tempo_lcd=minutos;
            //=1;
         }
         if(minutos==(tempo_lcd+2))
         {
           // //lcd=0
            pagina=0;
         } 
         if (!aum_pagina&&!bomba)
         {
            if (!botao_aum_pagina)
            {
               if (filtro6)
               {
                  filtro6--;
               }
               else 
               {
                  botao_aum_pagina=1;         
                  pagina=2;
               }
            }
         }
         else 
         {
            botao_aum_pagina=0;
            filtro6 = t_filtro;
         }
         
         if (!liga)
         {
            ajuste_data++;
            delay_ms(300);
            if (ajuste_data>4)
            {
               ajuste_data=0;
            }
         }
         
         if (ajuste_data==1&&!liga)
         { //PISCAR
            posicao0=1;//AJUSTA DIA
            lcd_gotoxy(4,2);
            printf (lcd_putc,"__");
            delay_ms(300);
         }
         if (ajuste_data==2&&!liga)
         {
            posicao0=2;//AJUSTA MES
            lcd_gotoxy(7,2);
            printf (lcd_putc,"__");
            delay_ms(300);
         }
         if (ajuste_data==3&&!liga)
         {
            posicao0=3;//AJUSTA ANO
            lcd_gotoxy(10,2);
            printf (lcd_putc,"__");
            delay_ms(300);
         }
         if (ajuste_data==4&&!liga)
         {
            posicao0=4;//AJUSTA DIA DA SEMANA
            lcd_gotoxy(13,2);
            printf (lcd_putc,"___");
            delay_ms(300);
         }
      switch (posicao0){
         case 1:
            //AJUSTE DO DIA:
            if (!acima){
               dia_1 = mytime->day;
               dia_1++;
               if (dia_1>31) dia_1=1;
               mytime->day = dia_1;
               RTC_Set(mytime);
               delay_ms(300);
            }
            if (!abaixo){
               dia_1 = mytime->day;
               dia_1--;
               if (dia_1<1) dia_1=31;
               mytime->day = dia_1;
               RTC_Set(mytime);
               delay_ms(300);
            }
         break;
         case 2:
            //AJUSTE DO MES:
            if (!acima){
               mes_1 = mytime->month;
               mes_1++;
               if (mes_1>12) mes_1=1;
               mytime->month = mes_1;
               RTC_Set(mytime);
               delay_ms(300);
            }
            if (!abaixo){
               mes_1 = mytime->month;
               mes_1--;
               if (mes_1<1) mes_1=12;
               mytime->month = mes_1;
               RTC_Set(mytime);
               delay_ms(300);
            }
         break;
         case 3:
            //AJUSTE DO ANO:
            if (!acima){
               ano_1 = mytime->year;
               ano_1++;
               if (ano_1>99) ano_1=23;
               mytime->year = ano_1;
               RTC_Set(mytime);
               delay_ms(300);
            }
            if (!abaixo){
               ano_1 = mytime->year;
               ano_1--;
               if (ano_1<23) ano_1=99;
               mytime->year = ano_1;
               RTC_Set(mytime);
               delay_ms(300);
            }
         break;
         case 4:
         //AJUSTE DO DIA DA SEMANA:
            if (!acima){
               dia_da_semana = mytime->dow;
               dia_da_semana++;
               if (dia_da_semana>7) dia_da_semana=1;
               mytime->dow = dia_da_semana;
               RTC_Set(mytime);
               delay_ms(300);
            }
            if (!abaixo){
               dia_da_semana = mytime->dow;
               dia_da_semana--;
               if (dia_da_semana<1) dia_da_semana=7;
               mytime->dow = dia_da_semana;
               RTC_Set(mytime);
               delay_ms(300);
            }
         break;
      }
      break;
          
///////////////////////////////////////////////////////////////////////////////
//                       3. TELA DE AJUSTE DE HORA                           //
///////////////////////////////////////////////////////////////////////////////
       case 2:
         // atualiza tempo
         mytime = RTC_Get();
         
         // escreve no LCD
         lcd_gotoxy(1,1); 
         printf (lcd_putc, "%1u. AJUSTAR HORA ", pagina+1);   
         lcd_gotoxy(1,2); 
         printf (lcd_putc, "   %02u:%02u:%02u     ", mytime->hours, mytime->minutes, mytime->seconds); 
        
         
         //CONTROLE DO BACKLITH LCD   
         if(!liga||!esquerdo||!acima||!abaixo||!aum_pagina)
         {
            tempo_lcd=minutos;
            //=1;
         }
         if(minutos==(tempo_lcd+2))
         {
           // //lcd=0
            pagina=0;
         } 
         //BOTAO MODO + E -
        /* if (!dim_pagina&&!bomba)
         {
            if (!botao_dim_pagina)
            {
               if (filtro5)
               {
                  filtro5--;
               }
               else 
               {
                  botao_dim_pagina=1;
                  pagina=0;
               }
            }
         }
         else {
            botao_dim_pagina=0;
            filtro5 = t_filtro;
         }*/
         if (!aum_pagina&&!bomba)
         {
            if (!botao_aum_pagina)
            {
               if (filtro6)
               {
                  filtro6--;
               }
               else 
               {
                  botao_aum_pagina=1;         
                  pagina=3;
               }
            }
         }
         else 
         {
            botao_aum_pagina=0;
            filtro6 = t_filtro;
         }
         if (!esquerdo)
         { //PISCAR
            posicao1=0;//AJUSTA HORAS
            lcd_gotoxy(4,2);
            printf (lcd_putc,"  ");
            delay_ms(180);
         }
         if (!liga)
         {
            posicao1=1;//AJUSTA MINUTOS
            lcd_gotoxy(7,2);
            printf (lcd_putc,"  ");
            delay_ms(180);
         }
      switch (posicao1){
         case 1:
         //AJUSTE DOS MINUTOS:
         if(!acima){
            minutos = mytime->minutes;
            minutos++;
            if (minutos>59) minutos=0;
            mytime->minutes = minutos;
            RTC_Set(mytime);    // seta hora e minuto no DS1307
            delay_ms(300);
         }    
         if(!abaixo){
            minutos = mytime->minutes;
            minutos--;
            if (minutos==255) minutos=59;
            mytime->minutes = minutos;
            RTC_Set(mytime);    // seta hora e minuto no DS1307
            delay_ms(300);
         }
         break;
         case 0:
         //AJUSTE DAS HORAS:
         if(!acima)
         {
            horas = mytime->hours;
            horas++;
            if (horas>23) horas=0;
            mytime->hours = horas;
            RTC_Set(mytime);    // seta hora e minuto no DS1307
            delay_ms(300);     
         }    
         if(!abaixo)
         {
            horas = mytime->hours;
            horas--;
            if (horas==255) horas=23;
            mytime->hours = horas;
            RTC_Set(mytime);    // seta hora e minuto no DS1307
            delay_ms(300);     
         }
         break;
       }
       break;  
      
///////////////////////////////////////////////////////////////////////////////
//        4. AJUSTE DO TEMPO DE REGA PARA 4 VÃƒÂVLULAS: h_val e m_val          //
///////////////////////////////////////////////////////////////////////////////
       case 3:
         lcd_gotoxy(1,1); // posiciona o cursor na linha 0, coluna 0
         printf (lcd_putc, "%1u. TEMPO P SETOR", pagina+1);
         
         //CONTROLE DO BACKLITH LCD   
         if(!liga||!esquerdo||!acima||!abaixo||!aum_pagina)
         {
            tempo_lcd=minutos;
           // //lcd=1
         }
         if(minutos==(tempo_lcd+2))
         {
          //  //lcd=0
            pagina=0;
         } 
                  
         if (!aum_pagina&&!bomba)
         {
            if (!botao_aum_pagina)
            {
               if (filtro6)
               {
                  filtro6--;
               }
               else {
                  botao_aum_pagina=1;         
                  pagina=4;
               }
            }
         }
         else 
         {
            botao_aum_pagina=0;
            filtro6 = t_filtro;
         }
         
         //SELEÃƒâ€¡ÃƒÆ'O DA VÃƒÂLVULA
         if(!esquerdo)
         {
            cont_val++;
            if (cont_val>3) cont_val=0;
            delay_ms(300);
         }
         lcd_gotoxy(1,2);
         printf (lcd_putc,"Setor %1u:   %02u:%02u", cont_val+1, h_val[cont_val], m_val[cont_val]);
     
         
         //AJUSTE DE TEMPO DA VÃƒÂLVULA 
         if(!liga)
         { //SELEÃƒâ€¡ÃƒÆ'O DE HORA/MINUTO
            posicao2=!posicao2;//
            delay_ms(180);
            if (!posicao2) 
            { //PISCAR HORAS VALVULA
               lcd_gotoxy(11,2);
               printf (lcd_putc,"  ");
               delay_ms(180);
            }
            if (posicao2)
            { //PISCAR MINUTOS VALVULA
               lcd_gotoxy(15,2);
               printf (lcd_putc,"  ");
               delay_ms(180);
            }
         }  
         switch (posicao2){
         
         case 0:
         //AJUSTE DAS HORAS VALVULA 
         if(!acima)
         {
            h_val[cont_val]=h_val[cont_val]+1;
            if (h_val[cont_val]>=24) h_val[cont_val]=0;
            ht_val[cont_val]=h_val[cont_val];
            delay_ms(300);
         }    
         if(!abaixo)
         {
            h_val[cont_val]=h_val[cont_val]-1;
            if (h_val[cont_val]==255) h_val[cont_val]=23;
            ht_val[cont_val]=h_val[cont_val];
            delay_ms(300);
         }
         break;
         case 1:         
         //AJUSTE DOS MINUTOS VALVULA
         if(!acima)
         {
            m_val[cont_val]=m_val[cont_val]+1;
            if (m_val[cont_val]>=60) m_val[cont_val]=0;
            mt_val[cont_val]=m_val[cont_val];
            delay_ms(300);
         }
         if(!abaixo)
         {
            m_val[cont_val]=m_val[cont_val]-1;
            if (m_val[cont_val]==255) m_val[cont_val]=59;
            mt_val[cont_val]=m_val[cont_val];
            delay_ms(300);
         }
         break;
         }    
       break;
       
///////////////////////////////////////////////////////////////////////////////
//                         5. HORA INICIAL DE REGA                           //
///////////////////////////////////////////////////////////////////////////////
       case 4:
         lcd_gotoxy(1,1);
         printf (lcd_putc, "%1u. HORA DE LIGAR", pagina+1);   
         lcd_gotoxy(1,2); 
         printf (lcd_putc, "   %02u:%02uh       ", hi_val, mi_val);
         
         //CONTROLE DO BACKLITH LCD   
         if(!liga||!esquerdo||!acima||!abaixo||!aum_pagina)
         {
            tempo_lcd=minutos;
           // //lcd=1
         }
         if(minutos==(tempo_lcd+2))
         {
           // //lcd=0
            pagina=0;
         } 
                
         if (!aum_pagina&&!bomba)
         {
            if (!botao_aum_pagina)
            {
               if (filtro6
               ){
                  filtro6--;
               }
               else 
               {
                  botao_aum_pagina=1;         
                  pagina=5;
               }
            }
         }
         else 
         {
            botao_aum_pagina=0;
            filtro6 = t_filtro;
         }
         if (!esquerdo)
         {
            posicao3=0;//PISCA HORAS
            lcd_gotoxy(4,2);
            printf (lcd_putc,"  ");
            delay_ms(180);
         }
         if (!liga){//
            posicao3=1;//PISCA MINUTOS
            lcd_gotoxy(7,2);
            printf (lcd_putc,"  ");
            delay_ms(180);
         }
         switch (posicao3)
         {
         case 1:
         //AJUSTE DOS MINUTOS:
         if(!acima)
         {
            mi_val++;
            if (mi_val>=60) mi_val=0;
               delay_ms(300);
         }
         if(!abaixo)
         {
            mi_val--;
            if (mi_val==255) mi_val=59;
               delay_ms(300); 
         }
         break;
         case 0:
         //AJUSTE DAS HORAS:
         if(!acima)
         {
            hi_val++;
            if (hi_val>=24) hi_val=0;
            delay_ms(300);
         }    
         if(!abaixo)
         {
            hi_val--;
            if (hi_val==255) hi_val=23;
            delay_ms(300);
         }
         break;
         }
       break;
       
/////////////////////////////////////////////////////////////////////////////// 
//            6. ESCOLHA DO LIMITE INFERIOR E SUPERIOR DO SENSOR             //
///////////////////////////////////////////////////////////////////////////////
       case 5:
         //static boolean caso;
         
         lcd_gotoxy(1,1);
         printf (lcd_putc, "%u. LIMITE SENSOR", pagina+1);   
         lcd_gotoxy(1,2);
         printf (lcd_putc, "inf:%02lu", limite_inf);
         lcd_putc(0x25); //sinal de porcentaqem Perguntar
         printf (lcd_putc, "  ");
         lcd_gotoxy(10,2);
         printf (lcd_putc, "sup:%02lu", limite_sup);
         lcd_putc(0x25); //sinal de porcentaqem
         
         //CONTROLE DO BACKLITH LCD   
         if(!liga||!esquerdo||!acima||!abaixo||!aum_pagina)
         {
            tempo_lcd=minutos;
           // lcd=1;
         }
         if(minutos==(tempo_lcd+2))
         {
            //lcd=0
            pagina=0;
         } 
         
         if (!aum_pagina&&!bomba)
         {
            if (!botao_aum_pagina)
            {
               if (filtro6)
               {
                  filtro6--;
                }
                else {
                   botao_aum_pagina=1;         
                   pagina=6;
                }
            }
         }
         else
         {
            botao_aum_pagina=0;
            filtro6 = t_filtro;
         }
         if (!esquerdo)
         {
            posicao4=0;//PISCA HORAS
            lcd_gotoxy(5,2);
            printf (lcd_putc,"   ");
            delay_ms(180);
         }
         if (!liga){//
            posicao4=1;//PISCA MINUTOS
            lcd_gotoxy(14,2);
            printf (lcd_putc,"   ");
            delay_ms(180);
         } 
         switch (posicao4)
         {
         
         case 0:
         //AJUSTE do LIMITE INFERIOR DO SENSOR
         if(!acima)
         {
         limite_inf++;
         if (limite_inf>99) limite_inf=0;
         delay_ms(200);
         }
         if(!abaixo)
         {
         limite_inf--;
         if (limite_inf==65535) limite_inf=99;
         delay_ms(200);
         }
       break;
       case 1:
         //AJUSTE DO LIMITE SUPERIOR DO SENSOR
         if(!acima)
         {
            limite_sup++;
            if (limite_sup>99) limite_sup=0;
            delay_ms(200);
         }    
         if(!abaixo)
         {
            limite_sup--;
            if (limite_sup==65535) limite_sup=99;
            delay_ms(200);
         }
         break;
         }
       break;
       
/////////////////////////////////////////////////////////////////////////////// 
//                           6. EXCLUSAO DE HORÃƒÂRIO                          //
///////////////////////////////////////////////////////////////////////////////
       case 6:
         //static boolean caso;
         
         lcd_gotoxy(1,1);
         printf (lcd_putc, "%u. NAO FUNCIONAR", pagina+1);   
         lcd_gotoxy(1,2);
         printf (lcd_putc,"de:%2uh ate:%2uh  ", hora_inf, hora_sup);
         
         
          //CONTROLE DO BACKLITH LCD   
         if(!liga||!esquerdo||!acima||!abaixo||!aum_pagina)
         {
            tempo_lcd=minutos;
           // lcd=1;
         }
         if(minutos==(tempo_lcd+2))
         {
            //lcd=0
            pagina=0;
         } 
                 
         if (!aum_pagina&&!bomba) 
         {
            if (!botao_aum_pagina)
            {
               if (filtro6)
               {
                  filtro6--;
               }
               else {
                  botao_aum_pagina=1;         
                  //ds1307_get_dds(cont_semana);
                  pagina=7;
               }
            }
         }
         else 
         {
            botao_aum_pagina=0;
            filtro6 = t_filtro;
         }
         if (!esquerdo)
         {
            posicao5=0;//PISCA HORAS
            lcd_gotoxy(5,2);
            printf (lcd_putc,"  ");
            delay_ms(180);
         }
         if (!liga)
         {//
            posicao5=1;//PISCA MINUTOS
            lcd_gotoxy(13,2);
            printf (lcd_putc,"  ");
            delay_ms(180);
         }  
         switch (posicao5)
         {
         
         case 0:
         //AJUSTE DA HORA INFERIOR
         if(!acima)
         {
            hora_inf++;
         if (hora_inf>=24) hora_inf=0;
            delay_ms(300);
         }
         if(!abaixo){
            hora_inf--;
         if (hora_inf==255) hora_inf=23;
            delay_ms(300);
         }
       break;
       case 1:
         //AJUSTE DA HORA SUPERIOR
         if(!acima)
         {
            hora_sup++;
         if (hora_sup>=24) hora_sup=0;
            delay_ms(300);
         }    
         if(!abaixo)
         {
            hora_sup--;
         if (hora_sup==255) hora_sup=23;
            delay_ms(300);
         }
         break;
         }
       break;
       
///////////////////////////////////////////////////////////////////////////////
//                8. AJUSTE DE DIA E EXCLUSAO DE DIA                         //
///////////////////////////////////////////////////////////////////////////////
       case 7:
         
         lcd_gotoxy(1,1);
         printf (lcd_putc, "%1u. DIA: ", pagina+1);   
         lcd_gotoxy(1,2);
         printf (lcd_putc, " ");
         lcd_gotoxy(3,2);
         printf (lcd_putc, " ");
         lcd_gotoxy(5,2);
         printf (lcd_putc, " ");
         lcd_gotoxy(7,2);
         printf (lcd_putc, " ");
         lcd_gotoxy(9,2);
         printf (lcd_putc, " ");
         lcd_gotoxy(11,2);
         printf (lcd_putc, " ");
         lcd_gotoxy(13,2);
         printf (lcd_putc, " ");
         lcd_gotoxy(15,2);
         printf (lcd_putc, " ");
         
         //CONTROLE DO BACKLITH LCD   
         if(!liga||!esquerdo||!acima||!abaixo||!aum_pagina)
         {
            tempo_lcd=minutos;
           // lcd=1;
         }
         if(minutos==(tempo_lcd+2))
         {
            //lcd=0
            pagina=0;
         } 
         // botao para trocar pÃƒÂ¡gina
         if (!aum_pagina&&!bomba)
         {
            if (!botao_aum_pagina)
            {
               if (filtro6){
                filtro6--;
                }
                else {
                   botao_aum_pagina=1;         
                   pagina=8;
                }
            }
         }
         else {
            botao_aum_pagina=0;
            filtro6 = t_filtro;
         }
         
         // Escreve dia da semana no lcd
         if (cont_semana==1)
         {
            lcd_gotoxy(9,1);
            printf (lcd_putc, "Domingo  ");
         }
         if (cont_semana==2)
         {
            lcd_gotoxy(9,1);
            printf (lcd_putc, "Segunda  ");
         }
         if (cont_semana==3)
         {
            lcd_gotoxy(9,1);
            printf (lcd_putc, "Terca    ");
         }
         if (cont_semana==4)
         {
            lcd_gotoxy(9,1);
            printf (lcd_putc, "Quarta   ");
         }
         if (cont_semana==5)
         {
            lcd_gotoxy(9,1);
            printf (lcd_putc, "Quinta   ");
         }
         if (cont_semana==6)
         {
            lcd_gotoxy(9,1);
            printf (lcd_putc, "Sexta    ");
         }
         if (cont_semana==7)
         {
            lcd_gotoxy(9,1);
            printf (lcd_putc, "Sabado   ");
         }
         
         // exclusÃƒÂ£o de dias
         if(dia[0]==1){ //D S T Q Q S S
            lcd_gotoxy(2,2);
            printf (lcd_putc,"D");
         }
         if(!dia[0])
         {
            lcd_gotoxy(2,2);
            printf (lcd_putc,"_");
         }
         if(dia[1]==1)
         {
            lcd_gotoxy(4,2);
            printf (lcd_putc,"S");
         }
         if(dia[1]==0)
         {
            lcd_gotoxy(4,2);
            printf (lcd_putc,"_");
         }
         if(dia[2]==1)
         {
            lcd_gotoxy(6,2);
            printf (lcd_putc,"T");
         }
         if(dia[2]==0)
         {
            lcd_gotoxy(6,2);
            printf (lcd_putc,"_");
         }
         if(dia[3]==1)
         {
            lcd_gotoxy(8,2);
            printf (lcd_putc,"Q");
         }
         if(dia[3]==0)
         {
            lcd_gotoxy(8,2);
            printf (lcd_putc,"_");
         }   
         if(dia[4]==1)
         {
            lcd_gotoxy(10,2);
            printf (lcd_putc,"Q");
         }
         if(dia[4]==0)
         {
            lcd_gotoxy(10,2);
            printf (lcd_putc,"_");
         } 
         if(dia[5]==1)
         {
            lcd_gotoxy(12,2);
            printf (lcd_putc,"S");
         }
         if(dia[5]==0)
         {
            lcd_gotoxy(12,2);
            printf (lcd_putc,"_");
         } 
         if(dia[6]==1){
            lcd_gotoxy(14,2);
            printf (lcd_putc,"S");
         }
         if(dia[6]==0)
         {
            lcd_gotoxy(14,2);
            printf (lcd_putc,"_");
         } 
         
         //SELEÃƒâ€¡ÃƒÆ'O dO VISOR
         if(!liga||!esquerdo)
         {
            if(!liga)
            {
               posicao6++;
               if(posicao6>7) posicao6=0;
               delay_ms(300);
            }
            if(!esquerdo)
            {
               posicao6--;
               if(posicao6==255) posicao6=7;
               delay_ms(300);
            }
            if (!posicao6)
            { //PISCAR
               lcd_gotoxy(9,1);
               printf (lcd_putc,"          ");
               delay_ms(180);
            }
            if (posicao6==1)
            { //PISCAR
               lcd_gotoxy(2,2);
               printf (lcd_putc,"  ");
               delay_ms(180);        
            }
            if (posicao6==2)
            { //PISCAR
               lcd_gotoxy(4,2);
               printf (lcd_putc,"  ");
               delay_ms(180);
            }
            if (posicao6==3)
            { //PISCAR
               lcd_gotoxy(6,2);
               printf (lcd_putc,"  ");
               delay_ms(180);
            }
            if (posicao6==4)
            { //PISCAR
               lcd_gotoxy(8,2);
               printf (lcd_putc,"  ");
               delay_ms(180);
            }
            if (posicao6==5)
            { //PISCAR
               lcd_gotoxy(10,2);
               printf (lcd_putc,"  ");
               delay_ms(180);
            }
            if (posicao6==6)
            { //PISCAR
               lcd_gotoxy(12,2);
               printf (lcd_putc,"  ");
               delay_ms(180);
            }
            if (posicao6==7)
            { //PISCAR
               lcd_gotoxy(14,2);
               printf (lcd_putc,"  ");
               delay_ms(180);
            }
         }
         switch (posicao6){
         case 0:
         //AJUSTE DO DIA DA SEMANA E DIA=0 OU 1
         
         if(!acima){
            cont_semana++;
            if (cont_semana>7)
            {
               cont_semana = 1;
            }
            //ds1307_set_dds(cont_semana);
            delay_ms(300);
         }
         if(!abaixo)
         {
            cont_semana--;
            if (cont_semana<1)
            {
               cont_semana=7;
            }
            //ds1307_set_dds(cont_semana);
            delay_ms(300);
         }
         
         break;
         case 1:
         if(!acima)
         {
            dia[0]=1;    
         }    
         if(!abaixo)
         {
            dia[0]=0;
         }
         break;
         case 2:
         if(!acima)
         {
            dia[1]=1;
         }    
         if(!abaixo)
         {
            dia[1]=0;
         }
         break;
         case 3:
         if(!acima)
         {
            dia[2]=1;
         }    
         if(!abaixo)
         {
            dia[2]=0;
         }
         break;
         case 4:
         if(!acima)
         {
            dia[3]=1;
         }    
         if(!abaixo)
         {
            dia[3]=0;
         }
         break;
         case 5:
         if(!acima)
         {
            dia[4]=1;
         }    
         if(!abaixo)
         {
            dia[4]=0;
         }
         break;
         case 6:
         if(!acima)
         {
            dia[5]=1;
         }    
         if(!abaixo)
         {
            dia[5]=0;
         }
         break;
         case 7:
         if(!acima)
         {
            dia[6]=1;
         }    
         if(!abaixo)
         {
            dia[6]=0;
         }
         break;
         }
         break;
         
///////////////////////////////////////////////////////////////////////////////
//////////////////        SALVAR CONFIGURAÃƒâ€¡Ãƒâ€¢ES NA EEPROM       ////////////////
///////////////////////////////////////////////////////////////////////////////                  
         case 8:
         lcd_gotoxy(1,1);         
         printf (lcd_putc, "%u. SALVAR CONF.?", pagina+1);   
         lcd_gotoxy(1,2); 
         printf (lcd_putc, "  ");
         lcd_putc(0x7f); //olhar
         printf (lcd_putc, " Nao   Sim ");
         lcd_putc(0x7e);
         printf (lcd_putc, " ");
         
         //CONTROLE DO BACKLITH LCD   
         if(!liga||!esquerdo||!acima||!abaixo||!aum_pagina){
            tempo_lcd=minutos;
            //lcd=1;
         }
         if(minutos==(tempo_lcd+2))
         {
            //lcd=0
            pagina=0;
         }   
         
         if (!aum_pagina) {
            if (!botao_aum_pagina){      
               if (filtro6){
                  filtro6--;
               }
               else {
                  botao_aum_pagina=1;         
                  pagina=0;
                  delay_ms(200);
               }
            }
         }
         else {
            botao_aum_pagina=0;
            filtro6 = t_filtro;
         }
         
         //NÃƒÆ'O
         if (!esquerdo){ 
            if (!botao_esquerdo){      
               if (filtro2){
                  filtro2--;
               }
               else {
                  botao_esquerdo=1;
                  pagina=0;
                  delay_ms(200);
               }
            }
         }
         else {
            botao_esquerdo=0;
            filtro2 = t_filtro;
         }
         
         //SIM
         if (!liga){
            if (!botao_direito){      
               if (filtro){
                  filtro--;
               }
               else {
                  botao_direito=1;
                  lcd_gotoxy(1,1);
                  printf (lcd_putc, "   SALVANDO...  ");
                  delay_ms(4);
                  lcd_gotoxy(1,2);
                  printf (lcd_putc, "                ");
                  write_eeprom(0,hi_val);         
                  write_eeprom(1,mi_val);
                  write_eeprom(2,programa);
                  write_eeprom(3,h_val[0]);
                  write_eeprom(4,h_val[1]);
                  write_eeprom(5,h_val[2]);
                  write_eeprom(6,h_val[3]);
                  write_eeprom(7,m_val[0]);
                  write_eeprom(8,m_val[1]);
                  write_eeprom(9,m_val[2]);
                  write_eeprom(10,m_val[3]);
                  write_eeprom(11,limite_inf);
                  write_eeprom(12,limite_sup);
                  write_eeprom(13,hora_inf);
                  write_eeprom(14,hora_sup);
                  write_eeprom(15,dia[0]);
                  write_eeprom(16,dia[1]);
                  write_eeprom(17,dia[2]);
                  write_eeprom(18,dia[3]);
                  write_eeprom(19,dia[4]);
                  write_eeprom(20,dia[5]);
                  write_eeprom(21,dia[6]);
                  // animaÃƒÂ§ÃƒÂ£o                
                  lcd_putc(0xff);
                  delay_ms(128);
                  lcd_putc(0xff);
                  delay_ms(128);
                  lcd_putc(0xff);
                  delay_ms(128);
                  lcd_putc(0xff);
                  delay_ms(128);
                  lcd_putc(0xff);
                  delay_ms(128);
                  lcd_putc(0xff);
                  delay_ms(1280);
                  delay_ms(512); 
                  pagina=0;
               }
            }
         }
         else {
            botao_direito=0;
            filtro = t_filtro;
         }
       break; 
       }
    
}

             
   
}
