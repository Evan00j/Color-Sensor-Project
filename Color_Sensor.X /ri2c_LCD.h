/*
 * Hi-Tech C I2C library for 16F1825
 * Routines for using an LCD with the PCF8574 chip and i2c buss
 * reversed for the mjkldz interface board  data pin 0-3, rs=6, rw=5, E=4, BkL=7
 * Low on R/W is for Write, RS is cmd=0, E write on Hi to Lo, read just after Lo to Hi
 * orignal data pin 4-7, rs=6, rw=5, E=2, BkL=7
*/

#define I2C_WRITE 0
#define I2C_READ 1
#define _XTAL_FREQ 4000000.0    /*for 4mhz*/

//read the busy bit from the LCD display using the PCF8574 I2C interface
unsigned char rI2C_LCD_Busy(unsigned char Slave) {

	unsigned char read_byte, Temp1;

	i2c_Start();      					// send Start
	i2c_Address(Slave, I2C_WRITE);	// Send  slave address with write operation 
	i2c_Write(0x2F);					// Set 4 data bits on the PCF8574 to HI (open coll)
	i2c_Write(0x3F);					// Now raise E to Hi, data gets output 4 bit mode
	i2c_Restart();						// Change modes to read
	i2c_Address(Slave, I2C_READ);	// Send slave address with read operation

//	__delay_ms(100.0); 		//Use#define _XTAL_FREQ 4000000.0    /*for 4mhz*/ delay to prevents hangs for missing LCD, comment out while
	while((i2c_Read(1))& 0x08);			// Wait till bit 3 turns to 0 use bitwise and

	Temp1 = i2c_Read(0);
	Temp1 &= 0x07;
	Temp1 = Temp1 << 4;
	i2c_Restart();
	i2c_Address(Slave, I2C_WRITE);
	i2c_Write(0x2F);					// Set 4 data bits on the PCF8574 to HI (open coll)
	i2c_Write(0x3F);					// Now raise E to Hi, data gets output 4 bit mode
	i2c_Restart();						// Change modes to read	
	i2c_Address(Slave, I2C_READ);						
	read_byte = i2c_Read(0);			// Read lo byte
	read_byte &= 0x0F;
	read_byte += Temp1;
 	i2c_Stop();	// send Stop
	return read_byte;					// Now return cursor position	
										// If more than one byte to be read, (0) should
										// be on last byte only

}

//rI2C_LCD_Init() sets up the LCD, ready, turned on, 4 bit 2 line mode and cleared @ home
void rI2C_LCD_Init(unsigned char Slave) {
unsigned char Tvar1;

	i2c_Start();      					// send Start
	i2c_Address(Slave, I2C_WRITE);	// Send  slave address with write operation 
	i2c_Write(0x93);					// Send the 3x command 3 times
	i2c_Write(0x83);					// one	
	i2c_Write(0x93);					// Send the 3x command 3 times
	i2c_Write(0x83);					// two	
	i2c_Write(0x93);					// Send the 3x command 3 times
	i2c_Write(0x83);					// three	
	i2c_Write(0x92);					// set in 4 bit mode
	i2c_Write(0x82);	
	i2c_Write(0x90);					// turn on and cursor blink, underline & home
	i2c_Write(0x80);	
	i2c_Write(0x1C);					// low byte
	i2c_Write(0x0C);
	Tvar1=rI2C_LCD_Busy(Slave);
 	i2c_Stop();
}

//rI2C_LCD_Pos(unsigned char Pos) moves cursor to specified position top line=0-F bottom 40-4F
void rI2C_LCD_Pos(unsigned char Slave,unsigned char Pos) {
unsigned char HiBytex, LoBytex;
	HiBytex = (Pos & 0x70)>> 4;
	LoBytex = (Pos & 0x0F);
	i2c_Start();      					// send Start
	i2c_Address(Slave, I2C_WRITE);	// Send  slave address with write operation 
	i2c_Write(HiBytex | 0x18);					// send Hi byte  bit 7 is the pos command
	i2c_Write(HiBytex | 0x08);					// 	
	i2c_Write(LoBytex | 0x10);					// Send Lo byte	
	i2c_Write(LoBytex | 0x00);
	Pos =rI2C_LCD_Busy(Slave);
 	i2c_Stop();
}

//rIC2_LCD_Write_String(unsigned char *, char Max)  sends string to the I2C LCD display max=16 chars
void rI2C_LCD_SWrite(unsigned char Slave,unsigned char * Data, char Max){
	int j, k;
	unsigned char H, L, Pos;
	i2c_Start();      					// send Start
	i2c_Address(Slave, I2C_WRITE);	// Send  slave address with write operation 

	for (j=0; j<Max; j++){
	
	H = (Data[j] & 0xF0) >> 4;
	L = Data[j] & 0x0F ;

	i2c_Write(H | 0x50);					// Write hi byte
	i2c_Write(H | 0x40);	
	i2c_Write(L | 0x50);					// low byte
	i2c_Write(L | 0x40);

	}
	Pos = rI2C_LCD_Busy(Slave);
 	i2c_Stop();	
}

//rI2C_LCD_Command(unsigned char); sends a command to the LCD
void rI2C_LCD_Command(unsigned char Slave,unsigned char Cmd){

unsigned char HiBytex, LoBytex, Pos;
	HiBytex = (Cmd & 0xF0) >> 4;
	LoBytex = Cmd & 0x0F ;	
	i2c_Start();      					// send Start
	i2c_Address(Slave, I2C_WRITE);	// Send  slave address with write operation 
	i2c_Write(HiBytex | 0x10);					// send Hi byte  
	i2c_Write(HiBytex | 0x00);					// 	
	i2c_Write(LoBytex | 0x10);					// Send Lo byte	
	i2c_Write(LoBytex | 0x00);
	Pos = rI2C_LCD_Busy(Slave);
 	i2c_Stop();
}