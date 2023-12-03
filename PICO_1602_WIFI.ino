#include <WiFi.h>
#define LCD_RS 2
#define LCD_E 3 
#define LCD_D0 15
#define LCD_D1 14
#define LCD_D2 13
#define LCD_D3 12
#define LCD_D4 11
#define LCD_D5 10
#define LCD_D6 9
#define LCD_D7 8
#define BOTTEN1 16
#define CMD 0
#define CHR 1
#define char_per_line 16
#define LCD_line1 0x80
#define LCD_line2 0xc0
//const char* ssid = "2.4G_open";
const char* ssid = "iPhone 14p";
const char* password = "123456778";
String header;
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;
String input = "";
String co="123456789ABCDEF1";
String co2="123456789ABCDEF1";
String IP1="IP:";
char temp[35];
int tail = 0;
int counter=0;
using namespace std;
WiFiServer server(80);

void init(){
    pinMode(2,GPIO_OUT);
    pinMode(3,GPIO_OUT);
    pinMode(5,INPUT);
    pinMode(6,GPIO_OUT);
    pinMode(8,GPIO_OUT);
    pinMode(9,GPIO_OUT);
    pinMode(10,GPIO_OUT);
    pinMode(11,GPIO_OUT);
    pinMode(12,GPIO_OUT);
    pinMode(13,GPIO_OUT);
    pinMode(14,GPIO_OUT);
    pinMode(15,GPIO_OUT);
}

void triggle_E(){
    delay(1);
    digitalWrite(LCD_E,HIGH);
    delay(1);
    digitalWrite(LCD_E,LOW);
    delay(1);
}

void gpio_write(int data,int mode){
    if(mode==0){
        digitalWrite(LCD_RS,LOW);
    }
    else if(mode==1){
        digitalWrite(LCD_RS,HIGH);
    }
    digitalWrite(LCD_D7,LOW);
    digitalWrite(LCD_D6,LOW);
    digitalWrite(LCD_D5,LOW);
    digitalWrite(LCD_D4,LOW);

    if((data &0x80) == 0x80){
        digitalWrite(LCD_D7,HIGH);
    }
    if((data &0x40) == 0x40){
        digitalWrite(LCD_D6,HIGH);
    }
    if((data & 0x20) == 0x20){
        digitalWrite(LCD_D5,HIGH);
    }
    if((data & 0x10) == 0x10){
        digitalWrite(LCD_D4,HIGH);
    }

    triggle_E();
    digitalWrite(LCD_D7,LOW);
    digitalWrite(LCD_D6,LOW);
    digitalWrite(LCD_D5,LOW);
    digitalWrite(LCD_D4,LOW);
    
    if((data & 0x08) == 0x08){
        digitalWrite(LCD_D7,HIGH);
    }
    if((data & 0x04) == 0x04){
        digitalWrite(LCD_D6,HIGH);
    }
    if((data & 0x02) == 0x02){
        digitalWrite(LCD_D5,HIGH);
    }
    if((data & 0x01) == 0x01){
        digitalWrite(LCD_D4,HIGH);
    }
    triggle_E();
}

void lcd_init(){
    gpio_write(0x33,CMD); 
    delay(50);
    gpio_write(0x32,CMD);
    delay(50);
    gpio_write(0x06,CMD);
    delay(50);
    gpio_write(0x0C,CMD);
    delay(50);
    gpio_write(0x28,CMD);
    delay(50);
    gpio_write(0x01,CMD);
    delay(50);
}

void lcd_text(String ltext,int line){
    gpio_write(line,CMD);
    int tsize=ltext.length();
    for(int size=0;size<16;size++){
        if(size>tsize-1){
            gpio_write(0x20,CHR);
        }
        else{
           gpio_write(int(ltext[size]),CHR);
        }
    }
}

void setup() {
    Serial.begin(115200);
    init();
    lcd_init();
    lcd_text("RaspberryPI PICO",LCD_line1);
    lcd_text("1602a WIFI Test",LCD_line2);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    lcd_text("WIFI connecting",LCD_line1);
    lcd_text(" ",LCD_line2);
    Serial.print("RaspberryPI pico WH");
    digitalWrite(LED_BUILTIN, LOW);
    WiFi.begin(ssid,password);
  
    while (WiFi.status() != WL_CONNECTED) {
        lcd_text("WIFI connecting",LCD_line1);
        lcd_text(" ",LCD_line2);
        WiFi.begin(ssid,password);
        delay(5000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Time:");
    Serial.println(currentTime);
    Serial.print("WiFi connected at IP Address ");
    Serial.println(WiFi.localIP());
    String IP = WiFi.localIP().toString().c_str();

    lcd_text("WIFI connected",LCD_line1);
    IP1.concat(IP);
    lcd_text(IP1,LCD_line2);

    server.begin();
}

void loop(){
    WiFiClient client = server.available(); 
    digitalWrite(LED_BUILTIN, LOW);
    if(WiFi.status() != WL_CONNECTED) {
        digitalWrite(6, HIGH);
    }
    else{
        digitalWrite(6, LOW);
    }
    
    if(digitalRead(BOTTEN1)==1 && counter==0){
        digitalWrite(LED_BUILTIN, HIGH);
        lcd_text(IP1,LCD_line1);
        lcd_text(" ",LCD_line2);
        counter++;
    }
    else if(digitalRead(BOTTEN1)==1){
        digitalWrite(LED_BUILTIN, HIGH);
        counter++;
    }
    else if(digitalRead(BOTTEN1)==0){
        digitalWrite(LED_BUILTIN, LOW);
        counter=0;
    }
    
    if (counter==500){
        lcd_text(" ",LCD_line1);
        lcd_text(" ",LCD_line2);
    } 
    if (counter==4000){
        lcd_text("by KULIB 23/12/1",LCD_line1);
        lcd_text("code in arduino",LCD_line2);
    } 
    if (client){                       
        currentTime = millis();
        previousTime = currentTime;
        Serial.print("Time:");
        Serial.println(currentTime);
        Serial.println("New Client.");
        String currentLine = "";
        while (client.connected()&&currentTime-previousTime<=timeoutTime){
            currentTime = millis();
            if (client.available()){
                char c = client.read();
                //Serial.print(c);
                
                header += c;
                if (c == '\n'){          
                    if (currentLine.length() == 0) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();
      
                        // Display the HTML web page
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");
            
                        // CSS to style the on/off buttons
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".inputbox{ width:300px; height:35px; text-align:left;line-height:30px; font-size: 20px;}");
                        client.println("</style></head>");
      
                        client.println("<h1>Raspberry Pico WH 1602A display Control</h1>");
                        client.println("<form method=\"get\" action=\"\">");
                        client.println("<input type=\"text\" class=\"inputbox\" id=\"input\" name=\"inputbox\" placeholder=\"input something\" maxlength=\"32\">");
                        client.println("<p><input type=\"submit\" class=\"button\"/></p>");
   
                        client.println("</form></body></html>");
      
                        client.println();
                        
                        break;
                    } 
                    else{
                        Serial.println(currentLine);
                        if(currentLine.indexOf("GET /?inputbox=") >= 0){
                            digitalWrite(6, HIGH);
                            Serial.println("FIND!");
                            tail = currentLine.indexOf(" HTTP/1.1");
                            input = currentLine.substring(15,tail);
                        }
                        currentLine = "";
                    }
                } 
                else if (c != '\r') {
                    currentLine += c;
                }
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.print("input is ");
        Serial.print(input);
        Serial.println("");
        
        for(int i=0;i<32;i++){
            temp[i]=0x0;
        }
        for(int i=0;i<32;i++){
            temp[i]=input[i];
        }
        for(int i=0;i<16;i++){
            if(temp[i]!=0x0 && temp[i]!='+')
                co[i]=temp[i];
            else
                co[i]=0X20;
            }
        lcd_text(co,LCD_line1);
        if(temp[16]==0x0)
            lcd_text(" ",LCD_line2);
        else{
            for(int i=16;i<32;i++){
                if(temp[i]!=0x0 && temp[i]!='+')
                    co2[i-16]=temp[i];
                else
                    co2[i-16]=0X20;
            }
            lcd_text(co2,LCD_line2);
       }
       delay(100);
       digitalWrite(6, LOW  );
    }
}
