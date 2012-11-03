/*! \file avrlibtypes.h \brief AVRlib global types and typedefines. */
//*****************************************************************************
//
// File Name	: 'avrlibtypes.h'
// Title		: AVRlib global types and typedefines include file
// Author		: Pascal Stang
// Created		: 7/12/2001
// Revised		: 9/30/2002
// Version		: 1.0
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
//	Description : Type-defines required and used by AVRlib.  Most types are also
//						generally useful.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
#ifndef TYPES_H
#define TYPES_H

#define SUCCESS  0
#define ERROR    0xFF
#define TIMEOUT  0xFE

typedef unsigned int uint;

/**
 * The 8-bit signed data type.
 */
typedef char int8;
/**
 * The volatile 8-bit signed data type.
 */
typedef volatile char vint8;
/**
 * The 8-bit unsigned data type.
 */
typedef unsigned char uint8;
/**
 * The volatile 8-bit unsigned data type.
 */
typedef volatile unsigned char vuint8;

/**
 * The 16-bit signed data type.
 */
typedef int int16;
/**
 * The volatile 16-bit signed data type.
 */
typedef volatile int vint16;
/**
 * The 16-bit unsigned data type.
 */
typedef unsigned int uint16;
/**
 * The volatile 16-bit unsigned data type.
 */
typedef volatile unsigned int vuint16;
/**
 * The 32-bit signed data type.
 */
typedef long int32;
/**
 * The volatile 32-bit signed data type.
 */
typedef volatile long vint32;
/**
 * The 32-bit unsigned data type.
 */
typedef unsigned long uint32;
/**
 * The volatile 32-bit unsigned data type.
 */
typedef volatile unsigned long vuint32;

/* bsd */
typedef uint8     u_char;   /**< 8-bit value */
typedef uint8       SOCKET;
typedef uint16      u_short;  /**< 16-bit value */
typedef uint16      u_int;    /**< 16-bit value */
typedef uint32      u_long;   /**< 32-bit value */


// data type definitions macros
#ifndef __cplusplus
typedef unsigned char  bool;
#endif
typedef unsigned char  u08;
typedef   signed char  s08;
typedef          char  c08;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned long  u32;
typedef   signed long  s32;
typedef unsigned long long u64;
typedef   signed long long s64;
typedef float 			   f32;
typedef double 			   f64;

#define pc08 EEMEM c08

typedef void (*voidFuncPtr)(void);

typedef void (*u08FuncPtr)(u08);

//typedef u08 (* const u08FuncPtr_Retu08)(*u08);


// maximum value that can be held
// by unsigned data types (8,16,32bits)
#define MAX_U08	255
#define MAX_U16	65535
#define MAX_U32	4294967295

// maximum values that can be held
// by signed data types (8,16,32bits)
#define MIN_S08	-128
#define MAX_S08	127
#define MIN_S16	-32768
#define MAX_S16	32767
#define MIN_S32	-2147483648
#define MAX_S32	2147483647

#define setGroup(var,groupmask,value) {var &= ~(groupmask);var |= (value);}
#define setBit(var,bitposition)       {var |=(1<<bitposition);}
#define clrBit(var,bitposition)       {var &= ~(1<<bitposition);}

#define outb(addr, data)  addr = (data)
#define inb(addr)     (addr)
#define inb(addr)     (addr)
#define outw(addr, data)  addr = (data)
#define inw(addr)     (addr)
#define BV(bit)     (1<<(bit))
#define cbi(reg,bit)  reg &= ~(BV(bit))
#define sbi(reg,bit)  reg |= (BV(bit))
#define cli()     __asm__ __volatile__ ("cli" ::)
#define sei()     __asm__ __volatile__ ("sei" ::)


// use this for packed structures
// (this is seldom necessary on an 8-bit architecture like AVR,
//  but can assist in code portability to AVR)
#define GNUC_PACKED __attribute__((packed))

// port address helpers
#define DDR(x) ((x)-1)    // address of data direction register of port x
#define PIN(x) ((x)-2)    // address of input register of port x
// MIN/MAX/ABS macros
#define MIN(a,b)      ((a<b)?(a):(b))
#define MAX(a,b)      ((a>b)?(a):(b))
#define ABS(x)        ((x>0)?(x):(-x))

// constants
#define PI    3.14159265359

#define CYCLES_PER_US ((F_CPU+500000)/1000000)  // cpu cycles per microsecond
#define US 1000
#define MS 1000000

/// Macro to set a port bit (1)
#define BIT_SET_HI(Port,Bit)             {Port |=(1<<Bit);}

/// Macro to clear a port bit (0)
#define BIT_SET_LO(Port,Bit)             {Port &= ~(1<<Bit);}

/// Macro to toggle a port bit
#define BIT_TOGGLE(Port,Bit)             {if(Port&(1<<Bit)) {Port &= ~(1<<Bit);} else {Port |=(1<<Bit);}}

/// Macro to test if a port bit is set (1?)
#define BIT_IS_HI(Port,Bit)              ((Port&(1<<Bit)) != 0)

/// Macro to test if a port bit is cleared (0?)
#define BIT_IS_LO(Port,Bit)              ((Port&(1<<Bit)) == 0)

/// Macro to wait until a port bit is set
#define LOOP_UNTIL_BIT_IS_HI(Port,Bit)   while(BIT_IS_LO(Port,Bit)) {;}

/// Macro to wait until a port bit is cleared
#define LOOP_UNTIL_BIT_IS_LO(Port,Bit)   while(BIT_IS_HI(Port,Bit)) {;}
//@}

///  \name Byte macros
//@{
/// Macro to extract the high 8 bits of a 16-bit value (Most Significant Byte)
#define U16_HI(u16Data) ((u08)((u16Data>>8)&0xff))

/// Macro to extract the low 8 bits of a 16-bit value (Least Significant Byte)
#define U16_LO(u16Data) ((u08)(u16Data&0xff))
//@}

/// \name Utility macros
//@{
/// Macro to calculate division with rounding to nearest integer value
#define DIV(Dividend,Divisor) (((Dividend+((Divisor)>>1))/(Divisor)))

/// Macro to check if a value is within bounds (Min <= Value <= Max ?)
#define BOUND(Value,Min,Max)    (((Value)>=(Min))&&((Value)<=(Max)))

/// Macro to calculate the length of an array
#define ARRAY_LENGTH(Array)      (sizeof(Array)/sizeof((Array)[0]))
//@}
#define PRINTF(format, ...) printf_P(PSTR(format), ## __VA_ARGS__)

#define DEF8x   defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)
#define DEF8    defined(__AVR_ATmega8__)
#define NDEF8x  !(DEF8x)
#define DEF164  defined(__AVR_ATmega164P__ || defined(__AVR_ATmega644__))
#define NDEF164 !(DEF164)

#endif
