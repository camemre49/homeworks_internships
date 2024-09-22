PROCESSOR 18F8722

#include <xc.inc>

; CONFIGURATION (DO NOT EDIT)
; CONFIG1H
CONFIG OSC = HSPLL      ; Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
CONFIG FCMEN = OFF      ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
CONFIG IESO = OFF       ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
; CONFIG2L
CONFIG PWRT = OFF       ; Power-up Timer Enable bit (PWRT disabled)
CONFIG BOREN = OFF      ; Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
; CONFIG2H
CONFIG WDT = OFF        ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
; CONFIG3H
CONFIG LPT1OSC = OFF    ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
CONFIG MCLRE = ON       ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
; CONFIG4L
CONFIG LVP = OFF        ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
CONFIG XINST = OFF      ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
CONFIG DEBUG = OFF      ; Disable In-Circuit Debugger


GLOBAL var0
GLOBAL var1
GLOBAL var2
GLOBAL re0_old
GLOBAL re1_old
GLOBAL is_enabled_PORTB
GLOBAL is_enabled_PORTC
GLOBAL counter
GLOBAL counter2

; Define space for the variables in RAM
PSECT udata_acs
var0:
  DS 1
var1:
  DS 1 ; Allocate 1 byte for var1
var2:
  DS 1   
re0_old:
  DS 1
re1_old:
  DS 1
counter:
  DS 1
counter2:
  DS 1
is_enabled_PORTB:
  DS 1
is_enabled_PORTC:
  DS 1
  
; Notes
  ; PORTB
  ; LATB
  ; TRISB determines whether the port is input/output
  ; When handling the output ports PORTBX and LATX is not matter. setf LATX == setf PORTX
  ; For input, PORTX must be used always


PSECT resetVec,class=CODE,reloc=2
resetVec:
  goto       main

PSECT CODE
main:	
  movlb 0 ; Put 0 to BSR
  clrf is_enabled_PORTB, b
  clrf is_enabled_PORTC, b
  call decide_io	; Determine input and output pins
  
  call light_up_outputs ; Set output pins to 1
  call buzy_wait	; Wait apprx 1 second
  call turn_off_leds	; Set output pins to 0
  
  clrf re0_old, b
  clrf re1_old, b
  clrf counter, b
  movlw 153
  movwf counter2, b ; counter2 = 170
  
infinite_loop:
  call check_buttons
  call update_display
  goto infinite_loop

    
decide_io:
  ; PORTB, PORTC, PORTD are output
  clrf TRISB, a
  clrf TRISC, a
  clrf TRISD, a
  ; PORTE is input
  setf TRISE, a
  clrf PORTE, a
  return

light_up_outputs:
  setf PORTB, a
  setf PORTC, a
  setf PORTD, a
  return

turn_off_leds:
  clrf PORTB, a
  clrf PORTC, a
  clrf PORTD, a
  return
  
buzy_wait:
  movlw 251
  movwf var0, b
  outer_outer_loop:
    clrf var1, b   ; var1 = 0
    outer_loop:	
      setf var2, b ; var2 = 255
      loop:
	decf var2, b ; decrease var2
	bnz loop
      incfsz var1, b
      bra outer_loop
    incfsz var0, b
    bra outer_outer_loop
  return
  
check_buttons:
  btfsc re0_old, 0, b ; check old value of RE0; if 0 check if it is turned from 1 to 0.
  rcall is_re0_toggled
  btfsc re1_old, 0, b ; check old value of RE1; if 0 check if it is turned from 1 to 0.
  rcall is_re1_toggled
  
  btfsc PORTE, 0, a ; check RE0; skip if 0
  setf re0_old, b
  btfsc PORTE, 1, a ; check RE1; skip if 0
  setf re1_old, b
  
  return
  
is_re0_toggled:
  btfss PORTE, 0, a ; check RE0; skip if 1
  rcall re0_toggled ; if 0; that means R0 is toggled
  return
  
is_re1_toggled:
  btfss PORTE, 1, a ; check RE1; skip if 1
  rcall re1_toggled ; if 0; that means RE1 is toggled
  return  
  
re0_toggled:
  clrf re0_old, b ; clear R0_old
  comf is_enabled_PORTC, b
  return
  
re1_toggled:
  clrf re1_old, b ; clear RE1_old
  comf is_enabled_PORTB, b
  return
  
  
update_display:
  incfsz counter, b ; skip if 0
  return
  counter_ov:
    incfsz counter2, b
    return
    counter_ov2:
      movwf counter2, b
      rcall pbPORTB
      rcall pbPORTC
      btg PORTD, 0, a
      return
      
pbPORTB:
  movf is_enabled_PORTB, f, b
  bnz enabled_PORTB
  disabled_PORTB:
    clrf PORTB, a
  continueB:
  return
  
pbPORTC:
  movf is_enabled_PORTC, f, b
  bnz enabled_PORTC
  disabled_PORTC:
    clrf PORTC, a
  continueC:
  return
  
enabled_PORTB:
  rlcf PORTB, f, a ; Rotate PORTB to left with carry
  bsf PORTB, 0, a ; Set least sig bit to 1
  bc clear_pbPORTB
  rcall continueB
  return

clear_pbPORTB:
  clrf PORTB, a
  return

  
enabled_PORTC:
  rrcf PORTC, f, a ; Rotate PORTC to right with carry
  bsf PORTC, 7, a  ; Set most sig bit to 1
  bc clear_pbPORTC
  rcall continueC
  return

clear_pbPORTC:
  clrf PORTC, a
  return
  
  
end resetVec