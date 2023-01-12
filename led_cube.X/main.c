#include <xc.h>
#include <pic18f4520.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define _XTAL_FREQ 4000000
#pragma config OSC = INTIO67    // Oscillator Selection bits
#pragma config WDT = OFF        // Watchdog Timer Enable bit 
#pragma config PWRT = OFF       // Power-up Enable bit
#pragma config BOREN = ON       // Brown-out Reset Enable bit
#pragma config PBADEN = OFF     // Watchdog Timer Enable bit 
#pragma config LVP = OFF        // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF        // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)
#define delay __delay_ms

int plane[5][5];
int mode = 0;
int flag = 0;

void init(){
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    TRISEbits.TRISE1 = 1; // base platform
    TRISEbits.TRISE2 = 1; // second platform
    TRISAbits.TRISA7 = 1;
    TRISAbits.TRISA6 = 1;
    TRISCbits.TRISC0 = 1;
    
    OSCCONbits.IRCF = 0b110;
    TRISAbits.RA5 = 1;
    ADCON1bits.VCFG0 = 0; 
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0b0101;
    ADCON0bits.CHS = 0b0101;
    
    ADCON2bits.ADCS = 0b100;
    ADCON2bits.ACQT = 0b010;
    ADCON0bits.ADON = 1;
    ADCON2bits.ADFM = 1;
    // enable A/D interrupt
    PIE1bits.ADIE = 1;
    // clear A/D interrupt flag bit
    PIR1bits.ADIF = 0;
    // enable peripheral interrupt
    INTCONbits.PEIE = 1;
    // global interrupt enabled
    INTCONbits.GIE = 1;
    ADCON0bits.GO = 1;
}

void draw_plane(){
    LATAbits.LA0 = plane[0][0];
    LATAbits.LA1 = plane[0][1];
    LATAbits.LA2 = plane[0][2];
    LATAbits.LA3 = plane[0][3];
    LATAbits.LA4 = plane[0][4];
    
    LATCbits.LC1 = plane[1][0];
    LATCbits.LC2 = plane[1][1];
    LATCbits.LC3 = plane[1][2];
    LATDbits.LD0 = plane[1][3];
    LATDbits.LD1 = plane[1][4];
    
    LATBbits.LB0 = plane[2][0];
    LATBbits.LB1 = plane[2][1];
    LATBbits.LB2 = plane[2][2];
    LATBbits.LB3 = plane[2][3];
    LATBbits.LB4 = plane[2][4];
    
    LATCbits.LC7 = plane[3][0];
    LATDbits.LD4 = plane[3][1];
    LATDbits.LD5 = plane[3][2];
    LATDbits.LD6 = plane[3][3];
    LATDbits.LD7 = plane[3][4];
    
    LATDbits.LD2 = plane[4][0];
    LATDbits.LD3 = plane[4][1];
    LATCbits.LC4 = plane[4][2];
    LATCbits.LC5 = plane[4][3];
    LATCbits.LC6 = plane[4][4];
}

void clear_plane(){
    for(int i = 0; i < 5; i++)
        for(int j = 0; j < 5; j++)
            plane[i][j] = 0;
    draw_plane();
}

void plane_on(int a){
    switch (a){
        case 0: 
            TRISEbits.TRISE1 = 0;
            LATEbits.LE1 = 1;
            break;
        case 1: 
            TRISEbits.TRISE2 = 0;
            LATEbits.LE2 = 1;
            break;
        case 2:             
            TRISAbits.TRISA7 = 0;
            LATAbits.LA7 = 1;
            break;
        case 3: 
            TRISAbits.TRISA6 = 0;
            LATAbits.LA6 = 1;
            break;
        case 4: 
            TRISCbits.TRISC0 = 0;
            LATCbits.LC0 = 1 ;
            break;
        default:
            TRISEbits.TRISE1 = 1; // base platform
            TRISEbits.TRISE2 = 1; // second platform
            TRISAbits.TRISA7 = 1;
            TRISAbits.TRISA6 = 1;
            TRISCbits.TRISC0 = 1;
            break;
    }
}

void set_plane(int a){
    TRISEbits.TRISE1 = 1; // base platform
    TRISEbits.TRISE2 = 1; // second platform
    TRISAbits.TRISA7 = 1;
    TRISAbits.TRISA6 = 1;
    TRISCbits.TRISC0 = 1;
    plane_on(a);
}



void clear_cube(){

    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    TRISEbits.TRISE1 = 1;
    TRISEbits.TRISE2 = 1;
    TRISAbits.TRISA7 = 1;
    TRISAbits.TRISA6 = 1;
    TRISCbits.TRISC0 = 1;
    for(int i = 0; i < 5; i++)
        for(int j = 0; j < 5; j++)
            plane[i][j] = 0;

}

void lighton(int x, int prev){
    set_plane(x/25);
    if(prev != -1)plane[(prev%25)/5][prev%5] = 0;
    if(x!=-1)plane[(x%25)/5][x%5] = 1;
    draw_plane();
}
int move(int x, int r){
    if(r == 0)x = (x%5 == 4) ? x-4 : x+1; //right
    else if(r == 1)x = (x%5 == 0) ? x+4 : x-1; //left
    else if(r == 2)x = (x%25 >= 20) ? x-20 : x+5; //back
    else if(r == 3)x = (x%25 < 5) ? x+20 : x-5; //front
    else if(r == 4)x = (x >= 100) ? x-100 : x+25; //high
    else if(r == 5)x = (x<25) ? x+100 : x-25; //low
    return x;
}

void random_pattern(int id){
    clear_plane();
    srand(time(NULL)); 
    int x = 0, r = 0, prevr, prevx = -1;
    lighton(0, prevx);
    r = rand() % 6;
    prevx = x;
    x = move(x,r);
    lighton(x,prevx);
    delay(2);
    while(mode == id){
        prevr = r;
        r=rand()%6;
        if(r==0||r==1)while(prevr+r==1)r=rand()%6;
        else if(r==2||r==3) while(prevr+r==5)r=rand()%6;
        else if(r==4||r==5) while(prevr+r==9)r=rand()%6;   
//        x = move(x,r);
        x=move(x,r);
        lighton(x,prevx);
        prevx = x;
        delay(2);
    }
}

void slash(int id){
    clear_cube();
    int x = 0;
    plane[0][0] = 1;
    while(mode == id){
        plane[x][x] = 0;
        draw_plane();
        x++;
        x %= 5;
        plane[x][x] = 1;
        set_plane(x);
        draw_plane();
    }
}

void antirainfall(int id){
    clear_cube();
    srand(time(NULL));
    int cnt=0, a[5]={0,0,0,0,0}, idx=0;
    for(int i=0;i<5;i++){
        a[i]=rand()%25;
        lighton(a[i],-1);
    }
    delay(200);
    while(mode == id){
        int tmp = a[idx];
        idx%=5;
        clear_plane();
        for(int i=1;i<5;i++){
            set_plane(i);
            lighton(tmp,tmp);
            tmp = move(tmp,4);
            delay(10);
        }
        a[idx++] = rand()%25;
        idx %= 5;
        set_plane(0);
        for(int i=0;i<5;i++)plane[a[i]/5][a[i]%5] = 1;
        draw_plane();
        delay(100);
    }
}

void hitler(int id){
    clear_cube();
    int x=0, it=5 ,cnt=0,itcnt=0;
    while(mode == id){
        clear_plane();
        draw_plane();
        if(x==0){
            if(cnt==0){
                plane[2][0] = 1;
                plane[2][1] = 1;
                plane[2][2] = 1;
                plane[2][4] = 1;
            }else if(cnt == 1){
                plane[2][2] = 1;
                plane[2][4] = 1;
            }else if(cnt == 2){
                plane[2][0] = 1;
                plane[2][1] = 1;
                plane[2][2] = 1;
                plane[2][3] = 1;
                plane[2][4] = 1;
            }else if(cnt == 3){
                plane[2][0] = 1;
                plane[2][2] = 1;
            }else if(cnt == 4){
                plane[2][0] = 1;
                plane[2][2] = 1;
                plane[2][3] = 1;
                plane[2][4] = 1;
            }
        }else if(x==1){
            if(cnt==0){
                plane[4][0] = 1;
                plane[3][1] = 1;
                plane[2][2] = 1;
                plane[0][4] = 1;
            }else if(cnt == 1){
                plane[2][2] = 1;
                plane[0][4] = 1;
            }else if(cnt == 2){
                plane[4][0] = 1;
                plane[3][1] = 1;
                plane[2][2] = 1;
                plane[1][3] = 1;
                plane[0][4] = 1;
            }else if(cnt == 3){
                plane[4][0] = 1;
                plane[2][2] = 1;
            }else if(cnt == 4){
                plane[4][0] = 1;
                plane[2][2] = 1;
                plane[1][3] = 1;
                plane[0][4] = 1;
            }
        }else if(x==2){
            if(cnt==0){
                plane[4][2] = 1;
                plane[3][2] = 1;
                plane[2][2] = 1;
                plane[0][2] = 1;
            }else if(cnt == 1){
                plane[2][2] = 1;
                plane[0][2] = 1;
            }else if(cnt == 2){
                plane[4][2] = 1;
                plane[3][2] = 1;
                plane[2][2] = 1;
                plane[1][2] = 1;
                plane[0][2] = 1;
            }else if(cnt == 3){
                plane[4][2] = 1;
                plane[2][2] = 1;
            }else if(cnt == 4){
                plane[4][2] = 1;
                plane[2][2] = 1;
                plane[1][2] = 1;
                plane[0][2] = 1;
            }
        }else if(x==3){
            if(cnt==0){
                plane[4][4] = 1;
                plane[3][3] = 1;
                plane[2][2] = 1;
                plane[0][0] = 1;
            }else if(cnt == 1){
                plane[2][2] = 1;
                plane[0][0] = 1;
            }else if(cnt == 2){
                plane[4][4] = 1;
                plane[3][3] = 1;
                plane[2][2] = 1;
                plane[1][1] = 1;
                plane[0][0] = 1;
            }else if(cnt == 3){
                plane[4][4] = 1;
                plane[2][2] = 1;
            }else if(cnt == 4){
                plane[4][4] = 1;
                plane[2][2] = 1;
                plane[1][1] = 1;
                plane[0][0] = 1;
            }
        }else if(x==4){
            if(cnt==0){
                plane[2][0] = 1;
                plane[2][2] = 1;
                plane[2][3] = 1;
                plane[2][4] = 1;
            }else if(cnt == 1){
                plane[2][0] = 1;
                plane[2][2] = 1;
            }else if(cnt == 2){
                plane[2][0] = 1;
                plane[2][1] = 1;
                plane[2][2] = 1;
                plane[2][3] = 1;
                plane[2][4] = 1;
            }else if(cnt == 3){
                plane[2][2] = 1;
                plane[2][4] = 1;
            }else if(cnt == 4){
                plane[2][0] = 1;
                plane[2][1] = 1;
                plane[2][2] = 1;
                plane[2][4] = 1;
            }
        }else if(x==5){
            if(cnt==0){
                plane[0][4] = 1;
                plane[1][3] = 1;
                plane[2][2] = 1;
                plane[4][0] = 1;
            }else if(cnt == 1){
                plane[2][2] = 1;
                plane[4][0] = 1;
            }else if(cnt == 2){
                plane[0][4] = 1;
                plane[1][3] = 1;
                plane[2][2] = 1;
                plane[3][1] = 1;
                plane[4][0] = 1;
            }else if(cnt == 3){
                plane[0][4] = 1;
                plane[2][2] = 1;
            }else if(cnt == 4){
                plane[0][4] = 1;
                plane[2][2] = 1;
                plane[3][1] = 1;
                plane[4][0] = 1;
            }
        }else if(x==6){
            if(cnt==0){
                plane[0][2] = 1;
                plane[1][2] = 1;
                plane[2][2] = 1;
                plane[4][2] = 1;
            }else if(cnt == 1){
                plane[2][2] = 1;
                plane[4][2] = 1;
            }else if(cnt == 2){
                plane[0][2] = 1;
                plane[1][2] = 1;
                plane[2][2] = 1;
                plane[3][2] = 1;
                plane[4][2] = 1;
            }else if(cnt == 3){
                plane[0][2] = 1;
                plane[2][2] = 1;
            }else if(cnt == 4){
                plane[0][2] = 1;
                plane[2][2] = 1;
                plane[3][2] = 1;
                plane[4][2] = 1;
            }
        }else if(x==7){
            if(cnt==0){
                plane[0][0] = 1;
                plane[1][1] = 1;
                plane[2][2] = 1;
                plane[4][4] = 1;
            }else if(cnt == 1){
                plane[2][2] = 1;
                plane[4][4] = 1;
            }else if(cnt == 2){
                plane[4][4] = 1;
                plane[3][3] = 1;
                plane[2][2] = 1;
                plane[1][1] = 1;
                plane[0][0] = 1;
            }else if(cnt == 3){
                plane[0][0] = 1;
                plane[2][2] = 1;
            }else if(cnt == 4){
                plane[4][4] = 1;
                plane[2][2] = 1;
                plane[3][3] = 1;
                plane[0][0] = 1;
            }
        }
        set_plane(cnt);
        draw_plane();
        cnt++;
        if(cnt == 5){
            cnt=0;
            itcnt++;
            if(itcnt>it){
                x++;
                itcnt=0;
            }
            x = x%8;
        }
    }
}

void scan(int id){
    clear_cube();

    while(mode == id){
        for(int i = 0; i < 5; i++)
            for(int j = 0; j < 5; j++)
                plane[i][j] = 1;
        //l-h
        for(int i = 0; i < 5; i++){
            set_plane(i);
            draw_plane();
            delay(8);
        }
        for(int i = 0; i < 4; i++){
            set_plane(4-i);
            draw_plane();
            delay(8);
        }
        for(int i = 0; i < 5; i++)
            plane_on(i);
        // f-b
        for(int i = 0; i < 5; i++){
            clear_plane();
            for(int j = 0; j < 5; j++){
                plane[i][j] = 1;
            }
            draw_plane();
            delay(8);
        }
        for(int i = 0; i < 4; i++){
            clear_plane();
            for(int j = 0; j < 5; j++){
                plane[4-i][j] = 1;
            }
            draw_plane();
            delay(8);
        }
        // l-r
        for(int i = 0; i < 5; i++){
            clear_plane();
            for(int j = 0; j < 5; j++){
                plane[j][i] = 1;
            }
            draw_plane();
            delay(8);
        }
        for(int i = 0; i < 4; i++){
            clear_plane();
            for(int j = 0; j < 5; j++){
                plane[j][4-i] = 1;
            }
            draw_plane();
            delay(8);
        }
    }        
}

void speedup(int id){
    clear_cube();
    int i = 0, dcnt = 0, del = 50;
    plane[0][0] = 1;
    plane[0][4] = 1;
    plane[4][0] = 1;
    plane[4][4] = 1;
    while(mode == id){
        if(i == 0 || i == 4){
            for(int j = 1; j < 4; j++){
                plane[j][0] = 1;
                plane[j][4] = 1;
                plane[0][j] = 1;
                plane[4][j] = 1;
            }
        }else{
            for(int j = 1; j < 4; j++){
                plane[j][0] = 0;
                plane[j][4] = 0;
                plane[0][j] = 0;
                plane[4][j] = 0;
            }
        }
        
        set_plane(5);
        draw_plane();
        set_plane(i);
        for(int j=0;j<del && mode == id;j++)delay(1);
        dcnt++;
        if(del>10)del-=2;
        else if(del>0 && dcnt == 10)del-=1;
        dcnt = dcnt%10;
        i++;
        i%=5;
    }
}

void cubeidle(int id){
    clear_cube();
    lighton(0,-1);
    while(mode == id){
        for(int i=1;i<125 && mode == id;i++){
            lighton(i,i-1);
            delay(10);
        }
        lighton(0,124);
        delay(2);
    }
//    while(1){
//        r = rand()%3;
//        if(r==0)for(int i=1;i<125;i++) lighton(i,i-1);
//        else if(r==1)
//        lighton(0,124);
//    }
}

void pulsing(int id){
    while(mode == id){
        clear_cube();
        delay(60);
        plane[2][2] = 1;
        draw_plane();
        plane_on(2);
        delay(30);
        for(int i = 1; i < 4; i++){
            plane[1][i] = 1;
            plane[3][i] = 1;
        }
        plane[2][1] = 1;
        plane[2][3] = 1;
        draw_plane();
        plane_on(1);
        plane_on(3);
        delay(40);
        for(int i = 0; i < 5; i++){
            for(int j = 0; j < 5; j++)
                plane[i][j] = 1;
        }
        plane_on(0);
        plane_on(4);
        draw_plane();
        delay(70);
        clear_cube();
        for(int i = 1; i < 4; i++){
            plane[1][i] = 1;
            plane[2][i] = 1;
            plane[3][i] = 1;
        }
        plane_on(1);
        plane_on(2);
        plane_on(3);
        draw_plane();
        delay(60);
        clear_cube();
        plane[2][2] = 1;
        draw_plane();
        plane_on(2);
        delay(50);
    }
}

void main(void) {
    init();

//    for(int i = 0; i < 5; i++)
//        for(int j = 0; j < 5; j++)
//            plane[i][j] = 1;
    while(1){
//        draw_plane();
        switch(mode){
            case 0:
                speedup(0);
                break;
            case 1:
                cubeidle(1);
                break;
            case 2:
                pulsing(2);
                break;
            case 3:
                random_pattern(3);
                break;
            case 4:
                hitler(4);
                break;
            case 5:
                scan(5);
                break;
            case 6:
                antirainfall(6);
            default:
                clear_cube();
                break;
        }

    };
    return;
}

void __interrupt(high_priority)H_ISR(){
    
    int value = ((ADRESH<<8) + ADRESL);

    //do things, map 1024 to 0-4
    unsigned char mapResult = 0;
    if(!flag && value > 200){
        flag = 1;
        mode++;
        mode%=7;
    }else if (flag && value < 20){
        flag = 0;
    }
//    while (value > 140) {
//        value -= 140;
//        mapResult += 1;
//    }
//    mode = mapResult;
//    if(value<3) mode = 0;
//    else if(value<6)mode = 1;
//    else if(value<9)mode = 2;
//    else if(value<15)mode = 3;
//    else if(value<20)mode = 4;
//    else if(value<30)mode = 5;
    
    //clear flag bit
    PIR1bits.ADIF = 0;
    
    //step5 & go back step3
//    delay at least 2tad
    __delay_us(2);

    ADCON0bits.GO = 1;

    return;
}