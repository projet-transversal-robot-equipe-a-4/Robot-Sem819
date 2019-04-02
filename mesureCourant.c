int mesureCourant(){
		sfr16 ADC0 = 0xBE;
		float Mesure_courant;
		float Courant;

		void InitCourant(){
			WDTCN = 0xDE;   // Dévalidation du watchdog 
			WDTCN = 0xAD;
			OSCXCN &=0x00; // reset de EXT Oscillator
			OSCXCN |= 0x67;	//EXT Oscillator	(Bit4: divisé 2)
			while((OSCXCN & 0x80) != 0x80) {}
			OSCICN |= 0x08; // enable externate oscillator
			OSCICN &= 0xFB; // disable internal osci
			XBR2 |= 0x40;	// crossbar enable
			REF0CN |= 0x03; // enable Vref	
			EA = 1;  // enable interrupt
			EIE2 |= 0x02; // interrupt end conversion
		}
		void Config_ADC(){
			AMX0CF = 0x00; // Configuration register AIN0 et AIN1 independantes + AIN6 et AIN7 inde
			AMX0SL = 0x00; // select AIN0
			ADC0CF = 0x80; // gain de 1 et config horloge SAR avec oscillo externe
			AD0TM = 0; // track ing continous
			AD0CM0 = 0; // start conversion mode select
			AD0CM1 = 0; // start conversion mode select
			AD0LJST = 0; // left justified
			AD0EN = 1; // enable ADC0
			AD0INT = 0; // flag set to 0
			AD0WINT = 0; //   ADC 0 WINDOW COMPARE INTERRUPT FLAG
		}

		void AnalogEntree(){
			AMX0SL = 0x00; // select AIN0
			AD0BUSY = 1; // start conversion
			while (AD0BUSY == 1) {}
			Courant = Mesure_courant;
		}
		void finConversion() interrupt 15 {
			AD0INT = 0; // flag set 0
			Mesure_courant = (ADC0/4096.0)*10.192;
		}
	}
		InitCourant();
		Init2();
		Config_ADC();
		AnalogEntree();
		return Courant;
}