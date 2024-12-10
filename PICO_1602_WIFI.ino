#include <WiFi.h>
//以下是引脚定义
#define LCD_RS 2                    //数据/指令切换引脚
#define LCD_E 3                     //触发引脚
#define LCD_D4 11                   //四位数据引脚
#define LCD_D5 10
#define LCD_D6 9
#define LCD_D7 8
#define LED_LIGHT 6                 //外接LED提示灯
#define BLA 4                       //背光PWM控制

#define BOTTEN 16                   //按钮引脚

//以下是命令定义
#define CMD 0                       //命令为0
#define CHR 1                       //数据为1
#define char_per_line 16            //每行16个字符
#define LCD_line1 0x80              //第一行地址
#define LCD_line2 0xc0              //第二行地址

const char* ssid = "";              //此处填入你的WiFi SSID
const char* password = "";          //此处填入你的WiFi密码
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;      //连接超时时间2S
String header;
String input = "";
String co="123456789ABCDEF1";
String co2="123456789ABCDEF1";
String IP1="IP:";
char temp[35];
int tail = 0;
int counter=0;
WiFiServer server(80);              //80端口设置服务器
using namespace std;

void init(){                        //初始化引脚
    pinMode(LCD_RS,GPIO_OUT);
    pinMode(LCD_E,GPIO_OUT);
    pinMode(LCD_D4,OUTPUT);
    pinMode(LCD_D5,GPIO_OUT);
    pinMode(LCD_D6,GPIO_OUT);
    pinMode(LCD_D7,GPIO_OUT);
    pinMode(LED_LIGHT,GPIO_OUT);
    pinMode(BOTTEN,INPUT);

    analogWrite(BLA, 255);
}

void triggle_E(){                     //触发E引脚
    delay(1);
    digitalWrite(LCD_E,HIGH);
    delay(1);
    digitalWrite(LCD_E,LOW);
    delay(1);
}

void gpio_write(int data,int mode){   //写入一个数据或指令

    //设定模式(指令/字符)
    if(mode==0){
        digitalWrite(LCD_RS,LOW);
    }
    else if(mode==1){
        digitalWrite(LCD_RS,HIGH);
    }

    //前四位
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

    //后四位
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

void lcd_init(){                        //初始化1602函数
    gpio_write(0x33,CMD);               //
    delay(50);
    gpio_write(0x32,CMD);               //
    delay(50);
    gpio_write(0x06,CMD);               //设置向右写入字符，设置屏幕内容不滚动
    delay(50);
    gpio_write(0x0C,CMD);               //开启屏幕显示，关闭光标显示，关闭光标闪烁
    delay(50);
    gpio_write(0x28,CMD);               //设定数据总线为四位，显示2行字符，使用5*8字符点阵
    delay(50);
    gpio_write(0x01,CMD);               //清屏
    delay(50);
}

void lcd_text(String ltext,int line){   //显示函数
    gpio_write(line,CMD);               //命令写入:行数
    int tsize=ltext.length();
    for(int size=0;size<16;size++){     //逐字写入
        if(size>tsize-1){
            gpio_write(0x20,CHR);       //若字符串大小小于16，则填充空格
        }
        else{
            gpio_write(int(ltext[size]),CHR); //转换成RAW编码后写入
        }
    }
}

void setup() {
    Serial.begin(115200);
    init();
    lcd_init();
    lcd_text("RaspberryPI PICO",LCD_line1);     //欢迎消息
    lcd_text("1602a WIFI Test",LCD_line2);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);            //点灯
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);

    lcd_text("WIFI connecting",LCD_line1);
    lcd_text(" ",LCD_line2);
    Serial.print("RaspberryPI pico WH");
    WiFi.begin(ssid,password);                  //在屏幕上显示状态
  
    while (WiFi.status() != WL_CONNECTED) {     //连接WiFi
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
    lcd_text(IP1,LCD_line2);                    //在屏幕上显示ip

    server.begin();
}

String get_kana_ascii(int utf8){      //假名utf转换成RAW字符函数,查表
  String raw_kana="  ";               //返回用变量
  if(utf8==354 || utf8==450)  //あ
    raw_kana[0]=0Xb1;
  if(utf8==356 || utf8==452)  //い
    raw_kana[0]=0Xb2;
  if(utf8==358 || utf8==454)  //う
    raw_kana[0]=0Xb3;
  if(utf8==360 || utf8==456)  //え
    raw_kana[0]=0Xb4;
  if(utf8==362 || utf8==458)  //お
    raw_kana[0]=0Xb5;

  if(utf8==353 || utf8==449)  //ぁ
    raw_kana[0]=0Xa7;
  if(utf8==355 || utf8==451)  //ぃ
    raw_kana[0]=0Xa8;
  if(utf8==357 || utf8==453)  //ぅ
    raw_kana[0]=0Xa9;
  if(utf8==359 || utf8==455)  //ぇ
    raw_kana[0]=0Xaa;
  if(utf8==361 || utf8==457)  //ぉ
    raw_kana[0]=0Xab;

  if(utf8==363 || utf8==459)  //か
    raw_kana[0]=0Xb6;
  if(utf8==365 || utf8==461)  //き
    raw_kana[0]=0Xb7;
  if(utf8==367 || utf8==463)  //く
    raw_kana[0]=0Xb8;
  if(utf8==369 || utf8==465)  //け
    raw_kana[0]=0Xb9;
  if(utf8==371 || utf8==467)  //こ
    raw_kana[0]=0Xba;

  if(utf8==364 || utf8==460)  //が
    {raw_kana[0]=0Xb6;raw_kana[1]=0Xde;}
  if(utf8==366 || utf8==462)  //ぎ
    {raw_kana[0]=0Xb7;raw_kana[1]=0Xde;}
  if(utf8==368 || utf8==464)  //ぐ
    {raw_kana[0]=0Xb8;raw_kana[1]=0Xde;}
  if(utf8==370 || utf8==466)  //げ
    {raw_kana[0]=0Xb9;raw_kana[1]=0Xde;}
  if(utf8==372 || utf8==468)  //ご
    {raw_kana[0]=0Xba;raw_kana[1]=0Xde;}

  if(utf8==373 || utf8==469)  //さ
    raw_kana[0]=0Xbb;
  if(utf8==375 || utf8==471)  //し
    raw_kana[0]=0Xbc;
  if(utf8==377 || utf8==473)  //す
    raw_kana[0]=0Xbd;
  if(utf8==379 || utf8==475)  //せ
    raw_kana[0]=0Xbe;
  if(utf8==381 || utf8==477)  //そ
    raw_kana[0]=0Xbf;

  if(utf8==374 || utf8==470)  //ざ
    {raw_kana[0]=0Xbb;raw_kana[1]=0Xde;}
  if(utf8==376 || utf8==472)  //じ
    {raw_kana[0]=0Xbc;raw_kana[1]=0Xde;}
  if(utf8==378 || utf8==474)  //ず
    {raw_kana[0]=0Xbd;raw_kana[1]=0Xde;}
  if(utf8==380 || utf8==476)  //ぜ
    {raw_kana[0]=0Xbe;raw_kana[1]=0Xde;}
  if(utf8==382 || utf8==478)  //ぞ
    {raw_kana[0]=0Xbf;raw_kana[1]=0Xde;}

  if(utf8==383 || utf8==479)  //た
    raw_kana[0]=0Xc0;
  if(utf8==385 || utf8==481)  //ち
    raw_kana[0]=0Xc1;
  if(utf8==388 || utf8==484)  //つ
    raw_kana[0]=0Xc2;
  if(utf8==390 || utf8==486)  //て
    raw_kana[0]=0Xc3;
  if(utf8==392 || utf8==488)  //と
    raw_kana[0]=0Xc4;

  if(utf8==384 || utf8==480)  //だ
    {raw_kana[0]=0Xc0;raw_kana[1]=0Xde;}
  if(utf8==386 || utf8==482)  //ぢ
    {raw_kana[0]=0Xc1;raw_kana[1]=0Xde;}
  if(utf8==389 || utf8==485)  //づ
    {raw_kana[0]=0Xc2;raw_kana[1]=0Xde;}
  if(utf8==391 || utf8==487)  //で
    {raw_kana[0]=0Xc3;raw_kana[1]=0Xde;}
  if(utf8==393 || utf8==489)  //ど
    {raw_kana[0]=0Xc4;raw_kana[1]=0Xde;}

  if(utf8==387 || utf8==483)  //っ
    raw_kana[0]=0Xaf;

  if(utf8==394 || utf8==490)  //な
    raw_kana[0]=0Xc5;
  if(utf8==395 || utf8==491)  //に
    raw_kana[0]=0Xc6;
  if(utf8==396 || utf8==492)  //ぬ
    raw_kana[0]=0Xc7;
  if(utf8==397 || utf8==493)  //ね
    raw_kana[0]=0Xc8;
  if(utf8==398 || utf8==494)  //の
    raw_kana[0]=0Xc9;

  if(utf8==399 || utf8==495)  //は
    raw_kana[0]=0Xca;
  if(utf8==402 || utf8==498)  //ひ
    raw_kana[0]=0Xcb;
  if(utf8==405 || utf8==501)  //ふ
    raw_kana[0]=0Xcc;
  if(utf8==408 || utf8==504)  //へ
    raw_kana[0]=0Xcd;
  if(utf8==411 || utf8==507)  //ほ
    raw_kana[0]=0Xce;

  if(utf8==400 || utf8==496)  //ば
    {raw_kana[0]=0Xca;raw_kana[1]=0Xde;}
  if(utf8==403 || utf8==499)  //び
    {raw_kana[0]=0Xcb;raw_kana[1]=0Xde;}
  if(utf8==406 || utf8==502)  //ぶ
    {raw_kana[0]=0Xcc;raw_kana[1]=0Xde;}
  if(utf8==409 || utf8==505)  //べ
    {raw_kana[0]=0Xcd;raw_kana[1]=0Xde;}
  if(utf8==412 || utf8==508)  //ぼ
    {raw_kana[0]=0Xce;raw_kana[1]=0Xde;}

  if(utf8==401 || utf8==497)  //ぱ
    {raw_kana[0]=0Xca;raw_kana[1]=0Xdf;}
  if(utf8==404 || utf8==500)  //ぴ
    {raw_kana[0]=0Xcb;;raw_kana[1]=0Xdf;}
  if(utf8==407 || utf8==503)  //ぷ
    {raw_kana[0]=0Xcc;raw_kana[1]=0Xdf;}
  if(utf8==410 || utf8==506)  //ぺ
    {raw_kana[0]=0Xcd;raw_kana[1]=0Xdf;}
  if(utf8==413 || utf8==509)  //ぽ
    {raw_kana[0]=0Xce;raw_kana[1]=0Xdf;}

  if(utf8==414 || utf8==510)  //ま
    raw_kana[0]=0Xcf;
  if(utf8==415 || utf8==511)  //み
    raw_kana[0]=0Xd0;
  if(utf8==416 || utf8==512)  //む
    raw_kana[0]=0Xd1;
  if(utf8==417 || utf8==513)  //め
    raw_kana[0]=0Xd2;
  if(utf8==418 || utf8==514)  //も
    raw_kana[0]=0Xd3;

  if(utf8==420 || utf8==516)  //や
    raw_kana[0]=0Xd4;
  if(utf8==422 || utf8==518)  //ゆ
    raw_kana[0]=0Xd5;
  if(utf8==424 || utf8==520)  //よ
    raw_kana[0]=0Xd6;

  if(utf8==419 || utf8==515)  //ゃ
    raw_kana[0]=0Xac;
  if(utf8==421 || utf8==517)  //ゅ
    raw_kana[0]=0Xad;
  if(utf8==423 || utf8==519)  //ょ
    raw_kana[0]=0Xae;

  if(utf8==425 || utf8==521)  //ら
    raw_kana[0]=0Xd7;
  if(utf8==426 || utf8==522)  //り
    raw_kana[0]=0Xd8;
  if(utf8==427 || utf8==523)  //る
    raw_kana[0]=0Xd9;
  if(utf8==428 || utf8==524)  //れ
    raw_kana[0]=0Xda;
  if(utf8==429 || utf8==525)  //ろ
    raw_kana[0]=0Xdb;

  if(utf8==431 || utf8==527)  //わ
    raw_kana[0]=0Xdc;
  if(utf8==434 || utf8==530)  //を
    raw_kana[0]=0Xa6;
  if(utf8==435 || utf8==531)  //ん
    raw_kana[0]=0Xdd;

  if(utf8==540)
    raw_kana[0]=0Xb0;   //ー
  if(utf8==290)
    raw_kana[0]=0Xa1    //。
  if(utf8==300)
    raw_kana[0]=0Xa2;   //「
  if(utf8==301)
    raw_kana[0]=0Xa3;   //」
  if(utf8==289)
    raw_kana[0]=0Xa4;   //、
  if(utf8==539)
    raw_kana[0]=0Xa5;   //・

  return raw_kana;
}

void loop(){
    WiFiClient client = server.accept();        //初始化客户端
    digitalWrite(LED_BUILTIN, LOW);
    if(WiFi.status() != WL_CONNECTED) {         //未连接WiFi则点灯
        digitalWrite(LED_LIGHT, HIGH);
    }
    else{
        digitalWrite(LED_LIGHT, LOW);
    }
    
    if(digitalRead(BOTTEN)==1 && counter==0){   //短按按钮显示IP
        digitalWrite(LED_BUILTIN, HIGH);        //按住是点灯
        lcd_text(ssid,LCD_line1);
        lcd_text(IP1,LCD_line2);
        counter++;
    }
    else if(digitalRead(BOTTEN)==1){            //长按开始计时
        digitalWrite(LED_BUILTIN, HIGH);
        if(counter%50==0)
          Serial.println(counter);

        counter++;
    }


    else if(digitalRead(BOTTEN)==0){            //按键松开清零计时器
        digitalWrite(LED_BUILTIN, LOW);
        counter=0;
    }
    
    if (counter==300){                           //300时刻清屏
        lcd_text(" ",LCD_line1);
        lcd_text(" ",LCD_line2);
    } 
    if (counter==700){                           //调整背光亮度
        lcd_text("Brightness",LCD_line1);
        lcd_text("0%",LCD_line2);
        analogWrite(BLA, 0);
    }
    if (counter==1000){
        lcd_text("Brightness",LCD_line1);
        lcd_text("25%",LCD_line2);
        analogWrite(BLA, 64);
    }
    if (counter==1300){
        lcd_text("Brightness",LCD_line1);
        lcd_text("50%",LCD_line2);
        analogWrite(BLA, 128);
    }
    if (counter==1600){
        lcd_text("Brightness",LCD_line1);
        lcd_text("75%",LCD_line2);
        analogWrite(BLA, 191);
    }
    if (counter==1900){
        lcd_text("Brightness",LCD_line1);
        lcd_text("100%",LCD_line2);
        analogWrite(BLA, 255);
    }


    if (counter==4000){                         //程序消息
        lcd_text("by KULIB 24/12/10",LCD_line1);
        lcd_text("code in arduino",LCD_line2);
    }

    if (client){                       
        currentTime = millis();
        previousTime = currentTime;
        Serial.print("Time:");
        Serial.println(currentTime);
        Serial.println("New Client.");
        String currentLine = "";
        while (client.connected() && currentTime-previousTime<=timeoutTime){
            currentTime = millis();
            if (client.available()){
                char c = client.read();
                
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
            
                        // CSS style
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
                        if(currentLine.indexOf("GET /?inputbox=") >= 0){    //获取输入
                            digitalWrite(LED_LIGHT, HIGH);
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
        Serial.print("input is {");
        Serial.print(input);
        Serial.println("}");

        //--------------------测试用代码-------------------
        // String teststr="                                ";
        // int s=0;
        // for(int i=0;i<=256;i++){
        //   teststr[s]=i;
        //   s++;
        //   if(s%32==0){
        //     s=0;
        //     //Serial.println(teststr);
        //   }
        // }
        //--------------------测试用代码-------------------

        int index0;                       //搜索用坐标
        String input_index0="";           //假名UTF8前子串
        String input_index1="";           //假名UTF8后子串
        index0=input.indexOf("%26%2312"); //假名UTF8特征前缀

        while(input.indexOf("%26%2312")!=-1){   //当输入存在假名时
          index0=input.indexOf("%26%2312");
          int kana_utf8=input.substring(index0+8,index0+11).toInt(); //三位假名utf8编码
          String raw_kana=get_kana_ascii(kana_utf8);                 //假名RAW编码

          Serial.print("utf8 kana:"); 
          Serial.print(kana_utf8);                  //调试   输出当前假名编码值
          Serial.print("  RAW kana:"); 
          Serial.println(raw_kana);                 //调试   输出RAW假名

          input_index0=input.substring(0,index0);
          input_index1=input.substring(index0+14,input.length());

          Serial.print("index0:");
          Serial.print(input_index0);
          Serial.print("  index1:");
          Serial.print(input_index1);
          
          if(input_index0!=0){      //防止index0=0时多一个空格
            if(raw_kana[1]==' ')    //判断是否浊音/半浊音
              input=input_index0+raw_kana[0]+input_index1;  //将字符串假名utf替换为RAW编码
            else
              input=input_index0+raw_kana+input_index1;
          }
          else{
            if(raw_kana[1]==' ')
              input=raw_kana[0]+input_index1;
            else
              input=raw_kana+input_index1;
          }

          Serial.print("      input:");
          Serial.println(input);
        }
        
        for(int i=0;i<32;i++){      //初始化临时数组
            temp[i]=0x0;
        }
        for(int i=0;i<32;i++){      //读入原始字符串
            temp[i]=input[i];
        }

        for(int i=0;i<16;i++){
            if(temp[i]!=0x0 && temp[i]!='+')  //将浏览器空格转换成的"+"转换回空格
                co[i]=temp[i];
            else
                co[i]=0X20;
            }
        lcd_text(co,LCD_line1);   //显示第一行
        
        Serial.print("line1 RAW:");
        Serial.println(co);

        if(temp[16]==0x0)         //若第二行内容为空则直接显示
            lcd_text(" ",LCD_line2);
        else{
            for(int i=16;i<32;i++){
                if(temp[i]!=0x0 && temp[i]!='+')  //替换第二行的“+”
                    co2[i-16]=temp[i];
                else
                    co2[i-16]=0X20;
            }
            lcd_text(co2,LCD_line2);    //显示第二行

            Serial.print("line2 RAW:");
            Serial.println(co2);
       }
       delay(100);
       digitalWrite(LED_LIGHT, LOW);
    }
}
