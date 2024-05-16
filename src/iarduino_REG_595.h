//	Библиотека для работы с 8-и битными сдвиговыми регистрами HC595/HTC595.
//  Версия: 1.0.2
//  Последнюю версию библиотеки Вы можете скачать по ссылке: http://iarduino.ru/
//  Подробное описание функции бибилиотеки доступно по ссылке: http://wiki.iarduino.ru/
//  Библиотека является собственностью интернет магазина iarduino.ru и может свободно использоваться и распространяться!
//  При публикации устройств или скетчей с использованием данной библиотеки, как целиком, так и её частей,
//  в том числе и в некоммерческих целях, просим Вас опубликовать ссылку: http://iarduino.ru
//  Автор библиотеки: Панькин Павел
//  Если у Вас возникли технические вопросы, напишите нам: shop@iarduino.ru

#ifndef iarduino_REG_595_h
#define iarduino_REG_595_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "char_to_7LED.h"

class	iarduino_REG_595{
	public:				iarduino_REG_595	(uint8_t HCP, uint8_t TCP, uint8_t DAT)								//	Конструктор красса.							(вывод тактирования сдвиговых регистров, вывод тактирования регистра защёлки, вывод данных)
											{pinHCP= HCP; pinTCP= TCP; pinDAT= DAT;}							//
																												//
		void			setPulseWidth		(uint8_t      Width				){ timDelay=Width;           }		//	Установить ширину импульсов в мкс.			(1 ... 255) Длительность импульсов и пауз на линиях тактирования в мкс.
		void			setInvertSCL		(bool         Invert			){ flgInvertSCL=Invert;      }		//	Установить инверсию логических уровней.		(true/false) Инверсия логических уровней на выводах тактирования: HCP и TCP.
		void			setInvertDAT		(bool         Invert			){ flgInvertDAT=Invert;      }		//	Установить инверсию логических уровней.		(true/false) Инверсия логических уровней на выводе данных DAT.
		void			setSegments			(String       Str				);									//	Установить порядок подключения сегментов к регистрам.		(строка) По умолчанию "hgfedcba", означает что старший бит (вывод Q7) является сегментом h, а младший бит (вывод Q0) является сегментом a.
		void			setSegments			(const char*  Str				){ setSegments((String)Str); }		//	Установить порядок подключения сегментов к регистрам.		(строка) По умолчанию "hgfedcba", означает что старший бит (вывод Q7) является сегментом h, а младший бит (вывод Q0) является сегментом a.
		void			setClearBefore		(bool         Clear				){ flgClearBefore=Clear;     }		//	Установить очистку дисплея перед выводом функцией print().	(true/false) По умолчанию дисплей очищается.
		void			setAlignLeft		(bool         Left				){ flgAlignLeft=Left;        }		//	Установить выравнивание по левому краю для функции print().	(true/false) По умолчанию выравнивание по правому краю.
																												//
		void			begin				(uint8_t      Sum				);									//	Инициализация регистров. (количество регистров).
		void			clear				(void							);									//	Очистка всех регистров.
		void			sendBit				(bool         Data				);									//	Отправка одного бита    в сдвиговый регистр, без записи в регистр защёлки.
		void			sendByte			(uint8_t      Data				);									//	Отправка одного байта   в сдвиговый регистр, без записи в регистр защёлки. Старшим битом вперёд.
		void			sendArray			(uint8_t*     Data, uint8_t Sum	);									//	Отправка массива байт   в сдвиговый регистр, без записи в регистр защёлки. Младшим байтом вперёд.
		void			sendChar			(const char   Sim, bool Point=0	);									//	Отправка одного символа в сдвиговый регистр, без записи в регистр защёлки. Символ отображается согласно правилам setSegments().
		void			latch				(void							);									//	Запись данных из сдвиговых регистров в регистры защёлки.
#ifdef RENESAS_CORTEX_M4																						//
		void			print				(         int Num, int Sys=10	){ print(( int64_t) Num, Sys); }	//	Вывод числа со знаком в указанной системе счисления.
		void			print				(unsigned int Num, int Sys=10	){ print((uint64_t) Num, Sys); }	//	Вывод числа без знака в указанной системе счисления.
#else																											//
		void			print				( int16_t     Num, int Sys=10	){ print(( int64_t) Num, Sys); }	//	Вывод числа со знаком в указанной системе счисления.
		void			print				(uint16_t     Num, int Sys=10	){ print((uint64_t) Num, Sys); }	//	Вывод числа без знака в указанной системе счисления.
#endif																											//
		void			print				( int32_t     Num, int Sys=10	){ print(( int64_t) Num, Sys); }	//	Вывод числа со знаком в указанной системе счисления.
		void			print				(uint32_t     Num, int Sys=10	){ print((uint64_t) Num, Sys); }	//	Вывод числа без знака в указанной системе счисления.
		void			print				( int64_t     Num, int Sys=10	);									//	Вывод числа со знаком в указанной системе счисления.
		void			print				(uint64_t     Num, int Sys=10	);									//	Вывод числа без знака в указанной системе счисления.
		void			print				(double       Num, int Sum=2	);									//	Вывод числа с указанным количеством знаков после запятой.
		void			print				(const char   Sim, bool Point=0	);									//	Вывод символа. Если второй параметр true, то символ выводится с точкой.
		void			print				(String       Str				);									//	Вывод строки.
		void			print				(const char*  Str				){ print((String)Str); }			//	Вывод строки.
	private:																									//
		uint8_t			pinHCP			=	255;																//	Вывод линии тактирования сдвиговых регистров.
		uint8_t			pinTCP			=	255;																//	Вывод линии тактирования регистра защёлки.
		uint8_t			pinDAT			=	255;																//	Вывод линии данных.
		uint8_t			sumReg			=	1;																	//	Количество последовательно соединённых регистров.
		uint8_t			timDelay		=	0;																	//	Длительность импульсов и пауз на всех линиях в мкс.
		bool			flgInvertSCL	=	false;																//	Флаг инверсии логических уровней на линиях тактирования.
		bool			flgInvertDAT	=	false;																//	Флаг инверсии логических уровней на линии данных.
		bool			flgClearBefore	=	true;																//	Флаг очистки дисплея перед выводом функцией print().
		bool			flgAlignLeft	=	false;																//	Флаг выравнивания данных выводимых функцией print() по левому краю.
		char			strSegments[9]	=	"hgfedcba";															//	Порядок следования сегментов.
																												//
		uint8_t			_getCharTo7LED		(char);																//	Функция преобразования символа в байт сегментов.
};

#endif
