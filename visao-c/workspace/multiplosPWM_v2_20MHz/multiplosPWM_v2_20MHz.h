#if F_CPU == 20000000
	#define PWM_TOP_VALUE  50000 // Top value to 20MHz
	#define PWM_PASSO 	   25000 // Passo entre cada PWM
	#define PWM_INIT_VALUE 2500
	// Macro para calculo do valor do registrador do PWM
	#define PWM_PRODUTO(VALUE)	((VALUE<<1)+(VALUE>>1))	// Multiplica por 2,5
#else
	#if F_CPU == 16000000
		#define PWM_TOP_VALUE  40000 // Top value to 20MHz
		#define PWM_PASSO 	   20000 // Passo entre cada PWM
		#define PWM_INIT_VALUE 2000
		#define PWM_PRODUTO(VALUE)	(VALUE<<1)	// Multiplica por 2,0
	#else
		#error Error 200 - F_CPU value invalid
	#endif
#endif


uint16 update_reg_value(uint16 min, uint16 max, uint8 percent)
{
	uint16 aux16;

	aux16 = (max-min)/100;
	aux16 = aux16*percent;
	aux16 = aux16 + min;
	aux16 = PWM_PRODUTO(aux16);

	return aux16;
}



