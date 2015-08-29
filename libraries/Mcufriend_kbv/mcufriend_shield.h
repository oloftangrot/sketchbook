#if 0
#elif defined(__AVR_ATxmega128A1__)
#define RD_PORT VPORT3
#define RD_PIN  0
#define WR_PORT VPORT3
#define WR_PIN  1
#define CD_PORT VPORT3
#define CD_PIN  2
#define CS_PORT VPORT3
#define CS_PIN  3
#define RESET_PORT VPORT3
#define RESET_PIN  4

// VPORTs are very fast.   CBI, SBI are only one cycle.    Hence all those RD_ACTIVEs
// ILI9320 data sheet says tDDR=100ns.    We need 218ns to read REGs correctly.
#define write_8(x)    { VPORT2.OUT = x; }
#define read_8()      ( VPORT2.IN )
#define setWriteDir() { PORTCFG.VPCTRLB=PORTCFG_VP3MAP_PORTF_gc | PORTCFG_VP2MAP_PORTC_gc; VPORT2.DIR = 0xFF; }
#define setReadDir()  { VPORT2.DIR = 0x00; }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_16(dst)  { RD_STROBE, RD_ACTIVE, RD_ACTIVE, RD_ACTIVE; dst = read_8(); RD_STROBE, RD_ACTIVE, RD_ACTIVE, RD_ACTIVE; dst = (dst<<8) | read_8(); RD_IDLE; }

#define PIN_LOW(p, b)        (p).OUT &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p).OUT |= (1<<(b))
#define PIN_OUTPUT(p, b)     (p).DIR |= (1<<(b))
#elif defined(__AVR_ATxmega32A4U__)
#define RD_PORT VPORT3
#define RD_PIN  0
#define WR_PORT VPORT3
#define WR_PIN  1
#define CD_PORT VPORT3
#define CD_PIN  2
#define CS_PORT VPORT3
#define CS_PIN  3
#define RESET_PORT PORTE
#define RESET_PIN  0

// VPORTs are very fast.   CBI, SBI are only one cycle.    Hence all those RD_ACTIVEs
// ILI9320 data sheet says tDDR=100ns.    We need 218ns to read REGs correctly.
// S6D0154 data sheet says tDDR=250ns.    We need ~500ns to read REGs correctly.
#define write_8(x)    { VPORT2.OUT = x; }
#define read_8()      ( VPORT2.IN )
#define setWriteDir() { PORTCFG.VPCTRLB=PORTCFG_VP13MAP_PORTB_gc | PORTCFG_VP02MAP_PORTC_gc; VPORT2.DIR = 0xFF; }
#define setReadDir()  { VPORT2.DIR = 0x00; }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_16(dst)  { RD_STROBE, RD_ACTIVE, RD_ACTIVE, RD_ACTIVE; dst = read_8(); RD_STROBE, RD_ACTIVE, RD_ACTIVE, RD_ACTIVE; dst = (dst<<8) | read_8(); RD_IDLE; }

#define PIN_LOW(p, b)        (p).OUT &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p).OUT |= (1<<(b))
#define PIN_OUTPUT(p, b)     (p).DIR |= (1<<(b))
#elif defined(__AVR_ATmega328P__)
#define RD_PORT PORTC
#define RD_PIN  0
#define WR_PORT PORTC
#define WR_PIN  1
#define CD_PORT PORTC
#define CD_PIN  2
#define CS_PORT PORTC
#define CS_PIN  3
#define RESET_PORT PORTC
#define RESET_PIN  4

#define DMASK         0x03
#define NMASK         ~DMASK
#define write_8(x)    { PORTB = (PORTB & NMASK) | ((x) & DMASK); PORTD = (PORTD & DMASK) | ((x) & NMASK); }
#define read_8()      ( (PINB & DMASK) | (PIND & NMASK) )
#define setWriteDir() { DDRB = (DDRB & NMASK) | DMASK; DDRD = (DDRD & DMASK) | NMASK;  }
#define setReadDir()  { DDRB = (DDRB & NMASK) & NMASK; DDRD = (DDRD & DMASK) & DMASK;  }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; RD_STROBE; dst = (dst<<8) | read_8(); RD_IDLE; }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))
#elif defined(__AVR_ATmega2560__)
#define RD_PORT PORTF
#define RD_PIN  0
#define WR_PORT PORTF
#define WR_PIN  1
#define CD_PORT PORTF
#define CD_PIN  2
#define CS_PORT PORTF
#define CS_PIN  3
#define RESET_PORT PORTF
#define RESET_PIN  4

#define EMASK         0x38
#define GMASK         0x20
#define HMASK         0x78
#define write_8(x)   {  PORTH &= ~HMASK; PORTG &= ~GMASK; PORTE &= ~EMASK; \
                        PORTH |= (((x) & (3<<0)) << 5); \
                        PORTE |= (((x) & (3<<2)) << 2); \
                        PORTG |= (((x) & (1<<4)) << 1); \
                        PORTE |= (((x) & (1<<5)) >> 2); \
                        PORTH |= (((x) & (3<<6)) >> 3); \
					 }

#define read_8()      ( ((PINH & (3<<5)) >> 5)\
                      | ((PINE & (3<<4)) >> 2)\
                      | ((PING & (1<<5)) >> 1)\
                      | ((PINE & (1<<3)) << 2)\
                      | ((PINH & (3<<3)) << 3)\
                      )
#define setWriteDir() { DDRH |=  HMASK; DDRG |=  GMASK; DDRE |=  EMASK;  }
#define setReadDir()  { DDRH &= ~HMASK; DDRG &= ~GMASK; DDRE &= ~EMASK;  }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; RD_STROBE; dst = (dst<<8) | read_8(); RD_IDLE; }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#elif defined(__SAMD21G18A__)
// configure macros for the control pins
  #define RD_PORT PORT->Group[0]
  #define RD_PIN  2
  #define WR_PORT PORT->Group[1]
  #define WR_PIN  8
  #define CD_PORT PORT->Group[1]
  #define CD_PIN  9
  #define CS_PORT PORT->Group[0]
  #define CS_PIN  4
  #define RESET_PORT PORT->Group[0]
  #define RESET_PIN  5
// configure macros for data bus
  #define DMASK 0x0030C3C0
  #define write_8(x) PORT->Group[0].OUT.reg = (PORT->Group[0].OUT.reg & ~DMASK)|(((x) & 0x0F) << 6)|(((x) & 0x30) << 10)|(((x) & 0xC0)<<14)
  #define read_8()   (((PORT->Group[0].IN.reg >> 6) & 0x0F)|((PORT->Group[0].IN.reg >> 10) & 0x30)|((PORT->Group[0].IN.reg >> 14) & 0xC0))
  #define setWriteDir() { PORT->Group[0].DIRSET.reg = DMASK; \
	                  PORT->Group[0].WRCONFIG.reg = (DMASK & 0xFFFF) | (0<<22) | (1<<28) | (1<<30); \
	                  PORT->Group[0].WRCONFIG.reg = (DMASK>>16) | (0<<22) | (1<<28) | (1<<30) | (1<<31); \
                        }
  #define setReadDir()  { PORT->Group[0].DIRCLR.reg = DMASK; \
	                  PORT->Group[0].WRCONFIG.reg = (DMASK & 0xFFFF) | (1<<17) | (1<<28) | (1<<30); \
	                  PORT->Group[0].WRCONFIG.reg = (DMASK>>16) | (1<<17) | (1<<28) | (1<<30) | (1<<31); \
                        }       
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { RD_STROBE; dst = read_8(); RD_IDLE; RD_STROBE; dst = (dst<<8) | read_8(); RD_IDLE; }
// Shield Control macros.
#define PIN_LOW(port, pin)    (port).OUTCLR.reg = (1<<(pin))
#define PIN_HIGH(port, pin)   (port).OUTSET.reg = (1<<(pin))
#define PIN_OUTPUT(port, pin) (port).DIR.reg |= (1<<(pin))

#else
#error MCU unsupported
#endif

#define RD_ACTIVE  PIN_LOW(RD_PORT, RD_PIN)
#define RD_IDLE    PIN_HIGH(RD_PORT, RD_PIN)
#define RD_OUTPUT  PIN_OUTPUT(RD_PORT, RD_PIN)
#define WR_ACTIVE  PIN_LOW(WR_PORT, WR_PIN)
#define WR_IDLE    PIN_HIGH(WR_PORT, WR_PIN)
#define WR_OUTPUT  PIN_OUTPUT(WR_PORT, WR_PIN)
#define CD_COMMAND PIN_LOW(CD_PORT, CD_PIN)
#define CD_DATA    PIN_HIGH(CD_PORT, CD_PIN)
#define CD_OUTPUT  PIN_OUTPUT(CD_PORT, CD_PIN)
#define CS_ACTIVE  PIN_LOW(CS_PORT, CS_PIN)
#define CS_IDLE    PIN_HIGH(CS_PORT, CS_PIN)
#define CS_OUTPUT  PIN_OUTPUT(CS_PORT, CS_PIN)
#define RESET_ACTIVE  PIN_LOW(RESET_PORT, RESET_PIN)
#define RESET_IDLE    PIN_HIGH(RESET_PORT, RESET_PIN)
#define RESET_OUTPUT  PIN_OUTPUT(RESET_PORT, RESET_PIN)

// General macros.   IOCLR registers are 1 cycle when optimised.
#define WR_STROBE { WR_ACTIVE; WR_IDLE; }         //PWLW=TWRL=50ns
#define RD_STROBE RD_IDLE, RD_ACTIVE, RD_ACTIVE, RD_ACTIVE  //PWLR=TRDL=150ns, tDDR=100ns

#define CTL_INIT()   { RD_OUTPUT; WR_OUTPUT; CD_OUTPUT; CS_OUTPUT; RESET_OUTPUT; }
#define WriteCmd(x)  { CD_COMMAND; write16(x); }
#define WriteData(x) { CD_DATA; write16(x); }
