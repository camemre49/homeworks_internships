// ============================ //
// Do not edit this part!!!!    //
// ============================ //
// 0x300001 - CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator,
                                // PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit
                                // (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit
                                // (Oscillator Switchover mode disabled)
// 0x300002 - CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out
                                // Reset disabled in hardware and software)
// 0x300003 - CONFIG1H
#pragma config WDT = OFF        // Watchdog Timer Enable bit
                                // (WDT disabled (control is placed on the SWDTEN bit))
// 0x300004 - CONFIG3L
// 0x300005 - CONFIG3H
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit
                                // (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled;
                                // RE3 input pin disabled)
// 0x300006 - CONFIG4L
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply
                                // ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit
                                // (Instruction set extension and Indexed
                                // Addressing mode disabled (Legacy mode))

#pragma config DEBUG = OFF      // Disable In-Circuit Debugger

#define KHZ 1000UL
#define MHZ (KHZ * KHZ)
#define _XTAL_FREQ (40UL * MHZ)

// ============================ //
//             End              //
// ============================ //

#include <xc.h>
#include <stdint.h>

// ============================ //
//        DEFINITIONS           //
// ============================ //

// You can write struct definitions here...

typedef struct Piece {
    uint8_t x;
    uint8_t y;
    uint8_t rotation;
    uint8_t shape;
    uint8_t submitted;
    uint8_t show;
} Piece;

// ============================ //
//          GLOBALS             //
// ============================ //

// You can write globals definitions here...

uint8_t filledBlocks = 0;

Piece activePiece = {0, 0, 0, 0, 0, 1};

uint8_t mapActivePieceC = 0;
uint8_t mapActivePieceD = 0;
uint8_t mapActivePieceE = 0;
uint8_t mapActivePieceF = 0;

uint8_t mapOriginalC = 0;
uint8_t mapOriginalD = 0;
uint8_t mapOriginalE = 0;
uint8_t mapOriginalF = 0;

uint8_t tryAndSubmit = 0;
uint8_t rotate = 0;
uint8_t timerEvent = 0;

uint8_t counter = 0;

// ============================ //
//          FUNCTIONS           //
// ============================ //

// You can write function definitions here...

void ShowOn7SHelper(uint8_t d) {
    switch(d) {
        case 0:
            PORTJ = 0b0111111;
            break;
            
        case 1:
            PORTJ = 0b110;
            break;
            
        case 2:
            PORTJ = 0b1011011;
            break;
            
        case 3:
            PORTJ = 0b1001111;
            break;
            
        case 4:
            PORTJ = 0b1100110;
            break;
            
        case 5:
            PORTJ = 0b1101101;
            break;
            
        case 6:
            PORTJ = 0b1111101;
            break;
            
        case 7:
            PORTJ = 0b111;
            break;
            
        case 8:
            PORTJ = 0b1111111;
            break;
            
        case 9:
            PORTJ = 0b1101111;
            break;       
    }
}

void ShowOn7S()
{
    static uint8_t turn = 0;

    switch (turn)
    {
      case 0:
          PORTHbits.RH3 = 0;
          ShowOn7SHelper(0);
          PORTHbits.RH0 = 1;
          turn = 1;
          break;
      case 1:
          PORTHbits.RH0 = 0;
          PORTHbits.RH1 = 1;
          turn = 2;
          break;
      case 2:
          PORTHbits.RH1 = 0;
          ShowOn7SHelper(filledBlocks / 10);
          PORTHbits.RH2 = 1;
          turn = 3;
          break;
      case 3:
          PORTHbits.RH2 = 0;
          ShowOn7SHelper(filledBlocks % 10);
          PORTHbits.RH3 = 1;
          turn = 0;
          break;
    }
}

void Init()
{
    // turn off interrupts
    INTCONbits.GIE = 0; 
    
    // button inputs
    PORTB = 0x00; LATB = 0x00; TRISB = 0x60;
    PORTG = 0x00; LATG = 0x00; TRISG = 0x0F;
    
    // led outputs
    PORTC = 0x00; LATC = 0x00; TRISC = 0x00;
    PORTD = 0x00; LATD = 0x00; TRISD = 0x00;
    PORTE = 0x00; LATE = 0x00; TRISE = 0x00;
    PORTF = 0x00; LATF = 0x00; TRISF = 0x00;
    
    // 7s outputs
    PORTH = 0x00; LATH = 0x00; TRISH = 0x00;
    PORTJ = 0x00; LATJ = 0x00; TRISJ = 0x00;
    
    ShowOn7SHelper(0);
    PORTHbits.RH0 = 1;
    PORTHbits.RH1 = 1;
    PORTHbits.RH2 = 1;
    PORTHbits.RH3 = 1;
    
    __delay_ms(1000);
    
    PORTHbits.RH0 = 0;
    PORTHbits.RH1 = 0;
    PORTHbits.RH2 = 0;
    PORTHbits.RH3 = 0;
}

void InitInterrupts() {
    RCONbits.IPEN = 0;      // No priority
    INTCONbits.RBIE = 1;    // Enable RB<4-7> interrupts
    INTCONbits.TMR0IE = 1;
    T0CON = 0b10000110;
    TMR0 = 46000;
    INTCONbits.GIE = 1;     // Enable global interrupts
}

void MoveRight() {
    if (activePiece.shape == 0)
    {
        if (activePiece.x < 3)
            activePiece.x++;
    }
    
    else
    {
        if (activePiece.x < 2)
            activePiece.x++;
    }
}

void MoveLeft() {
    if(activePiece.x > 0)
        activePiece.x--;
}

void MoveUp() {
    if(activePiece.y > 0)
        activePiece.y--;
}

void MoveDown() {
    if (activePiece.shape == 0)
    {
        if (activePiece.y < 7)
            activePiece.y++;
    }
    
    else
    {
        if (activePiece.y < 6)
            activePiece.y++;
    }
}

void CheckPortG()
{
    static uint8_t rg0_pressed = 0;
    static uint8_t rg1_pressed = 0;
    static uint8_t rg2_pressed = 0;
    static uint8_t rg3_pressed = 0;
    
    if (PORTGbits.RG0 && !rg0_pressed)
        rg0_pressed = 1;

    if (PORTGbits.RG1 && !rg1_pressed)
        rg1_pressed = 1;
    
    if (PORTGbits.RG2 && !rg2_pressed)
        rg2_pressed = 1;
    
    if (PORTGbits.RG3 && !rg3_pressed)
        rg3_pressed = 1;
    
    if (rg0_pressed && !PORTGbits.RG0)
    {
        MoveRight();
        rg0_pressed = 0;
    }
    
    if (rg1_pressed && !PORTGbits.RG1)
    {
        MoveUp();
        rg1_pressed = 0;
    }
    
    if (rg2_pressed && !PORTGbits.RG2)
    {
        MoveDown();
        rg2_pressed = 0;
    }
    
    if (rg3_pressed && !PORTGbits.RG3)
    {
        MoveLeft();
        rg3_pressed = 0;
    }
}

void SubmitObject()
{
    if (tryAndSubmit && !(mapOriginalC & mapActivePieceC) && !(mapOriginalD & mapActivePieceD) && !(mapOriginalE & mapActivePieceE) && !(mapOriginalF & mapActivePieceF))
    {
        mapOriginalC |= mapActivePieceC;
        mapOriginalD |= mapActivePieceD;
        mapOriginalE |= mapActivePieceE;
        mapOriginalF |= mapActivePieceF;
        
        switch (activePiece.shape)
        {
            case 0:
                filledBlocks++;
                break;
            case 1:
                filledBlocks += 4;
                break;
            case 2:
                filledBlocks += 3;
                break;
        }

        activePiece.submitted = 1;
    }

    tryAndSubmit = 0;
}

void GenerateObject() {
    if (activePiece.submitted)
    {            
        activePiece.x = 0;
        activePiece.y = 0;
        activePiece.rotation = 0;
        activePiece.shape += 1;
        
        if (activePiece.shape == 3)
            activePiece.shape = 0;

        activePiece.submitted = 0;
        activePiece.show = 1;
        
        counter = 0;
        TMR0 = 46000;
    }

}

void RotateObject()
{
    if (rotate)
    {
        activePiece.rotation++;
        
        if (activePiece.rotation == 4)
            activePiece.rotation = 0;

        rotate = 0;
    }            
}

void TimerEvent()
{
    if (timerEvent)
    {
        activePiece.show = !activePiece.show;
        
        counter++;
        if (counter == 8)
        {
            counter = 0;
            MoveDown();
        }
        
        timerEvent = 0;
    }
}

void showPoint(uint8_t x, uint8_t y) {
    switch (x) {
        case 0:
            switch (y) {
                case 0:
                    mapActivePieceC |= 0b00000001;
                    break;
                case 1:
                    mapActivePieceC |= 0b00000010;
                    break;
                case 2:
                    mapActivePieceC |= 0b00000100;
                    break;
                case 3:
                    mapActivePieceC |= 0b00001000;
                    break;
                case 4:
                    mapActivePieceC |= 0b00010000;
                    break;
                case 5:
                    mapActivePieceC |= 0b00100000;
                    break;
                case 6:
                    mapActivePieceC |= 0b01000000;
                    break;
                case 7:
                    mapActivePieceC |= 0b10000000;
                    break;       
            }
            break;
        case 1:
            switch (y) {
                case 0:
                    mapActivePieceD |= 0b00000001;
                    break;
                case 1:
                    mapActivePieceD |= 0b00000010;
                    break;
                case 2:
                    mapActivePieceD |= 0b00000100;
                    break;
                case 3:
                    mapActivePieceD |= 0b00001000;
                    break;
                case 4:
                    mapActivePieceD |= 0b00010000;
                    break;
                case 5:
                    mapActivePieceD |= 0b00100000;
                    break;
                case 6:
                    mapActivePieceD |= 0b01000000;
                    break;
                case 7:
                    mapActivePieceD |= 0b10000000;
                    break;       
            }
            break;
        case 2:
            switch (y) {
                case 0:
                    mapActivePieceE |= 0b00000001;
                    break;
                case 1:
                    mapActivePieceE |= 0b00000010;
                    break;
                case 2:
                    mapActivePieceE |= 0b00000100;
                    break;
                case 3:
                    mapActivePieceE |= 0b00001000;
                    break;
                case 4:
                    mapActivePieceE |= 0b00010000;
                    break;
                case 5:
                    mapActivePieceE |= 0b00100000;
                    break;
                case 6:
                    mapActivePieceE |= 0b01000000;
                    break;
                case 7:
                    mapActivePieceE |= 0b10000000;
                    break;       
            }
            break;
        case 3:
            switch (y) {
                case 0:
                    mapActivePieceF |= 0b00000001;
                    break;
                case 1:
                    mapActivePieceF |= 0b00000010;
                    break;
                case 2:
                    mapActivePieceF |= 0b00000100;
                    break;
                case 3:
                    mapActivePieceF |= 0b00001000;
                    break;
                case 4:
                    mapActivePieceF |= 0b00010000;
                    break;
                case 5:
                    mapActivePieceF |= 0b00100000;
                    break;
                case 6:
                    mapActivePieceF |= 0b01000000;
                    break;
                case 7:
                    mapActivePieceF |= 0b10000000;
                    break;       
            }
            break;      
    }
}

void showActivePiece() {
    switch (activePiece.shape) {
        case 0:
            showPoint(activePiece.x, activePiece.y);
            break;
        case 1:
            showPoint(activePiece.x, activePiece.y);
            showPoint(activePiece.x + 1, activePiece.y);
            showPoint(activePiece.x, activePiece.y + 1);
            showPoint(activePiece.x + 1, activePiece.y + 1);
            break;
        case 2:
            switch (activePiece.rotation)
            {
                case 0:
                    showPoint(activePiece.x, activePiece.y);
                    showPoint(activePiece.x + 1, activePiece.y);
                    showPoint(activePiece.x + 1, activePiece.y + 1);
                    break;
                case 1:
                    showPoint(activePiece.x + 1, activePiece.y);
                    showPoint(activePiece.x, activePiece.y + 1);
                    showPoint(activePiece.x + 1, activePiece.y + 1);
                    break;
                case 2:
                    showPoint(activePiece.x, activePiece.y);
                    showPoint(activePiece.x, activePiece.y + 1);
                    showPoint(activePiece.x + 1, activePiece.y + 1);
                    break;
                case 3:
                    showPoint(activePiece.x, activePiece.y);
                    showPoint(activePiece.x + 1, activePiece.y);
                    showPoint(activePiece.x, activePiece.y + 1);
                    break;
            }
            break;
    }
}

void ShowLeds() {
    mapActivePieceC = 0;
    mapActivePieceD = 0;
    mapActivePieceE = 0;
    mapActivePieceF = 0;
    
    showActivePiece();
    
    if (activePiece.show)
    {
        LATC = mapOriginalC | mapActivePieceC;
        LATD = mapOriginalD | mapActivePieceD;
        LATE = mapOriginalE | mapActivePieceE;
        LATF = mapOriginalF | mapActivePieceF;
    }
    
    else 
    {
        LATC = mapOriginalC & ~mapActivePieceC;
        LATD = mapOriginalD & ~mapActivePieceD;
        LATE = mapOriginalE & ~mapActivePieceE;
        LATF = mapOriginalF & ~mapActivePieceF;
    }
}

// ============================ //
//   INTERRUPT SERVICE ROUTINE  //
// ============================ //
__interrupt(high_priority)
void HandleInterrupt()
{   
    static uint8_t rb5_pressed = 0;
    static uint8_t rb6_pressed = 0;
    
    if(INTCONbits.RBIF) {
        
        if (PORTBbits.RB5 && !rb5_pressed)
            rb5_pressed = 1;
        
        if (PORTBbits.RB6 && !rb6_pressed)
            rb6_pressed = 1;
        
        if (rb5_pressed && !PORTBbits.RB5)
        {
            rotate = 1;
            rb5_pressed = 0;
        }

        if (rb6_pressed && !PORTBbits.RB6)
        {
            tryAndSubmit = 1;
            rb6_pressed = 0;
        }
     
        INTCONbits.RBIF = 0;
    }
    
    if(INTCONbits.TMR0IF) {
        timerEvent = 1;
        TMR0 = 46000;
        INTCONbits.TMR0IF = 0;
    }
}

// ============================ //
//            MAIN              //
// ============================ //
void main()
{
    // Main ...
    Init();
    InitInterrupts();
    
    while (1)
    {
        CheckPortG();
        SubmitObject();
        GenerateObject();
        RotateObject();
        TimerEvent();
        ShowOn7S();
        ShowLeds();
    }
}
