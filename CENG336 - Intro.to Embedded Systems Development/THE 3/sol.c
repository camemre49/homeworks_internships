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

#include <stdint.h>
#include <stdio.h>

#include <xc.h>

void clearRegisters();


// Enumeration to handle simulator modes
typedef enum SimulatorMode
{
    IDLE, ACTIVE, END
} SimulatorMode;
SimulatorMode simulatorMode;

// Distance to be traveled, set by the $GOOXXXX# command
int targetDistance;

// Speed of the plane
int flySpeed;

// Flag to determine if manuel mode is enabled
uint8_t isManuelModeEnabled;

uint8_t timerFlag100ms;

// Buffer to store the last received command
char lastCommand[8];
// Current index of the byte being read in the command buffer
char currentCommandIndex;
// Flag to indicate that the command has been fully received
uint8_t isCommandReady;

// Buffer to store the last command will be sent
char lastResponse[10];
uint8_t currentResponseIndex;
uint8_t terminateTransmit;

// Send button response flag
char sendButtonResponseFlag;
// "$PRS" + 2bytes for button code + '#' + '\0' = 8 bytes
char buttonResponse[8];

uint8_t task100ms;

// Last received analog value
unsigned int lastAnalogValue;
// If true, read analog values
char readAnalogFlag;
// Altitude determined by analog value
unsigned int correspondingAltitude;
// "$ALT" + 4bytes for ADC Res + '#' + '\0' = 10 bytes
char altitudeResponse[10];
// Variable to hold altitude response period, period is (altitudeResponsePeriod * 100)ms
int altitudeResponsePeriod;
// Counter of altitude response period
int altitudePeriodCounter;
// Send altitude response flag
char sendAltitudeResponseFlag;


/**
 * Initialization function
 */
void initialConfigurations(void)
{
    INTCONbits.GIE = 0;
    clearRegisters();
    
    // PORTB<4-7> is input to set leds
    TRISB = 0xF0;
    // PORTC<7> is input for serial communications
    TRISC = 0x80; 
    
    
    // BRG/EUSART Mode is asynchronous
    TXSTA1bits.SYNC = 0;
    // BRG/EUSART Mode is 8-bit
    TXSTA1bits.BRGH = 1;
    // Set as 21 for a Baud Rate ~= 115.2
    SPBRG1 = 21;
    // 8 bit baud rate generator
    BAUDCON1bits.BRG16 = 0;
    
    // set RC6/7 as serial communications pins
    RCSTA1bits.SPEN = 1;
    // set to start receiving data
    RCSTA1bits.CREN = 1;
    
    // Set timer prescale to 1:4
    T0CON = 0b00000100;
    // Set an initial value to create a timer for 100ms 
    TMR0 = 34288;
    
    // Set ADC Inputs
    TRISH = 0x10; // AN12 input RH4
    // Configure ADC
    ADCON0 = 0x30; // Channel 12;
    ADCON0bits.ADON = 1; //Turn on AD Converter
    ADCON1 = 0x00; // All analog pins
    ADCON2 = 0xAA; // Right Align | 12 Tad | Fosc/32
    sprintf(altitudeResponse, "$ALT");
    altitudeResponse[9] = 0;

    // Enable PORTB<4-7> interrupts and clear its flag
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
    // Enable timer interrupts and clear its flag
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
    // Enable peripheral interrupts and clear its flag
    INTCONbits.PEIE = 1;
    // Enable transmission interrupts and clear its flag
    PIE1bits.TX1IE = 1;
    PIR1bits.TX1IF = 0;
    // Enable reception interrupts and clear its flag
    PIE1bits.RC1IE = 1;
    PIR1bits.RC1IF = 0;
    // Enable global interrupts and clear its flag
    INTCONbits.GIE = 1;
}

/**
 * Clear all registers to be used.
 */
void clearRegisters() {
    simulatorMode = IDLE;
    targetDistance = 120;
    flySpeed = 0;
    isManuelModeEnabled = 0;
    timerFlag100ms = 0;
    currentCommandIndex = 0;
    isCommandReady = 0;
    currentResponseIndex = 0;
    terminateTransmit = 0;
    sendButtonResponseFlag = 0;
    lastAnalogValue = 0;
    correspondingAltitude = 0;
    altitudeResponsePeriod = 0;
    task100ms = 0;
    PORTA = 0x00; LATA = 0x00; TRISA = 0x00;
    PORTB = 0x00; LATB = 0x00; TRISB = 0x00;
    PORTC = 0x00; LATC = 0x00; TRISC = 0x00; 
    PORTD = 0x00; LATD = 0x00; TRISD = 0x00;
    BAUDCON1 = 0x00;
    SPBRG1 = 0x00;
    TXSTA1 = 0x00;
    RCSTA1 = 0x00;
    T0CON = 0x00;
    PIR1 = 0x00;
    ADCON0 = 0x00;
    ADCON1 = 0x00;
    ADCON2 = 0x00;
    ADRESH = 0x00;
    ADRESL = 0x00;
    return;
}

void handleCommand()
{   
    // If command is not ready yet, return
    if (!isCommandReady)
        return;
    isCommandReady = 0;
    
    // Convert hexadecimal chars in the last command into integer
    char* ptr;
    int valueInLastCommand = strtol(lastCommand + 3, &ptr, 16);
    
    // In IDLE mode, only valid command is $GOOXXXX#
    if (simulatorMode == IDLE)
    {
        if (lastCommand[0] == 'G' && lastCommand[1] == 'O' && lastCommand[2] == 'O') //$GOOXXXX#
        {
            // Enable timer when GO command is received to ensure send commands for 100ms precisely
            T0CONbits.TMR0ON = 1;
            simulatorMode = ACTIVE;
            targetDistance = valueInLastCommand;
        }
    }
    // If simulator mode is ACTIVE
    else
    {
        switch (lastCommand[0])
        {
            case 'E': //$END#
                if(!(lastCommand[1] == 'N' && lastCommand[2] == 'D')) {
                    break;
                }
                simulatorMode = END;
                break;
                
            case 'S': //$SPDXXXX#
                if(!(lastCommand[1] == 'P' && lastCommand[2] == 'D')) {
                    break;
                }
                flySpeed = valueInLastCommand;
                break;
                
            case 'A': //$ALTXXXX#
                if(!(lastCommand[1] == 'L' && lastCommand[2] == 'T')) {
                    break;
                }
                altitudeResponsePeriod = valueInLastCommand / 100;
                altitudePeriodCounter = 0;
                if(altitudeResponsePeriod > 0) {
                    readAnalogFlag = 1;
                }
                else {
                    readAnalogFlag = 0;
                }
                break;
                
            case 'M': //$MANXX#
                if(!(lastCommand[1] == 'A' && lastCommand[2] == 'N')) {
                    break;
                }
                isManuelModeEnabled = valueInLastCommand;
                break;
                
            case 'L': //$LEDXX#
                if(!(lastCommand[1] == 'E' && lastCommand[2] == 'D')) {
                    break;
                }
                if (!isManuelModeEnabled)
                    break;
                
                switch (valueInLastCommand)
                {
                    case 0:
                        LATAbits.LA0 = 0;
                        LATBbits.LB0 = 0;
                        LATCbits.LC0 = 0;
                        LATDbits.LD0 = 0;
                        break;
                        
                    case 1:
                        LATDbits.LD0 = 1;
                        break;
                        
                    case 2:
                        LATCbits.LC0 = 1;
                        break;
                        
                    case 3:
                        LATBbits.LB0 = 1;
                        break;
                        
                    case 4:
                        LATAbits.LA0 = 1;
                        break;
                }
                break;
                
            default:
                break;
        }
    }
}

/**
 * Generate a response and send to autopilot
 */
void sendDistance()
{
    // If 100ms is not passed, return
    // No need to check altitude and button response flags since if they would be set, they would clear the timer flag
    if (!timerFlag100ms)
        return;
    timerFlag100ms = 0;
    
    if (task100ms)
    {
        targetDistance -= flySpeed;
        if(targetDistance < 0)
            targetDistance = 0;
        
         // If analog read flag is set, count for altitude response and enable send altitude response flag
        if(readAnalogFlag) {
            altitudePeriodCounter++;
            if(altitudePeriodCounter == altitudeResponsePeriod) {
                sendAltitudeResponseFlag = 1;
                altitudePeriodCounter = 0;
            }
        }

        task100ms = 0;
    }
    
    // Start response transmission
    sprintf(lastResponse, "$DST%04x#", targetDistance);
    TXREG1 = lastResponse[currentResponseIndex++];
    TXSTA1bits.TXEN = 1;
}


/**
 * Send the saved button response to autopilot
 */
void sendButtonResponse() {
    
    // If button response flag is not set, return
    if(!sendButtonResponseFlag || !isManuelModeEnabled || !timerFlag100ms) {
        return;
    }
    sendButtonResponseFlag = 0;
    timerFlag100ms = 0;
    
    // Start response transmission
    sprintf(lastResponse, buttonResponse);
    TXSTA1bits.TXEN = 1;
    return;
}

/*
 * Send the saved analog response to autopilot
 */
void sendAnalogResponse(){
    
    // If send altitude response flag is not set, return
    // This flag already checks timer flag, no need to check again
    
    if (task100ms)
    {
        targetDistance -= flySpeed;
        if(targetDistance < 0)
            targetDistance = 0;
        
         // If analog read flag is set, count for altitude response and enable send altitude response flag
        if(readAnalogFlag) {
            altitudePeriodCounter++;
            if(altitudePeriodCounter == altitudeResponsePeriod) {
                sendAltitudeResponseFlag = 1;
                altitudePeriodCounter = 0;
            }
        }

        task100ms = 0;
    }
    
    if(!sendAltitudeResponseFlag){
        return;
    }
    
    sendAltitudeResponseFlag = 0;
    timerFlag100ms = 0;
    
    // Start response transmission
    sprintf(lastResponse, altitudeResponse);
    TXSTA1bits.TXEN = 1;
    return;
}

/**
 * Reads analog value when necessary and store it
 */
void readAnalogValue() {
   // If read analog flag is not enabled, return
    if(!readAnalogFlag){
        return;
    }
    
    // Get ADC Sample
    GODONE = 1; // Start ADC conversion
    while(GODONE); // Poll and wait for conversion to finish.
    lastAnalogValue = (ADRESH << 8) + ADRESL; // Get the result;
    
    if(lastAnalogValue < 256) {
        correspondingAltitude = 9000;
    }
    else if(lastAnalogValue < 512) {
        correspondingAltitude = 10000;
    }
    else if(lastAnalogValue < 768) {
        correspondingAltitude = 11000;
    }
    else if(lastAnalogValue < 1024) {
        correspondingAltitude = 12000;
    }
    
    sprintf(altitudeResponse + 4, "%04X", correspondingAltitude);
    // sprintf terminates string with null, put '#'
    altitudeResponse[8] = '#';
    return;
}

/**
 * High priority ISR
 */
__interrupt(high_priority)
void handleHighInterrupt()
{   
    // Handle PORTB<4-7> presses in manuel mode and clear the flag
    static uint8_t rb4_pressed = 0;
    static uint8_t rb5_pressed = 0;
    static uint8_t rb6_pressed = 0;
    static uint8_t rb7_pressed = 0;
    if(INTCONbits.RBIF) {
        if (PORTBbits.RB4 && !rb4_pressed)
            rb4_pressed = 1;
                
        if (PORTBbits.RB5 && !rb5_pressed)
            rb5_pressed = 1;
        
        if (PORTBbits.RB6 && !rb6_pressed)
            rb6_pressed = 1;
        
        if (PORTBbits.RB7 && !rb7_pressed)
            rb7_pressed = 1;
        
        if (rb4_pressed && !PORTBbits.RB4)
        {
            if (isManuelModeEnabled)
            {            sendButtonResponseFlag = 1;
            sprintf(buttonResponse, "$PRS04#");
            LATDbits.LATD0 = 0;}

            rb4_pressed = 0;
        }
        
        if (rb5_pressed && !PORTBbits.RB5)
        {
            if (isManuelModeEnabled){
            sendButtonResponseFlag = 1;
            sprintf(buttonResponse, "$PRS05#");
            LATCbits.LATC0 = 0;
            }
            rb5_pressed = 0;
        }

        if (rb6_pressed && !PORTBbits.RB6)
        {
            if (isManuelModeEnabled){
            sendButtonResponseFlag = 1;
            sprintf(buttonResponse, "$PRS06#");
            LATBbits.LATB0 = 0;}
            rb6_pressed = 0;
        }
        
        if (rb7_pressed && !PORTBbits.RB7)
        {
            if (isManuelModeEnabled){
            sendButtonResponseFlag = 1;
            sprintf(buttonResponse, "$PRS07#");
            LATAbits.LATA0 = 0;
            }
            rb7_pressed = 0;
        }
        
        INTCONbits.RBIF = 0;
    }
    
    // Handle the timer interrupt and configure it again for 100ms
    if (INTCONbits.TMR0IF)
    {
        timerFlag100ms = 1;
        task100ms = 1;
        TMR0 = 34288;
        INTCONbits.TMR0IF = 0;
    }
    
    // Handle command receive interrupt and clear the flag
    if (PIR1bits.RC1IF)
    {   
        // If '$' is read, start to receive command
        if (RCREG1 == '$')
        {
            currentCommandIndex = 0;
        }
        // If '#' is read, end the command receipt
        else if (RCREG1 == '#')
        {
            lastCommand[currentCommandIndex] = '\0';
            isCommandReady = 1;
        }
        // Read the current byte of the command
        else
        {
            lastCommand[currentCommandIndex++] = RCREG1;
        }
        
        PIR1bits.RC1IF = 0;
    }
    
    // Handle transmission to autopilot
    if (PIR1bits.TX1IF) 
    {
        if (terminateTransmit && TXREG1 != '#')
        {
            terminateTransmit = 0;
            currentResponseIndex = 0;
            TXSTA1bits.TXEN = 0;
        }
        else
        {
            if (lastResponse[currentResponseIndex] == '#')
                terminateTransmit = 1;
            
            // Send the next byte
            TXREG1 = lastResponse[currentResponseIndex++];
        }
    }
}

void main(void) {
    initialConfigurations();

    while (simulatorMode != END)
    {
        if (task100ms)
        {
            // Substract speed from target distance at every 100ms
            targetDistance -= flySpeed;
            if(targetDistance < 0)
                targetDistance = 0;
            // If analog read flag is set, count for altitude response and enable send altitude response flag
            if(readAnalogFlag) {
                altitudePeriodCounter++;
                if(altitudePeriodCounter == altitudeResponsePeriod) {
                    sendAltitudeResponseFlag = 1;
                    altitudePeriodCounter = 0;
                }
            }
        
            task100ms = 0;
        }
        handleCommand();
        
        if(simulatorMode == ACTIVE) {
            // First priority: Send analog response if necessary
            readAnalogValue();
            sendAnalogResponse();

            // Second priority: Send button response if necessary
            sendButtonResponse();

            // Least proiorty: Send distance response if necessary
            sendDistance();
        }
        
        if (RCSTA1bits.OERR || RCSTA1bits.FERR)
        {
            RCSTA1bits.CREN = 0;
            RCSTA1bits.CREN = 1;
        }
    }
    
    while(1) {
        // End loop
    }
}
