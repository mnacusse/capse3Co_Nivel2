/*============================================================================
 * Autor: Eric Pernia.
 * Fecha: 2017/05/18
 * Licencia: BSD de 3 clausulas.
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

#include "programa.h"   // <= su propio archivo de cabecera
#include "sapi.h"       // <= Biblioteca sAPI

#include <math.h>       // <= Funciones matematicas
//#include "ff.h"        // <= Biblioteca FAT FS

/*==================[definiciones y macros]==================================*/

#define BAUD_RATE 115200

#define FILENAME "magnetometro_hmc5883l.txt"
/*==================[definiciones de datos internos]=========================*/

static int16_t hmc5883l_x_raw;
static int16_t hmc5883l_y_raw;
static int16_t hmc5883l_z_raw;

static int16_t angle = 0;

/* Buffers */
static uint8_t uartBuff[10];

//static FATFS fs;           // <-- FatFs work area needed for each volume
//static FIL fp;             // <-- File object needed for each open file

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/
void configPeripheral( void );

void WriteHmc58831ToUart( void );

//void WriteInFile ( void );

//void sendAngleToFile( void );

//void sendHmc5883lToFile( int16_t axis, uint8_t axisName );

void sendAngleToUart( void );

void sendHmc5883lToUart( int16_t axis, uint8_t axisName );

int16_t vectorR2ToAngle( int16_t axis1, int16_t axis2 );

uint8_t* angleToDirection( int16_t angle );

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base);

/* Enviar fecha y hora en formato "DD/MM/YYYY, HH:MM:SS" */
void showDateAndTime( rtc_t * rtc );

// FUNCION que se ejecuta cada vezque ocurre un Tick
//bool_t diskTickHook( void *ptr );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){
     // Inicializar y configurar la plataforma
    boardConfig();   
    configPeripheral();
    
    delay_t miDelay;
    delayConfig( &miDelay, 1000 );
    
    
     // Estructura RTC
    rtc_t rtc;

    rtc.year = 2017;
    rtc.month = 6;
    rtc.mday = 7;
    rtc.wday = 4;
    rtc.hour = 19;
    rtc.min = 58;
    rtc.sec= 0;

    bool_t val = 0;
    uint8_t i = 0;

    // Inicializar RTC
    val = rtcConfig( &rtc );
    
    delay(2000);
   
    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {      
      
	   if( delayRead( &miDelay ) ){

         hmc5883lRead( &hmc5883l_x_raw, &hmc5883l_y_raw, &hmc5883l_z_raw );
         // Se debe esperar minimo 67ms entre lecturas su la tasa es de 15Hz
         // para leer un nuevo valor del magnetómetro
         //delay(67);

         angle = vectorR2ToAngle( hmc5883l_x_raw, hmc5883l_y_raw );

         // Usar Servo
         servoWrite( SERVO0, (uint8_t)angle );
         
         //Escribir en Uart datos de magnetómetro
        WriteHmc58831ToUart ();  
        // Leer fecha y hora
        val = rtcRead( &rtc );
        // Mostrar fecha y hora en formato "DD/MM/YYYY, HH:MM:SS"
        showDateAndTime( &rtc ); 
        //Escribe los datos obtenidos en el archivo 
        //WriteInFile(); 
         
        }
    } 

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta 
   // directamenteno sobre un microcontroladore y no es llamado/ por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/
/*
void WriteInFile ( void ) {
     
   // Inicializar el conteo de Ticks con resolución de 10ms, 
   // con tickHook diskTickHook
   tickConfig( 10, diskTickHook );
             
   // ------ PROGRAMA QUE ESCRIBE EN LA SD -------

   UINT nbytes;
   
   // Give a work area to the default drive
   if( f_mount( &fs, "", 0 ) != FR_OK ){
      // If this fails, it means that the function could
      // not register a file system object.
      // Check whether the SD card is correctly connected
   }

   // Create/open a file, then write a string and close it
   if( f_open( &fp, FILENAME, FA_WRITE | FA_OPEN_APPEND ) == FR_OK ){
      f_write( &fp, "Hola mundo\r\n", 12, &nbytes );

      f_close(&fp);

      if( nbytes == 12 ){
         // Turn ON LEDG if the write operation was successful
         gpioWrite( LEDG, ON );
      }
   } else{
      // Turn ON LEDR if the write operation was fail
      gpioWrite( LEDR, ON );
       
    }
}


void sendAngleToFile( void ){

   // Envio la primer parte dle mensaje a la Uart
   uartWriteString( UART_USB, "Angulo: ");

   // Conversion de muestra entera a ascii con base decimal
   itoa( (int) angle, uartBuff, 10 ); /* 10 significa decimal

   //Envio el valor del eje
   uartBuff[4] = 0;    // NULL
   uartWriteString( UART_USB, uartBuff );

    Envio un 'espacio'
   uartWriteString( UART_USB, "     ");
}

void sendHmc5883lToFile( int16_t axis, uint8_t axisName ){

   // Envio la primer parte dle mensaje a la Uart
   uartWriteString( UART_USB, "HMC5883L eje ");
   uartWriteByte( UART_USB, axisName );
   uartWriteString( UART_USB, ": ");

  // Conversion de muestra entera a ascii con base decimal
   itoa( (int) axis, uartBuff, 10 ); // 10 significa decimal

   //Envio el valor del eje
   uartBuff[4] = 0;    /* NULL
   uartWriteString( UART_USB, uartBuff );

   // Envio un 'espacio'
   uartWriteString( UART_USB, "     ");
}*/


void configPeripheral( void ){
     // ---------- CONFIGURACIONES ------------------------------
    
   // Inicializar HMC5883L
   HMC5883L_config_t hmc5883L_configValue;

   hmc5883lPrepareDefaultConfig( &hmc5883L_configValue );

   hmc5883L_configValue.mode = HMC5883L_continuous_measurement;
   hmc5883L_configValue.samples = HMC5883L_8_sample;

   hmc5883lConfig( hmc5883L_configValue );

   /* Inicializar Uart */
   uartConfig(UART_USB, BAUD_RATE);

   /* Configurar Servo */
   servoConfig( 0,      SERVO_ENABLE );
   servoConfig( SERVO0, SERVO_ENABLE_OUTPUT );
   
   // SPI configuration
  // spiConfig( SPI0 );
}

void WriteHmc58831ToUart( void ){
    sendHmc5883lToUart( hmc5883l_x_raw, 'x' );
    sendHmc5883lToUart( hmc5883l_y_raw, 'y' );
    sendHmc5883lToUart( hmc5883l_z_raw, 'z' );
    
    sendAngleToUart();
    uartWriteString( UART_USB, "Apunta al ");
    uartWriteString( UART_USB,  angleToDirection( angle )  );
    
          
    hmc5883l_x_raw=0;
    hmc5883l_y_raw=0;
    hmc5883l_z_raw=0;
}
void sendAngleToUart( void ){
   
   /* Envio la primer parte dle mensaje a la Uart */
   uartWriteString( UART_USB, "Angulo: ");

   /* Conversion de muestra entera a ascii con base decimal */
   itoa( (int) angle, uartBuff, 10 ); /* 10 significa decimal */
   
   /* Envio el valor del eje */
   uartBuff[4] = 0;    /* NULL */
   uartWriteString( UART_USB, uartBuff );
   
   /* Envio un 'espacio' */
   uartWriteString( UART_USB, "     ");
}

void sendHmc5883lToUart( int16_t axis, uint8_t axisName ){
   
   /* Envio la primer parte dle mensaje a la Uart */
   uartWriteString( UART_USB, "HMC5883L eje ");
   uartWriteByte( UART_USB, axisName );
   uartWriteString( UART_USB, ": ");

   /* Conversion de muestra entera a ascii con base decimal */
   itoa( (int) axis, uartBuff, 10 ); /* 10 significa decimal */
   
   /* Envio el valor del eje */
   uartBuff[4] = 0;    /* NULL */
   uartWriteString( UART_USB, uartBuff );
   
   /* Envio un 'espacio' */
   uartWriteString( UART_USB, "     ");
}


int16_t vectorR2ToAngle( int16_t axis1, int16_t axis2 ){

	float angle = 0;
	float heading = 0;

	// Angulo en radianes
	heading = atan2( (float)axis2, (float)axis1 );

	// Angulo en radianes ajustado
	if( heading < 0.0 ){
		heading += 2.0 * 3.1415;
	}

	// Angulo en grados
	angle = (heading * 180.0 / 3.1415); //M_PI

	return (int16_t)angle;
}


uint8_t* angleToDirection( int16_t angle ){

   if (angle >= 360) {
      angle %= 360;
   }

   if (angle >= 338 || angle < 23) {
      return (char*)"Norte     ";
   } else if (angle < 68) {
      return (char*)"Noreste     ";
   } else if (angle < 113) {
      return (char*)"Este     ";
   } else if (angle < 158) {
      return (char*)"Sudeste     ";
   } else if (angle < 203) {
      return (char*)"Sur     ";
   } else if (angle < 248) {
      return (char*)"Suroeste     ";
   } else if (angle < 293) {
      return (char*)"Oeste     ";
   } else {
      return (char*)"Noroeste     ";
   }
   }

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base) {
   // check that the base if valid
   if (base < 2 || base > 36) { *result = '\0'; return result; }

   char* ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}


/* Enviar fecha y hora en formato "DD/MM/YYYY, HH:MM:SS" */
void showDateAndTime( rtc_t * rtc ){
   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->mday), (char*)uartBuff, 10 ); /* 10 significa decimal */
   /* Envio el dia */
   if( (rtc->mday)<10 )
      uartWriteByte( UART_USB, '0' );
   uartWriteString( UART_USB, uartBuff );
   uartWriteByte( UART_USB, '/' );

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->month), (char*)uartBuff, 10 ); /* 10 significa decimal */
   /* Envio el mes */
   if( (rtc->month)<10 )
      uartWriteByte( UART_USB, '0' );
   uartWriteString( UART_USB, uartBuff );
   uartWriteByte( UART_USB, '/' );

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->year), (char*)uartBuff, 10 ); /* 10 significa decimal */
   /* Envio el año */
   if( (rtc->year)<10 )
      uartWriteByte( UART_USB, '0' );
   uartWriteString( UART_USB, uartBuff );

   uartWriteString( UART_USB, ", ");

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->hour), (char*)uartBuff, 10 ); /* 10 significa decimal */
   /* Envio la hora */
   if( (rtc->hour)<10 )
      uartWriteByte( UART_USB, '0' );
   uartWriteString( UART_USB, uartBuff );
   uartWriteByte( UART_USB, ':' );

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->min), (char*)uartBuff, 10 ); /* 10 significa decimal */
   /* Envio los minutos */
   // uartBuff[2] = 0;    /* NULL */
   if( (rtc->min)<10 )
      uartWriteByte( UART_USB, '0' );
   uartWriteString( UART_USB, uartBuff );
   uartWriteByte( UART_USB, ':' );

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->sec), (char*)uartBuff, 10 ); /* 10 significa decimal */
   /* Envio los segundos */
   if( (rtc->sec)<10 )
      uartWriteByte( UART_USB, '0' );
   uartWriteString( UART_USB, uartBuff );

   /* Envio un 'enter' */
   uartWriteString( UART_USB, "\r\n");
}
/*bool_t diskTickHook( void *ptr ){
   disk_timerproc();   // Disk timer process
   return 1;
}*/

/*==================[fin del archivo]========================================*/
