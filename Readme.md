## TPJ655 second part
*Developed as part of the Graduate Capstone Project at Seneca polytechnic under the supervision of Prof. Karen Craigs*
*Developed by Jiseok Song and Ashburne Blak Gano*
*Make sure that tft_espi library needs proper driver to be defined in User_Setup.h. Mine used ILI9341_DRIVER*
*This project is a cooperation game with 2 players, each has one console*
*It has 5 stages and players should communicate in only verbal ways.*
1. Simple random 4 numbers in 1P digit display to be typed using 2P keypad
2. Simple 4x4 pattern in 1P 4x4 led matrix to be clicked by 2P 4x4 button matrix
3. 1P TFT display will show random symbol and it will be chosen from 5 options on 2P side tft display
4. 1P TFT display will show a masked symbol and 1P will use joystick to move the pointer to remove the mask and this symbol will be chosen from 5 options on 2P side tft display
5. 1P TFT display will show 2 symbols and the difference between these two will be chosen from 5 options on 2P side tft display
*After that, 2P motor will be triggered and rfid card key will show up. Tap this card to 1P rfid tag and win the game.*
*Demo*

https://github.com/user-attachments/assets/f2b18f92-be52-4de3-94df-3f7aa46961ff

For detailed documentation, read TPJ Project Report.docx
