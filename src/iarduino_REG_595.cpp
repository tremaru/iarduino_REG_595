#include "iarduino_REG_595.h"														//
																					//
//		ПОДКЛЮЧАЕМ МАССИВ ПРЕОБРАЗОВАНИЯ СИМВОЛОВ В СЕГМЕНТЫ:						//
//extern const uint8_t arr_CharTo7LED[];												//
																					//
//		ИНИЦИАЛИЗАЦИЯ РЕГИСТРОВ:													//
void	iarduino_REG_595::begin(uint8_t Sum){										//	Параметр: Sum - количество регистров.
			sumReg=Sum;																//	Сохраняем указанное количество регистров.
			pinMode(pinHCP, OUTPUT); digitalWrite(pinHCP, flgInvertSCL);			//	Конфигурируем вывод тактирования сдвиговых регистров.
			pinMode(pinTCP, OUTPUT); digitalWrite(pinTCP, flgInvertSCL);			//	Конфигурируем вывод тактирования записи в регистр защёлки.
			pinMode(pinDAT, OUTPUT); digitalWrite(pinDAT, flgInvertDAT);			//	Конфигурируем вывод линии данных.
			clear();																//	Чистим все регистры.
}																					//
																					//
//		ОЧИСТКА ВСЕХ РЕГИСТРОВ:														//
void	iarduino_REG_595::clear(void){												//
			for(uint8_t i=0; i<sumReg; i++){ sendByte(0); } latch();				//	Отправляем 0 во все сдвиговые регистры и записываем их в регистры защёлки.
}																					//
																					//
//		ОТПРАВКА БИТА В СДВИГОВЫЙ РЕГИСТР (БЕЗ ЗАПИСИ В РЕГИСТР ЗАЩЁЛКИ):			//
void	iarduino_REG_595::sendBit(bool Data){										//	Параметр: Data - отправляемый бит.
			digitalWrite(pinDAT, Data^flgInvertDAT); delayMicroseconds(timDelay);	//	Устанавливаем уровень.
			digitalWrite(pinHCP,     !flgInvertSCL); delayMicroseconds(timDelay);	//	Тактируем отправку в сдвиговый регистр.
			digitalWrite(pinHCP,      flgInvertSCL); delayMicroseconds(timDelay);	//
}																					//
																					//
//		ОТПРАВКА БАЙТА В СДВИГОВЫЙ РЕГИСТР (БЕЗ ЗАПИСИ В РЕГИСТР ЗАЩЁЛКИ):			//
void	iarduino_REG_595::sendByte(uint8_t Data){									//	Параметр: Data - отправляемый байт.
			for(uint8_t i=0; i<8; i++){ sendBit(Data&(1<<(7-i))); }					//	Отправляем байт старшим битом вперёд.
}																					//
																					//
//		ОТПРАВКА МАССИВА БАЙТ В СДВИГОВЫЙ РЕГИСТР (БЕЗ ЗАПИСИ В РЕГИСТР ЗАЩЁЛКИ):	//
void	iarduino_REG_595::sendArray(uint8_t* Data, uint8_t Sum){					//	Параметры: Data - массив отправляемых байтов, Sum - количество отправляемых байтов.
			for(uint8_t i=0; i<Sum; i++){ sendByte(Data[i]); }						//	Отправляем байт младшим байтом вперёд.
}																					//
																					//
//		ОТПРАВКА СИМВОЛА В СДВИГОВЫЙ РЕГИСТР (БЕЗ ЗАПИСИ В РЕГИСТР ЗАЩЁЛКИ):		//	Символ отображается согласно правилам setSegments().
void	iarduino_REG_595::sendChar(const char Sim, bool Point){						//	Параметры: Sim - символ, Point - добавить точку к символу.
			uint8_t i = _getCharTo7LED(Sim);										//	Получаем байт сегментов для символа Sim.
			if( Point ){ i |= _getCharTo7LED('.'); }								//	Добавляем сегмент точки.
			for(uint8_t j=0; j<8; j++){sendBit((i&(1<<(strSegments[j]-'a')))?1:0);}	//	Отправляем биты в соответствии с правилами strSegments
}																					//
																					//
//		ЗАПИСЬ ДАННЫХ ИЗ СДВИГОВЫХ РЕГИСТРОВ В РЕГИСТРЫ ЗАЩЁЛКИ:					//
void	iarduino_REG_595::latch(void){												//
			digitalWrite(pinTCP, !flgInvertSCL); delayMicroseconds(timDelay);		//	Тактируем запись данных из сдвиговых регистров в регистры защёлки.
			digitalWrite(pinTCP,  flgInvertSCL); delayMicroseconds(timDelay);		//
}																					//
																					//
//		ПОРЯДОК ПОДКЛЮЧЕНИЯ СЕГМЕНТОВ:												//
void	iarduino_REG_595::setSegments(String Str){									//	Параметр: Str - строка следования сегментов.
			if(Str.length()>=8){for(uint8_t i=0; i<9; i++){strSegments[i]=Str[i];}}	//	Сохраняем строку Str в strSegments.
}																					//
																					//
//		ВЫВОД ЧИСЛА СО ЗНАКОМ В УКАЗАННОЙ СИСТЕМЕ СЧИСЛЕНИЯ:						//
void	iarduino_REG_595::print( int64_t Num, int Sys){								//
			if( Sys<2 ){ Sys=2; } if( Sys>35 ){ Sys=35; }							//	Ограничиваем диапазон систем счисления.
			bool f = Num<0; if( f ){ Num *= -1; }									//	Избавляемся от знака.
			char Str[(sumReg+1)]; Str[sumReg]=0; uint8_t len=sumReg;				//	Создаём строку Str и переменную для получения количества разрядов len.
			for(uint8_t i=0; i<sumReg; i++){										//
				Str[sumReg-1-i]=Num%Sys+((Num%Sys)<10?'0':'A'-10); Num/=Sys;		//	Преобразуем очередную цифру числа в символ строки: 123%10 = '3'; 123/10 = 12;
				if(!Num){if(f&&(i+1)<sumReg){i++;Str[sumReg-1-i]='-';}len=i+1;i=sumReg;} // Добавляем символ '-' для отрицательного числа. Сохраняем количество разрядов и выходим из цикла.
			}	print(&Str[sumReg-len]);											//	Выводим строку (от старшего разряда до конца строки).
}																					//
																					//
//		ВЫВОД ЧИСЛА БЕЗ ЗНАКА В УКАЗАННОЙ СИСТЕМЕ СЧИСЛЕНИЯ:						//
void	iarduino_REG_595::print(uint64_t Num, int Sys){								//
			if( Sys<2 ){ Sys=2; } if( Sys>35 ){ Sys=35; }							//	Ограничиваем диапазон систем счисления.
			char Str[(sumReg+1)]; Str[sumReg]=0; uint8_t len=sumReg;				//	Создаём строку Str и переменную для получения количества разрядов len.
			for(uint8_t i=0; i<sumReg; i++){										//
				Str[sumReg-1-i]=Num%Sys+((Num%Sys)<10?'0':'A'-10); Num/=Sys;		//	Преобразуем очередную цифру числа в символ строки: 123%10 = '3'; 123/10 = 12;
				if( !Num ){ len=i+1; i=sumReg; }									//	Сохраняем количество разрядов и выходим из цикла.
			}	print(&Str[sumReg-len]);											//	Выводим строку (от старшего разряда до конца строки).
}																					//
																					//
void	iarduino_REG_595::print(double Num, int Sum){								//	Вывод числа с указанным количеством знаков после запятой.
			uint8_t i, f=(Num<0.0f)?1:0; if(f){Num*=-1.0f;}							//	Флаг наличия отрицательного знака.
			uint32_t j=(uint32_t)Num;												//	Переменная для хранения дробного числа, как целого
			String Str;																//	Строка для вывода числа.
		//	Преобразуем double в округлённое uint32_t без точки:					//
			for(uint8_t i=0; i<Sum; i++){ Num*=10.0f; } Num+=0.5f; j=(uint32_t)Num;	//	Получаем огруглённое число до и после запятой, без точки (Num=12.3 => j=123).
		//	При переполнении заменяем число на 999...:								//
			Str=(String)j;															//	Сохраняем число в строку, чтоб определить его длину без точки.
			if( Str.length()+(f?1:0)>sumReg ){										//	Если дина числа со знаком без точки превышает количество регистров (разрядов), значит число не помещается.
				j=0; for(i=0; i<sumReg-(f?1:0); i++){j*=10; j+=9;}					//	Меняем число на все девятки.
			}																		//
		//	Заполняем строку для вывода:											//
			Str="";																	//
			for(i=0; i<Sum; i++){ Str=(j%10)+Str; j/=10; }							//	Заполняем строку Str цифрами дробной части числа.
			if(Sum){Str='.'+Str;} if(j==0){Str='0'+Str;}							//	Добавляем в строку Str точку и, при необходимости, ведущий 0.
			while(j){ Str=(j%10)+Str; j/=10; } if(f){Str='-'+Str;}					//	Заполняем строку Str цифрами целой части числа и ставим отрицательный знак (если он есть).
		//	Выводим строку на дисплей:												//
			print(Str);																//	Выводим строку (от старшего разряда до конца строки).
}																					//
																					//
//		ВЫВОД СИМВОЛА:																//
void	iarduino_REG_595::print(const char Sim, bool Point){						//	Параметры: Sim - символ, Point - добавить точку к символу.
			if( flgClearBefore ){ for(uint8_t i=1; i<sumReg; i++){sendByte(0);} }	//	Чистим предыдущие разряды.
			sendChar(Sim, Point); latch();											//	Отправляем символ в сдвиговый регистр и записываем.
}																					//
																					//
//		ВЫВОД СТРОКИ:																//
void	iarduino_REG_595::print(String Str){										//	Параметр: Str - строка.
			uint8_t i,j,k,l=Str.length()?1:0;										// 
			for(i=1; i<Str.length(); i++){ if( Str[i]!='.' && Str[i]!=',' ){l++;}}	//	Определяем размер строки без точек.
			if(flgClearBefore && !flgAlignLeft){for(i=l;i<sumReg;i++){sendByte(0);}}//	Чистим предыдущие разряды.
			for( i=0; i<Str.length(); i++ ){										//	Проходим по всем символам строки.
			    if( Str[i]!='.' && Str[i]!=',' && (Str[i+1]=='.' || Str[i+1]==',') ){sendChar(Str[i],1); i++;}else{sendChar(Str[i]);} // Отправляем очередной символ (добавляя к нему точку, при необходимости) в сдвиговые регистры.
			}																		//
			if(flgAlignLeft){for(i=l; i<sumReg; i++){sendByte(0);}}					//	Чистим оставшиеся разряды.
			latch();																//	Записываем данные из сдвиговых регистров в регистры защёлки.
}																					//
																					//
//	ФУНКЦИЯ ПРЕОБРАЗОВАНИЯ СИМВОЛА В БАЙТ СЕГМЕНТОВ:								//
uint8_t	iarduino_REG_595::_getCharTo7LED(char c){									//	Параметр: c - символ.
			if(c==' ' ){ c=0; }														//
			if(c=='_' ){ c=4; }														//
			if(c=='-' ){ c=7; }														//
			if(c=='.' ){ c=8; }														//
			if(c<=8            ){return pgm_read_byte(&arr_CharTo7LED[c       ]);}	//
			if(c>='0' && c<='9'){return pgm_read_byte(&arr_CharTo7LED[c-'0'+ 9]);}	//
			if(c>='a' && c<='z'){return pgm_read_byte(&arr_CharTo7LED[c-'a'+19]);}	//
			if(c>='A' && c<='Z'){return pgm_read_byte(&arr_CharTo7LED[c-'A'+19]);}	//
			if(c=='*'          ){return pgm_read_byte(&arr_CharTo7LED[      45]);}	//
			return 0;																//
};																					//	
