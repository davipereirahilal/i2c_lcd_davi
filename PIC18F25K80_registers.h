// REGISTRADORES INCLU�DOS MANUALMENTE//
#byte WDTCON = 0xFD1
#byte TXSTA1 = 0xFAC
#byte RCSTA1 = 0xFAB
#byte BAUDCON1 = 0xFA7 
#byte SPBRG1 = 0xFAF
#byte SPBRGH1 = 0xF7D
#byte TXREG1 = 0xFAD // byte a ser enviado
#byte RXREG1 = 0xFAE // byte recebido

#bit TRMT = TXSTA1.1
#bit SPEN = RCSTA1.7
#biT TXEN = TXSTA1.5 

// PORTs
#byte   porta = 0xf80
#byte   portb = 0xf81
#byte   portc = 0xf82
#byte   porte = 0xf84

// LATs
#byte   lata = 0xf89
#byte   latb = 0xf8A
#byte   latc = 0xf8B
#byte   late = 0xf8D

//// GERADOS PELO CCS ///
#byte TOSU = 0xFFF
#byte TOSH = 0xFFE
#byte TOSL = 0xFFD
#byte STKPTR = 0xFFC
#bit    STKPTR0 = STKPTR.0
#bit    STKPTR1 = STKPTR.1
#bit    STKPTR2 = STKPTR.2
#bit    STKPTR3 = STKPTR.3
#bit    STKPTR4 = STKPTR.4
#bit    STKUNF = STKPTR.6
#bit    STKFUL = STKPTR.7
#byte STKPTR = 0xFFC
#bit    SP0 = STKPTR.0
#bit    SP1 = STKPTR.1
#bit    SP2 = STKPTR.2
#bit    SP3 = STKPTR.3
#bit    SP4 = STKPTR.4
#bit    STKOVF = STKPTR.7
#byte PCLATU = 0xFFB
#byte PCLATH = 0xFFA
#byte PCL = 0xFF9
#byte TBLPTRU = 0xFF8
#byte TBLPTRH = 0xFF7
#byte TBLPTRL = 0xFF6
#byte TABLAT = 0xFF5
#byte PRODH = 0xFF4
#byte PRODL = 0xFF3
#byte INTCON = 0xFF2
#bit    INT0F = INTCON.1
#bit    T0IF = INTCON.2
#bit    INT0E = INTCON.4
#bit    T0IE = INTCON.5
#bit    PEIE = INTCON.6
#bit    GIE = INTCON.7
#byte INTCON = 0xFF2
#bit    RBIF = INTCON.0
#bit    INT0IF = INTCON.1
#bit    TMR0IF = INTCON.2
#bit    RBIE = INTCON.3
#bit    INT0IE = INTCON.4
#bit    TMR0IE = INTCON.5
#bit    PEIE_GIEL = INTCON.6
#bit    GIE_GIEH = INTCON.7
#byte INTCON = 0xFF2
#bit    GIEL = INTCON.6
#bit    GIEH = INTCON.7
#byte INTCON2 = 0xFF1
#bit    INT3P = INTCON2.1
#bit    T0IP = INTCON2.2
#byte INTCON2 = 0xFF1
#bit    RBIP = INTCON2.0
#bit    INT3IP = INTCON2.1
#bit    TMR0IP = INTCON2.2
#bit    INTEDG3 = INTCON2.3
#bit    INTEDG2 = INTCON2.4
#bit    INTEDG1 = INTCON2.5
#bit    INTEDG0 = INTCON2.6
#bit    RBPU = INTCON2.7
#byte INDF0 = 0xFEF
#byte POSTINC0 = 0xFEE
#byte POSTDEC0 = 0xFED
#byte PREINC0 = 0xFEC
#byte PLUSW0 = 0xFEB
#byte FSR0H = 0xFEA
#byte FSR0L = 0xFE9
#byte WREG = 0xFE8
#byte INDF1 = 0xFE7
#byte POSTINC1 = 0xFE6
#byte POSTDEC1 = 0xFE5
#byte PREINC1 = 0xFE4
#byte PLUSW1 = 0xFE3
#byte FSR1H = 0xFE2
#byte FSR1L = 0xFE1
#byte BSR = 0xFE0
#byte INDF2 = 0xFDF
#byte POSTINC2 = 0xFDE
#byte POSTDEC2 = 0xFDD
#byte PREINC2 = 0xFDC
#byte PLUSW2 = 0xFDB
#byte FSR2H = 0xFDA
#byte FSR2L = 0xFD9
#byte STATUS = 0xFD8
#bit    C = STATUS.0
#bit    DC = STATUS.1
#bit    Z = STATUS.2
#bit    OV = STATUS.3
#bit    N = STATUS.4
#byte RCON = 0xFD0
#bit    BOR = RCON.0
#bit    POR = RCON.1
#bit    PD = RCON.2
#bit    TO = RCON.3
#bit    RI = RCON.4
#bit    CM = RCON.5
#bit    SBOREN = RCON.6
#bit    IPEN = RCON.7
#byte ADRESH = 0xFC4
#byte ADRESL = 0xFC3
#byte ADCON0 = 0xFC2
#bit    DONE = ADCON0.1
#byte ADCON0 = 0xFC2
#bit    ADON = ADCON0.0
#bit    GO = ADCON0.1
#bit    CHS0 = ADCON0.2
#bit    CHS1 = ADCON0.3
#bit    CHS2 = ADCON0.4
#bit    CHS3 = ADCON0.5
#bit    CHS4 = ADCON0.6
#byte ADCON1 = 0xFC1
#bit    CHSN0 = ADCON1.0
#bit    CHSN1 = ADCON1.1
#bit    CHSN2 = ADCON1.2
#bit    VNCFG = ADCON1.3
#bit    VCFG0 = ADCON1.4
#bit    VCFG1 = ADCON1.5
#bit    TRIGSEL0 = ADCON1.6
#bit    TRIGSEL1 = ADCON1.7
#byte ADCON2 = 0xFC0
#bit    ADCS0 = ADCON2.0
#bit    ADCS1 = ADCON2.1
#bit    ADCS2 = ADCON2.2
#bit    ACQT0 = ADCON2.3
#bit    ACQT1 = ADCON2.4
#bit    ACQT2 = ADCON2.5
#bit    ADFM = ADCON2.7
#byte IPR4 = 0xFB8
#bit    CCP3IP = IPR4.0
#bit    CCP4IP = IPR4.1
#bit    CCP5IP = IPR4.2
#bit    CMP1IP = IPR4.4
#bit    CMP2IP = IPR4.5
#bit    EEIP = IPR4.6
#bit    TMR4IP = IPR4.7
#byte PIR4 = 0xFB7
#bit    CCP3IF = PIR4.0
#bit    CCP4IF = PIR4.1
#bit    CCP5IF = PIR4.2
#bit    CMP1IF = PIR4.4
#bit    CMP2IF = PIR4.5
#bit    EEIF = PIR4.6
#bit    TMR4IF = PIR4.7
#byte PIE4 = 0xFB6
#bit    CCP3IE = PIE4.0
#bit    CCP4IE = PIE4.1
#bit    CCP5IE = PIE4.2
#bit    CMP1IE = PIE4.4
#bit    CMP2IE = PIE4.5
#bit    EEIE = PIE4.6
#bit    TMR4IE = PIE4.7
#byte HLVDCON = 0xFA8
#bit    HLVDL0 = HLVDCON.0
#bit    HLVDL1 = HLVDCON.1
#bit    HLVDL2 = HLVDCON.2
#bit    HLVDL3 = HLVDCON.3
#bit    HLVDEN = HLVDCON.4
#bit    IRVST = HLVDCON.5
#bit    BGVST = HLVDCON.6
#bit    VDIRMAG = HLVDCON.7
#byte IPR3 = 0xFA5
#bit    CCP1IP = IPR3.1
#bit    CCP2IP = IPR3.2
#bit    CTMUIP = IPR3.3
#bit    TX2IP = IPR3.4
#bit    RC2IP = IPR3.5
#byte PIR3 = 0xFA4
#bit    CCP1IF = PIR3.1
#bit    CCP2IF = PIR3.2
#bit    CTMUIF = PIR3.3
#bit    TX2IF = PIR3.4
#bit    RC2IF = PIR3.5
#byte PIE3 = 0xFA3
#bit    CCP1IE = PIE3.1
#bit    CCP2IE = PIE3.2
#bit    CTMUIE = PIE3.3
#bit    TX2IE = PIE3.4
#bit    RC2IE = PIE3.5
#byte IPR2 = 0xFA2
#bit    LVDIP = IPR2.2
#byte IPR2 = 0xFA2
#bit    TMR3GIP = IPR2.0
#bit    TMR3IP = IPR2.1
#bit    HLVDIP = IPR2.2
#bit    BCLIP = IPR2.3
#bit    OSCFIP = IPR2.7
#byte PIR2 = 0xFA1
#bit    LVDIF = PIR2.2
#byte PIR2 = 0xFA1
#bit    TMR3GIF = PIR2.0
#bit    TMR3IF = PIR2.1
#bit    HLVDIF = PIR2.2
#bit    BCLIF = PIR2.3
#bit    OSCFIF = PIR2.7
#byte PIE2 = 0xFA0
#bit    LVDIE = PIE2.2
#byte PIE2 = 0xFA0
#bit    TMR3GIE = PIE2.0
#bit    TMR3IE = PIE2.1
#bit    HLVDIE = PIE2.2
#bit    BCLIE = PIE2.3
#bit    OSCFIE = PIE2.7
#byte IPR1 = 0xF9F
#bit    TXIP = IPR1.4
#bit    RCIP = IPR1.5
#byte IPR1 = 0xF9F
#bit    TMR1IP = IPR1.0
#bit    TMR2IP = IPR1.1
#bit    TMR1GIP = IPR1.2
#bit    SSPIP = IPR1.3
#bit    TX1IP = IPR1.4
#bit    RC1IP = IPR1.5
#bit    ADIP = IPR1.6
#byte PIR1 = 0xF9E
#bit    TXIF = PIR1.4
#bit    RCIF = PIR1.5
#byte PIR1 = 0xF9E
#bit    TMR1IF = PIR1.0
#bit    TMR2IF = PIR1.1
#bit    TMR1GIF = PIR1.2
#bit    SSPIF = PIR1.3
#bit    TX1IF = PIR1.4
#bit    RC1IF = PIR1.5
#bit    ADIF = PIR1.6
#byte PIE1 = 0xF9D
#bit    TMR1IE = PIE1.0
#bit    TMR2IE = PIE1.1
#bit    TMR1GIE = PIE1.2
#bit    SSPIE = PIE1.3
#bit    TX1IE = PIE1.4
#bit    RC1IE = PIE1.5
#bit    ADIE = PIE1.6
#byte PIE1 = 0xF9D
#bit    TXIE = PIE1.4
#bit    RCIE = PIE1.5
#byte IPR5 = 0xF78
#bit    FIFOWMIP = IPR5.0
#bit    RXBnIP = IPR5.1
#bit    TXBnIP = IPR5.4
#byte IPR5 = 0xF78
#bit    RXB0IP = IPR5.0
#bit    RXB1IP = IPR5.1
#bit    TXB0IP = IPR5.2
#bit    TXB1IP = IPR5.3
#bit    TXB2IP = IPR5.4
#bit    ERRIP = IPR5.5
#bit    WAKIP = IPR5.6
#bit    IRXIP = IPR5.7
#byte PIR5 = 0xF77
#bit    FIFOWMIF = PIR5.0
#bit    RXBnIF = PIR5.1
#bit    TXBnIF = PIR5.4
#byte PIR5 = 0xF77
#bit    RXB0IF = PIR5.0
#bit    RXB1IF = PIR5.1
#bit    TXB0IF = PIR5.2
#bit    TXB1IF = PIR5.3
#bit    TXB2IF = PIR5.4
#bit    ERRIF = PIR5.5
#bit    WAKIF = PIR5.6
#bit    IRXIF = PIR5.7
#byte PIE5 = 0xF76
#bit    FIFOWMIE = PIE5.0
#bit    RXBnIE = PIE5.1
#bit    TXBnIE = PIE5.4
#byte PIE5 = 0xF76
#bit    RXB0IE = PIE5.0
#bit    RXB1IE = PIE5.1
#bit    TXB0IE = PIE5.2
#bit    TXB1IE = PIE5.3
#bit    TXB2IE = PIE5.4
#bit    ERRIE = PIE5.5
#bit    WAKIE = PIE5.6
#bit    IRXIE = PIE5.7
#byte ANCON0 = 0xF5D
#byte ANCON1 = 0xF5C
#bit    ANSEL8 = ANCON1.0
#bit    ANSEL9 = ANCON1.1
#bit    ANSEL10 = ANCON1.2
#byte PADCFG1 = 0xF56
#bit    CTMUDS = PADCFG1.0
#byte CTMUCONH = 0xF55
#bit    CTTRIG = CTMUCONH.0
#bit    IDISSEN = CTMUCONH.1
#bit    EDGSEQEN = CTMUCONH.2
#bit    EDGEN = CTMUCONH.3
#bit    TGEN = CTMUCONH.4
#bit    CTMUSIDL = CTMUCONH.5
#bit    CTMUEN = CTMUCONH.7
#byte CTMUCONL = 0xF54
#bit    EDG1STAT = CTMUCONL.0
#bit    EDG2STAT = CTMUCONL.1
#bit    EDG1SEL0 = CTMUCONL.2
#bit    EDG1SEL1 = CTMUCONL.3
#bit    EDG1POL = CTMUCONL.4
#bit    EDG2SEL0 = CTMUCONL.5
#bit    EDG2SEL1 = CTMUCONL.6
#bit    EDG2POL = CTMUCONL.7
#byte CTMUICON = 0xF53
#bit    IRNG0 = CTMUICON.0
#bit    IRNG1 = CTMUICON.1
#bit    ITRIM0 = CTMUICON.2
#bit    ITRIM1 = CTMUICON.3
#bit    ITRIM2 = CTMUICON.4
#bit    ITRIM3 = CTMUICON.5
#bit    ITRIM4 = CTMUICON.6
#bit    ITRIM5 = CTMUICON.7