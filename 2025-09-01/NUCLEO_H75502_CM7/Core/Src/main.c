/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>		// Pour les limites comme INT_MAX, UINT_MAX
#include <float.h>    	// Pour les limites de float/double
#include <math.h>


#define ARM_MATH_CM7
#include "arm_math.h"


//librairie personel
#include "utility.h"
/* integrité systeme
 *
 * 1. pvd/bor
 * 2. iwdg
 * 3. surveillance thermique, batteries pour le futur cell li-oh
 *
 *
 *
 *
 *
 * qualité mesure
 * 1. ecc
 * 2. css
 * 3. wwdg
 *
 *
 *
 * mpu
 * rcc
 * protection flash/ram
 *
 *
 * semaphore entre les coeurs
 *
 *
 *
 */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* DUAL_CORE_BOOT_SYNC_SEQUENCE: Define for dual core boot synchronization    */
/*                             demonstration code based on hardware semaphore */
/* This define is present in both CM7/CM4 projects                            */
/* To comment when developping/debugging on a single core                     */
#define DUAL_CORE_BOOT_SYNC_SEQUENCE

#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc3;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */
//type de variable:

char caractere_demo = 0;

const char ACK = 42; //	*
const char NAK = 35; //	#

int val_01 = 55;				// −2 147 483 648 à 2 147 483 647
unsigned int val_02 = 55;		// 0 à 4 294 967 295

// 8 bits
volatile int8_t   i8  = 0;       // -128
volatile uint8_t  u8  = 0;      // 255

// 16 bits
volatile int16_t  i16 = 32768;      // -32 768
volatile uint16_t u16 = 65535;     // 65 535

// 32 bits
volatile int32_t  i32 = INT32_MIN;      // -2 147 483 648
volatile uint32_t u32 = UINT32_MAX;     // 4 294 967 295

// 64 bits
volatile int64_t  i64 = 1000000000000000000;      // -9 223 372 036 854 775 808
volatile uint64_t u64 = 7446744073709551614;     // 18 446 744 073 709 551 615

// ─────────────────────────────────────────────
// Types à virgule flottante
// ─────────────────────────────────────────────
volatile float  f32 = FLT_MAX;          // ~3.4E+38, précision ~7 chiffres
volatile double f64 = DBL_MAX;          // ~1.7E+308, précision ~15 chiffres
volatile long double test22 = 12.3333333;





typedef struct {
	int a;
	int b;
	int c;
} s_struct;

#define OFF		0U
#define ON      1U


__IO uint32_t Main_Sys_Status = OFF;




int counter = 0;

#define RxBuf_SIZE	 10
#define MainBuf_SIZE 20

/* RxBuf   — le buffer temporaire rempli par le DMA à chaque réception.
 * MainBuf — le buffer principal circulaire où tu conserves tout l’historique ou
 * 		   les blocs successifs de données reçues.
*/

uint8_t RxBuf[RxBuf_SIZE];
uint8_t MainBuf[MainBuf_SIZE];

uint16_t oldPos = 0;
uint16_t newPos = 0;

volatile int isOK = 0;





//mise en place des memoires disponible

#define RAMD2_DATA __attribute__((section(".ram_d2_data"), used))
#define RAMD2_BSS  __attribute__((section(".ram_d2_bss"), used))

RAMD2_DATA int myInitializedVar = 42;
RAMD2_BSS  int myUninitializedVar;

RAMD2_DATA uint8_t TxBuf[1024];



/*
 * .itcm_code : code rapide exécuté depuis ITCM (copié depuis flash).
 * .dtcm_data : données initialisées rapides, copiées depuis flash vers DTCM.
 * .dtcm_bss : données non initialisées rapides, mises à zéro en DTCM.
 */


#define ITCM_CODE __attribute__((section(".itcm_code"), used))
#define DTCM_DATA __attribute__((section(".dtcm_data"), used))
#define DTCM_BSS  __attribute__((section(".dtcm_bss"), used))

DTCM_DATA uint8_t bufferDT[512];

DTCM_BSS uint8_t bufferDT_uninit[256];

ITCM_CODE void fast_fn(void)
{
    bufferDT[0] = 123;
}


#define RAMD3_DATA __attribute__((section(".ram_d3_data"), used))
#define RAMD3_BSS  __attribute__((section(".ram_d3_bss"), used))

RAMD3_DATA uint8_t bufferD3[512];
RAMD3_BSS uint8_t bufferD3_uninit;



uint16_t blink_delays[] = {
		500,
		250,
		100
};

uint8_t blink_delay = 0;

//mise en place de delay non bloquant
uint32_t  now = 0, next_blink = 500, next_tick = 1000, loop_cnt = 0;

//utilisant des delay changable pour le blink d'une LED



// mise en place du bouton user
#define BUTTON_RELEASED                    0U
#define BUTTON_PRESSED                     1U
#define BUTTONn                            1U

__IO uint32_t ButtonState = BUTTON_RELEASED;


//temp


//RAMD3_DATA __IO uint16_t ADC_VAL[2];


#define CHNB (2)
#define BUFSIZE (64)
#define HBUFSIZE (BUFSIZE / 2)
#define QBUFSIZE (BUFSIZE / 4)
#define ADCBUFSIZE (CHNB * BUFSIZE)

RAMD3_DATA __IO uint16_t adcBuf[ADCBUFSIZE]; /* ADC buffer dans RAM_D3 pour le BDMA */
uint16_t adcChBuf[BUFSIZE]; /* ADC channel buffer */
uint32_t adcChAvg[CHNB]; /* ADC channel average value */
volatile uint32_t adcFlag = 0; /* ADC DMA data ready flag */
uint16_t vDDA = 0; /* VDDA [mV] evaluated based on VREFINT ADC channel */
int16_t temperatureDegC = 0; /* TS temperature [DegC] */
uint16_t i,j,k;


//test librairie
int32_t resultat = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_BDMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_ADC3_Init(void);
/* USER CODE BEGIN PFP */
//dma avant module
void bubblesort(uint16_t *array, uint16_t length);
int integer_add(int x, int y);
void sum1(s_struct *s);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
  int32_t timeout;
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
/* USER CODE END Boot_Mode_Sequence_0 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
  /* Wait until CPU2 boots and enters in stop mode or timeout*/
  timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
  Error_Handler();
  }
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
HSEM notification */
/*HW semaphore Clock enable*/
__HAL_RCC_HSEM_CLK_ENABLE();
/*Take HSEM */
HAL_HSEM_FastTake(HSEM_ID_0);
/*Release HSEM in order to notify the CPU2(CM4)*/
HAL_HSEM_Release(HSEM_ID_0,0);
/* wait until CPU2 wakes up from stop mode */
timeout = 0xFFFF;
while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
if ( timeout < 0 )
{
Error_Handler();
}
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_BDMA_Init();
  MX_USART3_UART_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */
  printf("INITIAL_START \r\n");
  printf("%c\r\n", ACK);			//%c pour dire que char sera afficher version caractere
  printf("%d\r\n", NAK);			//%d pour dire que char sera afficher version codu numerique





  i8  = -42;
  u8  = 200;
  volatile char     c   = 'A';

  f32 = 3.14159f;
  f64 = 2.718281828459045;

  printf("// ─────────────── entiers signés 8 bits ───────────────\r\n");
  printf("int8_t %%d   -> %d\r\n",   i8);	// -42 signé (promu en int)
  printf("int8_t %%i   -> %i\r\n",   i8);	// -42 idem que %d
  //printf("int8_t %%hhd -> %hhd\r\n", i8); // format strict signed char
  printf("int8_t %%x   -> %x\r\n",   i8);	// ffffffd6  hexadécimal
  printf("int8_t %%X   -> %X\r\n",   i8);	// FFFFFFD6  hexadécimal majuscules
  printf("int8_t %%o   -> %o\r\n",   i8);	// 37777777726 octal
  printf("int8_t %%c   -> %c\r\n",   i8);	// ��erreur �� interprété comme ASCII

  printf("// ─────────────── entiers non signés 8 bits ───────────────\r\n");
  printf("uint8_t %%u   -> %u\r\n",   u8);	// 200 non signé
  //printf("uint8_t %%hhu -> %hhu\r\n", u8);// format strict unsigned char
  printf("uint8_t %%x   -> %x\r\n",   u8);	// c8 hexadécimal
  printf("uint8_t %%X   -> %X\r\n",   u8);	// C8 hexadécimal majuscules
  printf("uint8_t %%o   -> %o\r\n",   u8);	// 31 octal
  printf("uint8_t %%c   -> %c\r\n",   u8);	// ��erreur �� ASCII

  printf("// ─────────────── char ───────────────\r\n");
  printf("char %%c -> %c\r\n", c);			// A caractère
  printf("char %%d -> %d\r\n", c);			// 65 valeur numérique ASCII
  printf("char %%x -> %x\r\n", c);			// 41 valeur ASCII en hex

  printf("// ─────────────── flottants ───────────────\r\n");
  // ⚠️ nécessite l’option : -u _printf_float
  printf("float  %%f -> %f\r\n", f32);		// 3.141590 	notation décimale
  printf("float  %%e -> %e\r\n", f32);		// 3.141590e+00 notation scientifique
  printf("float  %%g -> %g\r\n", f32);		// 3.1415		 auto (f ou e)

  printf("double %%f -> %f\r\n", f64);		// 2.718282
  printf("double %%e -> %e\r\n", f64);		// 2.718282e+00
  printf("double %%g -> %g\r\n", f64);		// 2.71828


  printf("%10.3e\r\n", f64);
  printf("%10.3g\r\n", f64);
  printf("%10.3f\r\n", f64);
  printf("x apres .: %.15f\r\n", f64);		//2.718281828459045 float avec x chiffre afficher

  printf("sizeof(char)     = %d\r\n", sizeof(char)); //1
  printf("sizeof(short int)    = %d\r\n", sizeof(short int)); //2
  printf("sizeof(int)      = %d\r\n", sizeof(int)); //4
  printf("sizeof(long int)     = %d\r\n", sizeof(long int)); //4
  printf("sizeof(long long int)= %d\r\n", sizeof(long long int)); //8
  printf("sizeof(float)    = %d\r\n", sizeof(float)); //4
  printf("sizeof(double)   = %d\r\n", sizeof(double)); //8
  printf("sizeof(long double) = %d\r\n", sizeof(long double)); //1
  printf("sizeof(uint8_t)  = %d\r\n", sizeof(uint8_t)); //1
  printf("sizeof(uint16_t) = %d\r\n", sizeof(uint16_t)); //2
  printf("sizeof(uint32_t) = %d\r\n", sizeof(uint32_t)); //4
  printf("sizeof(uint64_t) = %d\r\n", sizeof(uint64_t)); //8
  printf("sizeof(void *)   = %d\r\n", sizeof(void *)); //4

  s_struct s = {10, 10, 0};
  printf("before sum1: a = %d b = %d c = %d\r\n", s.a, s.b, s.c);	//before sum1: a = 10 b = 10 c = 0
  sum1(&s);
  printf("after sum1: a = %d b = %d c = %d\r\n", s.a, s.b, s.c);	//after sum1: a = 10 b = 10 c = 20


  //cycle trough an arry from a pointers

  uint16_t a[] = {
		  1,
		  2,
		  3,
		  4,
		  5
  };


  for (int n= 0; n < sizeof(a) / sizeof(a[0]); ++n) {
	  printf("a[%d] = %d\r\n", n, a[n]);
  }

  //uint16_t *ap = &a[0];
  uint16_t *ap = a;
  for (int n = 0; n < sizeof(a) / sizeof(a[0]); ++n) {
	  printf("a[%d] = %d\r\n", n, ap[n]);
  }

  for (int n = 0; n < sizeof(a) / sizeof(a[0]); ++n) {
	  printf("a[%d] = %d\r\n", n, *ap);
	  ++ap; //ap += 1;
  }




  /*
   * Legal
   * pa = a
   * pa++
   *
   * illegal
   * a = pa
   * a++
   *
   */



  char sbuf[128] = {0};
  printf("Length of sbuf = %d\r\n", sizeof(sbuf));	//Length of sbuf = 128


  char cstr1[] = "This is";
  char cstr2[] = "a string";

  printf("cstr1 len = %d: %s\r\n", strlen(cstr1), cstr1);	//cstr1 len = 7: This is
  printf("cstr2 len = %d: %s\r\n", strlen(cstr2), cstr2);	//cstr2 len = 8: a string

  //strcpy(sbuf, cstr1);
  //strcat(sbuf, " ");
  //strcat(sbuf, cstr2);

  strncpy(sbuf, cstr1, sizeof(sbuf) - strlen(sbuf) - 1);
  strncat(sbuf, " ", sizeof(sbuf) - strlen(sbuf) - 1);
  strncat(sbuf, cstr2, sizeof(sbuf) - strlen(sbuf) - 1);

  printf("sbuf = %s\r\n", sbuf);	//sbuf = This is a string

  char *sbuf2;
  //éviter malloc, privilégier buffers statique
  sbuf2 = malloc(128);

  if (sbuf2) {
	  strcpy(sbuf2, sbuf);

	  printf("sbuf2 len = %d: %s\r\n", strlen(sbuf2), sbuf2);	//sbuf2 len = 16: This is a string

	  free(sbuf2);

  }

  char cstr3[] = "a string";


  //fonction cstr2 = cstr3 donc si faux sera print
  if(strcmp(cstr1, cstr3)) printf("cstr1 does not match cstr3\r\n"); //cstr1 does not match cstr3

  if(strcmp(cstr2, cstr3)) printf("cstr2 does not match cstr3\r\n");

  char *loc = strstr(sbuf, "a s");
  printf("location = %d: %s\r\n", loc - sbuf, loc);	//location = 8: a string



  uint8_t opp_t1 = 0;

  opp_t1 = opp_t1 + 2;
  printf(" 2 %u\r\n", opp_t1);
  //meme chose que

  opp_t1 = 0;
  opp_t1 += 2;
  printf("+= 2 %u\r\n", opp_t1);


  opp_t1 -= 1;
  printf("-= 1 %u\r\n", opp_t1);

  opp_t1 *= 2;
  printf("*= 2 %u\r\n", opp_t1);

  opp_t1 /= 1;
  printf("/= 1 %u  -D 2\r\n", opp_t1);

  opp_t1 = 4%3;
  printf("4%%3 %u\r\n", opp_t1);


  //i += j += k
  //est comme
  //i += (j+=k)




  opp_t1 = 55;
  printf(" %5u\r\n", opp_t1);
  printf(" %-5u\r\n", opp_t1);

  uint16_t opp_t3 = 14555;
  printf(" %8u\r\n", opp_t3);
  printf(" %-8u\r\n", opp_t3);

  printf("x %u\r\n", opp_t1);
  printf("++x %u\r\n", ++opp_t1);
  printf("x %u\r\n", opp_t1);

  opp_t1 = 55;
  printf("x %u\r\n", opp_t1);
  printf("x++ %u\r\n", opp_t1++);
  printf("x %u\r\n", opp_t1);

  opp_t1 = 55;
  printf("x %u\r\n", opp_t1);
  printf("--x %u\r\n", --opp_t1);
  printf("x %u\r\n", opp_t1);

  opp_t1 = 55;
  printf("x %u\r\n", opp_t1);
  printf("x-- %u\r\n", opp_t1--);
  printf("x %u\r\n", opp_t1);

  //i=1
  //j= 2
  // k = ++i + j++

  // ====

  // i = i + 1
  //k = i + j
  //j = j + 1


  //*************************************************************************************************************
  //*
  //*							EXAMPLE POUR arm_math.h et math.h
  //*
  //*
  //*************************************************************************************************************
  double math_t1 = 0;
  double math_t2 = 2;
  double math_t3 = 3;


  math_t1 = pow(math_t2, math_t3);
  printf("math_t1 -> %f\r\n", math_t1); //8



  // pow difference entre float et double
  float  f_a = 2.0f, f_b = 3.0f, f_res;
  double d_a = 2.0 , d_b = 3.0 , d_res;

  // pow
  f_res = powf(f_a, f_b);
  d_res = pow(d_a, d_b);
  printf("powf(float)  = %.7f\r\n", f_res);
  printf("pow(double) = %.15f\r\n", d_res);

  // sqrt
  f_res = sqrtf(f_a);
  d_res = sqrt(d_a);
  printf("sqrtf(float)  = %.7f\r\n", f_res);
  printf("sqrt(double) = %.15f\r\n", d_res);

  // sin
  f_a = 2.0f;
  d_a = 2.0;
  f_res = sinf(f_a);
  d_res = sin(d_a);
  printf("sinf(float)  = %.7f\r\n", f_res);
  printf("sin(double) = %.15f\r\n", d_res);

  // cos
  f_a = 2.0f;
  d_a = 2.0;
  f_res = cosf(f_a);
  d_res = cos(d_a);
  printf("cosf(float)  = %.7f\r\n", f_res);
  printf("cos(double) = %.15f\r\n", d_res);















  HAL_UARTEx_ReceiveToIdle_DMA(&huart3, RxBuf, RxBuf_SIZE);


  HAL_ADCEx_Calibration_Start(&hadc3,ADC_CALIB_OFFSET_LINEARITY,ADC_SINGLE_ENDED);


  HAL_ADC_Start_DMA(&hadc3, (uint32_t *)adcBuf, ADCBUFSIZE);



  resultat = additionner_deux_nombres(10, 5);

  memset (TxBuf, 0x1, 1024);
  // Printf



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {



	  now = HAL_GetTick();
	  if (now >= next_blink)
	  {
		  //printf("Toggling GREEN LED \r\n");
		  HAL_GPIO_TogglePin(LDG_GPIO_Port, LDG_Pin);

		  next_blink = now + blink_delays[blink_delay];
	  }

	  if (now >= next_tick)
	  {
		  //C/C++ Build > Settings > Tool Settings > MCU GCC Linker > Miscellaneous  -u _printf_float
		  //printf("Tick %lu (loop count = %lu cb = %lu)  vdda = %0.2f temp = %0.2f \r\n", now / 1000, loop_cnt, cb, vdda, temp); pas bon

		  //printf("Tick %lu (loop count = %lu\r\n", now / 1000, loop_cnt);
		  if (adcFlag!=RESET)
		  {
			  adcFlag = RESET;

	  		  HAL_ADC_Stop_DMA(&hadc3);

	  		  /* InterQuartile Mean */

	  		  for(k=0;k<CHNB;k++)

	  		  {

	  		    j = 0; adcChAvg[k] = 0;

	  		    for(i=k;i<ADCBUFSIZE;i+=CHNB)

	  		    {

	  		      adcChBuf[j++] = adcBuf[i];

	  		    }

	  		    bubblesort(adcChBuf,BUFSIZE);

	  		    for (i=QBUFSIZE;i<(BUFSIZE-QBUFSIZE);i++)

	  		    {

	  		      adcChAvg[k] += adcChBuf[i];

	  		    }

	  		    adcChAvg[k] = adcChAvg[k] / HBUFSIZE;

	  		  }


	  		  SCB_InvalidateDCache_by_Addr((uint32_t*)adcBuf, sizeof(adcBuf));

	  		  vDDA = __LL_ADC_CALC_VREFANALOG_VOLTAGE(adcChAvg[0],LL_ADC_RESOLUTION_16B);

	  		  temperatureDegC =   __LL_ADC_CALC_TEMPERATURE(vDDA,adcChAvg[1],LL_ADC_RESOLUTION_16B);

	  		  //printf("%dmV | %dDegC\r\n",(int)vDDA,(int)temperatureDegC);

	  		  HAL_ADC_Start_DMA(&hadc3,(uint32_t*)adcBuf,ADCBUFSIZE);

	  		}

	  		  loop_cnt = 0;
	  		  next_tick = now + 1000;


	  	  }
	  	  if (ButtonState)
	  	  {
	  		  //printf("Button Press \r\n");

	  		  ++blink_delay;

	  		  if(blink_delay >= sizeof(blink_delays) / sizeof(blink_delays[0]))
	  		  {
	  			  blink_delay = 0;
	  		  }
	  		  ButtonState = BUTTON_RELEASED;
	  	  }

	  	  ++loop_cnt;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV256;
  hadc3.Init.Resolution = ADC_RESOLUTION_16B;
  hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = ENABLE;
  hadc3.Init.NbrOfConversion = 2;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc3.Init.OversamplingMode = DISABLE;
  hadc3.Init.Oversampling.Ratio = 1;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_BDMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_BDMA_CLK_ENABLE();

  /* DMA interrupt init */
  /* BDMA_Channel0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BDMA_Channel0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(BDMA_Channel0_IRQn);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LDG_GPIO_Port, LDG_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LDY_GPIO_Port, LDY_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_BTN_Pin */
  GPIO_InitStruct.Pin = USER_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_BTN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LDG_Pin */
  GPIO_InitStruct.Pin = LDG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LDG_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LDY_Pin */
  GPIO_InitStruct.Pin = LDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LDY_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(USER_BTN_EXTI_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USER_BTN_EXTI_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


// cette fonction returne l'additien sous une valeur int des 2 valurs donne a la fonction
int integer_add(int x, int y){
	int result;
	result = x + y;
	return result;
}

void sum1(s_struct *s) {
	s->c = s->a + s->b;
}



//relier usart3 a printf
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart3, (uint8_t*)ptr, len, HAL_MAX_DELAY);
	return len;
}
//usart 3 reception de données
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART3)
	{
		oldPos = newPos;	// Update the last position before copying new data

		/* If the data in large and is about to exceed the buffer size, we have to route it to the start of the buffer		 *
		 * This is to maintain the circular buffer
		 * The old data in the main buffer will be overlapped
		 */
		if (oldPos+Size > MainBuf_SIZE)	// If the current position + new data size is greater then the main buffer
		{
			uint16_t datatocopy = MainBuf_SIZE-oldPos;	// Find out how much space is left in the main buffer
			memcpy ((uint8_t *)MainBuf+oldPos, RxBuf, datatocopy);	// Copy data in that remaining space

			oldPos = 0;	// Point to the start of the buffer
			memcpy ((uint8_t *)MainBuf, (uint8_t *)RxBuf+datatocopy, (Size-datatocopy));	// Copy the remaining data
			newPos = (Size-datatocopy);	// Update de position
		}

		/* If theb current position + new data size is less than the main buffer
		 * We will simply copy the data into the buffer and update the position
		 */
		else
		{
			memcpy ((uint8_t *)MainBuf+oldPos, RxBuf, Size);
			newPos = Size+oldPos;
		}

		/* Stat the DMA again */

		HAL_UARTEx_ReceiveToIdle_DMA(&huart3, RxBuf, RxBuf_SIZE);
		__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
	}


	/***************************	PROCESS (LITTLE) THE DATA HERE	***************************/

	/* Let's say we want to check for the keyword "OK" within our incoming data */

	for(int i=0; i<Size-1; i++)
	{
		if ((RxBuf[i] == 'O') && (RxBuf[i+1] == 'K'))
		{
		isOK = 1;
		}
	}
}

//interupt bouton
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == USER_BTN_Pin) {
    // Bouton pressé
	  ButtonState = BUTTON_PRESSED;
  }
}

//optimization de timer HAL_GetTick();
inline uint32_t HAL_GetTick(void)
{
  return uwTick;
}

void bubblesort(uint16_t *array, uint16_t length)
{
   uint16_t i, j,tmp;

   for (i = 1; i < length ; i++)
   {
      for (j = 0; j < length - i ; j++)
      {
          if (array[j] > array[j+1])
          {
              tmp = array[j];
              array[j] = array[j+1];
              array[j+1] = tmp;
          }
      }
   }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{

	adcFlag = SET;
//	SCB_InvalidateDCache_by_Addr((uint32_t*)ADC_VAL, sizeof(ADC_VAL));
//	vDDA = __LL_ADC_CALC_VREFANALOG_VOLTAGE(ADC_VAL[0],LL_ADC_RESOLUTION_16B);
//	temperatureDegC =   __LL_ADC_CALC_TEMPERATURE(vDDA,ADC_VAL[1],LL_ADC_RESOLUTION_16B);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
