#pragma once
#include "EEPROM.h"
#include "GlobalShare.h"
#include "logger.h"
#define MAX_STRING_SIZE 50

Logger communicationLogger("Comms", INFO);

class Communication
{
    
    private:
        char incomingMessage[MAX_STRING_SIZE];
        int UART;
        bool echo;
        void float2str(float var1, char * str, int dec);
        int intToStr(int x, char str[], int d);
        void reverse(char* str, int len);
        void prompt();
    public:
        void send(float var);
        void send(float var1, float var2);
        void send(float var1, float var2, float var3);
        void send(const char *string);
        Communication(int UART_PORT);
        void console();
        ~Communication();
};


constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

Communication::Communication(int UART_PORT)
{
    Serial1.begin(9600);
    Serial2.begin(9600);
    echo = false;
    communicationLogger.Send(DEBUG, "SETTING UP UART ON PORT: ", UART_PORT);
    memset(incomingMessage, 0, sizeof(incomingMessage));
}



void Communication::console()
{
    bool eofFlag = false;
    if (Serial.available() || Serial1.available() || Serial2.available())
    {
        if(strlen(incomingMessage) < MAX_STRING_SIZE)
        {
            char incomingChar = 0;
            if(Serial.available())
                incomingChar = Serial.read();
            else if(Serial1.available())
                incomingChar = Serial1.read();
            else if(Serial2.available())
                incomingChar = Serial2.read();
            
            switch (incomingChar)
            {
            case '\r':
                Serial.flush();
                if(strlen(incomingMessage) != 0)
                    eofFlag = true;
                break;
            case '\b':
                if(strlen(incomingMessage))
                {
                    if(echo)
                        Serial.write("\b \b");
                }
                
                incomingMessage[strlen(incomingMessage) - 1] = 0;
                break;
            default:
                incomingMessage[strlen(incomingMessage)] = incomingChar;
                if(echo)
                {
                    Serial.write(incomingChar);
                }
                break;
            }
        }
        else
        {
            communicationLogger.Send(WARNING, "WARNING - EXCEEDED SIZE LIMIT OF 50 - RESETING INPUT");
            memset(incomingMessage, 0, sizeof(incomingMessage));
        }
    }
    if(eofFlag && !EasyCommMode)
    {
        send("\n\r");
        char *tokens;
        Serial.printf("MESSAGE:  %s", incomingMessage);
        tokens = strtok(incomingMessage, " ");
        switch (str2int(tokens))
        {
        case str2int("help"):
        {
            //! To be stored in a better place.... should include licence stuff as well... maybe eeprom?
            send("Commands:\n\r");
            send(  "help,\n\r"
                            "home - go to home location,\n\r"
                            "sethome Az El - set home location at Az El,\n\r"
                            "gotoloc Az El - go to location at Az El,\n\r"
                            "shutdown - turn off rotator,\n\r"
                            "currentloc - get current location,\n\r"
                            "desiredloc - get destination location,\n\r"
                            "lights - turn on tower lights\n\r");
            send("For detailed help, look at the guide.\n\n\r");
            break;
        }
        case str2int("gotoloc"):
        {
            float var1, var2;
            tokens = strtok (NULL, " ");
            sscanf(tokens, "%f", &var1);
            tokens = strtok (NULL, " ");
            sscanf(tokens, "%f", &var2);
            rxAzEl.az = var1;
            rxAzEl.el = var2;
            communicationLogger.Send(INFO, "GOTOLOC SET: ", rxAzEl.az, rxAzEl.el);
            break;
        }
        
        case str2int("currentloc"):
        {    
            send(txAzEl.az, txAzEl.el);
            break;
        }

        case str2int("test"):
        {   
            
            testFlag = true;
            send("Testing.... enter qtest to exit");
            delay(1000);
            break;
        }

        case str2int("qtest"):
        {
            testFlag = false;
            send("\n\n\n\r");
            break;
        }

        case str2int("desiredloc"):
        {
            send(rxAzEl.az, rxAzEl.el);
            break;
        }

        case str2int("sethome"):
        {
            float var1, var2;
            tokens = strtok (NULL, " ");
            sscanf(tokens, "%f", &var1);
            tokens = strtok (NULL, " ");
            sscanf(tokens, "%f", &var2);
            home.az = var1;
            home.el = var2;
            EEPROM.write(ROM_HOME_AZ, home.az);
            EEPROM.write(ROM_HOME_EL, home.el);
            communicationLogger.Send(INFO, "SETHOME SET: ", home.az, home.el);
            communicationLogger.Send(INFO, "SETHOME EEPROM: ", EEPROM.read(ROM_HOME_AZ), EEPROM.read(ROM_HOME_EL));
            break;
        }

        case str2int("home"):
        {
            rxAzEl.az = EEPROM.read(ROM_HOME_AZ);
            rxAzEl.el = EEPROM.read(ROM_HOME_EL);
            communicationLogger.Send(INFO, "GOING TO HOME: ", rxAzEl.az, rxAzEl.el);
            break;
        }

        case str2int("shutdown"):
        {
            while(1)
            {
                if(Serial.available())
                {
                    communicationLogger.Send(INFO, "Awake...");
                    break;
                }
                digitalWrite(13, HIGH);
                delay(25);
                digitalWrite(13, LOW);
                delay(2500);
            }
            break;
        }

        case str2int("lights"):
        {
            //send("%f %f", rotor->CurrentLocAz(), rotor->CurrentLocEl());
            break;
        }
        
        case str2int("warranty"):
        {
            send("This program is distributed in the hope that it will be useful,\n\r"
                            "but WITHOUT ANY WARRANTY; without even the implied warranty of\n\r"
                            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\r"
                            "GNU General Public License for more details.\n\n\r"
                            "You should have received a copy of the GNU General Public License\n\r"
                            "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n\r");
            break;
        }

        case str2int("e"):
        {
            echo = !echo;
            if(echo)
            {
                logSend = true;
                send("Echo on\n\r");
            }
            else
            {
                logSend = false;
                send("Echo off\n\r");
            }
            
            break;
        }

        case str2int("autoupdate"):
        {
            digitalWrite(13, 1);
            float var1, var2;
            tokens = strtok (NULL, " ");
            sscanf(tokens, "%f", &var1);
            tokens = strtok (NULL, " ");
            sscanf(tokens, "%f", &var2);
            rxAzEl.az = var1;
            rxAzEl.el = var2;
            
            if(var1 == 0 && var2 == 0)
            {
                rxAzEl = home;
            }
            

            bool moving;
            bool homeState;
            float volts = 12;
            if(abs(txAzEl.az - rxAzEl.az) > 3 || abs(txAzEl.el - rxAzEl.el) > 3)
            {
                moving = true;
            }
            else
            {
                moving = false;
            }
            if(txAzEl.az == home.az && txAzEl.el == home.el)
            {
                homeState = true;
            }
            else
            {
                homeState = false;
            }
            
            Serial.printf("%lf %lf %lf %lf %s %lf %lf %s\n\r", txAzEl.az, txAzEl.el, rxAzEl.az, rxAzEl.el, moving ? "true" : "false", volts, tempmonGetTemp(), homeState ? "true" : "false");
            Serial1.printf("%lf %lf %lf %lf %s %lf %lf %s\n\r", txAzEl.az, txAzEl.el, rxAzEl.az, rxAzEl.el, moving ? "true" : "false", volts, tempmonGetTemp(), homeState ? "true" : "false");
            Serial2.printf("%lf %lf %lf %lf %s %lf %lf %s\n\r", txAzEl.az, txAzEl.el, rxAzEl.az, rxAzEl.el, moving ? "true" : "false", volts, tempmonGetTemp(), homeState ? "true" : "false");
            digitalWrite(13, 0);
            break;
        }

        case str2int("EC"):
            communicationLogger.Send(INFO, "Entering easycomm mode");
            EasyCommMode = true;
            break;

        case str2int("NEC"):
            communicationLogger.Send(INFO, "Exiting easycomm mode");
            EasyCommMode = false;
            break;

        default:
            if(echo)
            {
                send("Unknown command: ");
                send(incomingMessage);
                send("\r\n");
            }
            break;
        }
        memset(incomingMessage, 0, sizeof(incomingMessage));
        prompt();
    }

    else if (eofFlag && EasyCommMode)
    {
        switch (incomingMessage[0])
        {
        case 'A':
            if(incomingMessage[1] == 'Z' && incomingMessage[2] == ' ')
            {
                char str[30] = {0};
                char buf[20] = {0};
                strcat(str, "AZ");
                float2str(txAzEl.az, buf, 1);
                strcat(str, buf);
                send(str);
            }
            else if(incomingMessage[1] == 'Z')
            {
                char * value = incomingMessage + 2;
                sscanf(value, "%LF", &rxAzEl.az);
            }
            break;
        case 'E':
            if(incomingMessage[1] == 'L' && incomingMessage[2] == ' ')
            {
                char str[30] = {0};
                char buf[20] = {0};
                strcat(str, "EL");
                float2str(txAzEl.el, buf, 1);
                strcat(str, buf);
                send(str);
            }
            else if(incomingMessage[1] == 'L')
            {
                char * value = incomingMessage + 2;
                sscanf(value, "%LF", &rxAzEl.el);
            }
            break;
        
        default:
            break;
        }
    }
    
    
}

void Communication::send(float var)
{
    Serial.printf("%lf\n\r", var);
    Serial1.printf("%lf\n\r", var);
    Serial2.printf("%lf\n\r", var);
}

void Communication::send(float var1, float var2)
{
    Serial.printf("%lf %lf\n\r", var1, var2);
    Serial1.printf("%lf %lf\n\r", var1, var2);
    Serial2.printf("%lf %lf\n\r", var1, var2);
}

void Communication::send(float var1, float var2, float var3)
{
    Serial.printf("%lf %lf %lf\n\r", var1, var2, var3);
    Serial1.printf("%lf %lf %lf\n\r", var1, var2, var3);
    Serial2.printf("%lf %lf %lf\n\r", var1, var2, var3);
}

void Communication::send(const char *string)
{
    Serial.printf("%s", string);
    Serial1.printf("%s", string);
    Serial2.printf("%s", string);
}



void Communication::prompt()
{
    if(echo)
        send("DEATHBOX PROMPT -$ ");
}


void Communication::float2str(float var1, char * str, int dec)
{
    int ipart = (int)var1; 
    float fpart = var1 - (float)ipart; 
    int i = intToStr(ipart, str, 0); 

    if (dec != 0) { 
        str[i] = '.'; // add dot 
  
        // Get the value of fraction part upto given no. 
        // of points after dot. The third parameter  
        // is needed to handle cases like 233.007 
        fpart = fpart * pow(10, dec); 
  
        intToStr((int)fpart, str + i + 1, dec); 
    } 
}

int Communication::intToStr(int x, char str[], int d) 
{ 
    int i = 0; 
    while (x) { 
        str[i++] = (x % 10) + '0'; 
        x = x / 10; 
    } 
  
    // If number of digits required is more, then 
    // add 0s at the beginning 
    while (i < d) 
        str[i++] = '0'; 
  
    reverse(str, i); 
    str[i] = '\0'; 
    return i; 
} 

void Communication::reverse(char* str, int len) 
{ 
    int i = 0, j = len - 1, temp; 
    while (i < j) { 
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp; 
        i++; 
        j--; 
    } 
} 

  