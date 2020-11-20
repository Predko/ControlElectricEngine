#ifndef LCD_DEBUG_INFO_H
#define LCD_DEBUG_INFO_H

#ifdef LCD_DEBUG_ENABLE

    #include <LiquidCrystal_I2C.h>

    // Set the LCD address to 0x27 for a 16 chars and 2 line display
    LiquidCrystal_I2C lcd(0x27, 16, 2);
 
    #define LCD_INIT    lcd.init(); lcd.backlight()

    #define LCD_DEBUG(col, row, str, info)   lcd.setCursor((col), (row)); lcd.print((str)); lcd.print((info))
    #define LCD_DEBUGSTR(col, row, str)   lcd.setCursor((col), (row)); lcd.print((str))
#else
    #define LCD_INIT
    
    #define LCD_DEBUG(col, row, str, info)
    #define LCD_DEBUGSTR(col, row, str)
#endif

#endif