void WakeUpRoutine(char stripElement[], int pbSpeed)
{
	
}

void SleepRoutine(char stripElement[], int pbSpeed)
{
	
}

void StartUpRoutine(char stripElement[], int pbSpeed)
{
	
}

void PowerDownRoutine(char stripElement[], int pbSpeed)
{
	
}

void WakeUpAlert(char stripElement[], int pbSpeed)
{
	
}

void LeaveAlert(char stripElement[], int pbSpeed)
{
	
}

void NoonAlert(char stripElement[], int pbSpeed)
{
	
}

void EveningAlert(char stripElement[], int pbSpeed)
{
	
}

void PrimeTimeAlert(char stripElement[], int pbSpeed)
{
	
}

//=================================================================================================

CRGB[] SetDefaultTheme(char stripElement[], CRGB leds[])
{
	if(strcmp(stripElement,"SHELF") == 0)
	{
		int shelf = 0;
		
		for(int i = 0 + (shelf * 17); i<=17 + (shelf * 17) ; i++) leds[i] = 0x00bfff;
		shelf++;
		for(int i = 1 + (shelf * 17); i<=17 + (shelf * 17) ; i++) leds[i] = 0xff7f00;
		shelf++;
		
		for(int i = 1 + (shelf * 17); i<=9 + (shelf * 17) ; i++) leds[i] = 0xff0000;
		for(int i = 9 + (shelf * 17); i<=17 + (shelf * 17) ; i++) leds[i] = 0x00bfff;

		
		shelf++;
		for(int i = 1 + (shelf * 17); i<=17 + (shelf * 17) ; i++) leds[i] = 0xff0000;
		shelf++;
		for(int i = 1 + (shelf * 17); i<=17 + (shelf * 17) ; i++) leds[i] = 0x00edff;
		shelf++;
		for(int i = 1 + (shelf * 17); i<=17 + (shelf * 17) ; i++) leds[i] = 0xf2ff00;
	}
}










