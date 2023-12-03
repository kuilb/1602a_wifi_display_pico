# 1602a_wifi_display_pico
使用树莓派pico WH + 1602a显示屏  
使用arduino IDE编写

### 注意事项
使用前请自定义WiFi ssid和密码，只支持2.4GWiFi  
(const char* ssid = "YOUR_SSID";  
const char* password = "YOUR_PASSWORD";)

请在代码头的define处修改GPIO序号  
按钮默认GPIO16，可自行修改

1602a的V0和RW接地

### 配置方法
arduino-首选项-附加开发板管理器网址填入  
https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json

前往开发板管理器下载pico_sdk  
编译烧录

### 使用方法
接入pico同一WiFi,打开pico上显示的ip即可
按键功能：短按显示ip，长按清屏