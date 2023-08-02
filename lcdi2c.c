#include <stdint.h>
#include <stdlib.h>     // biblioteca com funÃ§Ãµes de tratamento de strings

 /*
#if defined LCD_I2C_NO_STREAM
  #define LCD_I2C_START()   i2c_start()
  #define LCD_I2C_STOP()    i2c_stop()
  #define LCD_I2C_WRITE(x)  i2c_write(x)
  #define LCD_I2C_READ(x)   i2c_read(x)
  
#elif defined LCD_I2C_STREAM
  #define LCD_I2C_START()   i2c_start(LCD_I2C_STREAM)
  #define LCD_I2C_STOP()    i2c_stop(LCD_I2C_STREAM)
  #define LCD_I2C_WRITE(x)  i2c_write(LCD_I2C_STREAM, x)
  #define LCD_I2C_READ(x)   i2c_read(LCD_I2C_STREAM, x)
#else
  #define LCD_I2C_START()   i2c_start(LCD_I2C)
  #define LCD_I2C_STOP()    i2c_stop(LCD_I2C)
  #define LCD_I2C_WRITE(x)  i2c_write(LCD_I2C, x)
  #define LCD_I2C_READ(x)   i2c_read(LCD_I2C, x)
*/
  #define LCD_I2C_START()   i2c_start()
  #define LCD_I2C_STOP()    i2c_stop()
  #define LCD_I2C_WRITE(x)  i2c_write(x)
  #define LCD_I2C_READ(x)   i2c_read(x)
 
//#endif


#define LCD_BACKLIGHT          0x08
#define LCD_NOBACKLIGHT        0x00
#define LCD_FIRST_ROW          0x80
#define LCD_SECOND_ROW         0xC0
#define LCD_THIRD_ROW          0x94
#define LCD_FOURTH_ROW         0xD4
#define LCD_CLEAR              0x01
#define LCD_RETURN_HOME        0x02
#define LCD_ENTRY_MODE_SET     0x04
#define LCD_CURSOR_OFF         0x0C
#define LCD_UNDERLINE_ON       0x0E
#define LCD_BLINK_CURSOR_ON    0x0F
#define LCD_MOVE_CURSOR_LEFT   0x10
#define LCD_MOVE_CURSOR_RIGHT  0x14
#define LCD_TURN_ON            0x0C
#define LCD_TURN_OFF           0x08
#define LCD_SHIFT_LEFT         0x18
#define LCD_SHIFT_RIGHT        0x1E


#ifndef LCD_TYPE
   #define LCD_TYPE 2           // 0=5x7, 1=5x10, 2=2 lines
#endif


int8 RS;
unsigned int8 i2c_addr;
unsigned int8 backlight_val = LCD_BACKLIGHT;

void LCD_Write_Nibble(unsigned int8 n);
void LCD_Cmd(unsigned int8 Command);
void lcd_gotoxy(unsigned int8 col, unsigned int8 row);
void lcd_putc(unsigned int8 LCD_Char);
void LCD_Begin(unsigned int8 _i2c_addr);
void Backlight();
void noBacklight();
void Expander_Write(unsigned int8 value);

void LCD_Write_Nibble(unsigned int8 n) {
  n |= RS;
  Expander_Write(n);
  Expander_Write(n | 0x04);
  delay_us(1);
  Expander_Write(n & 0xFB);
  delay_us(50);
}

void LCD_Cmd(unsigned int8 Command) {
  RS = 0;
  LCD_Write_Nibble(Command & 0xF0);
  LCD_Write_Nibble((Command << 4) & 0xF0);
}

void lcd_gotoxy(unsigned int8 col, unsigned int8 row) {
  switch(row) {
    case 2:
      LCD_Cmd(0xC0 + col-1);
      break;
    case 3:
      LCD_Cmd(0x94 + col-1);
      break;
    case 4:
      LCD_Cmd(0xD4 + col-1);
    break;
    default:      // case 1:
      LCD_Cmd(0x80 + col-1);
  }
}

void lcd_putc(unsigned int8 LCD_Char){
  RS = 1;
  LCD_Write_Nibble(LCD_Char & 0xF0);
  LCD_Write_Nibble((LCD_Char << 4) & 0xF0);
}

void lcd_init(unsigned int8 _i2c_addr) {
  i2c_addr = _i2c_addr;
  Expander_Write(0);
  delay_ms(40);
  LCD_Cmd(3);
  delay_ms(5);
  LCD_Cmd(3);
  delay_ms(5);
  LCD_Cmd(3);
  delay_ms(5);
  LCD_Cmd(LCD_RETURN_HOME);
  delay_ms(5);
  LCD_Cmd(0x20 | (LCD_TYPE << 2));
  delay_ms(50);
  LCD_Cmd(LCD_TURN_ON);
  delay_ms(50);
  LCD_Cmd(LCD_CLEAR);
  delay_ms(50);
  LCD_Cmd(LCD_ENTRY_MODE_SET | LCD_RETURN_HOME);
  delay_ms(50);
}

void Backlight() {
  backlight_val = LCD_BACKLIGHT;
  Expander_Write(0);
}

void noBacklight() {
  backlight_val = LCD_NOBACKLIGHT;
  Expander_Write(0);
}

void Expander_Write(unsigned int8 value) {
  //i2c_start(I2C_LCD);
  //i2c_write(I2C_LCD, i2c_addr);
  //i2c_write(I2C_LCD, value | backlight_val);
  LCD_I2C_START();
  LCD_I2C_WRITE(i2c_addr);
  LCD_I2C_WRITE(value | backlight_val);
  LCD_I2C_STOP();

}

