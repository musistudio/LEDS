#include <MsTimer2.h> // 引入定时器库

#define button 3
#define hall 2

int starttime;
int stoptime;
long looptime = 0;

int leds[] = {8, 9, 10, 11}; // led引脚集合
int mode = 3;                // 设置模式， 0为初始化，1为同心圆顺序， 2为同心圆逆序，3为秒针
int modeLen = 4;             // 模式长度
int btnState = 0;            // 按钮初始状态为0
int hallState = 0;           // 霍尔传感器状态
int deg = 0;                 // 度数
int numbers = 0;             // 测速圈数
int t = 0;                   // 电机转1°所需要时间
int hl = 0;
int start = 0;

// 项目初始化
void setup()
{
  Serial.begin(9600);     // 设置串口波特率
  pinMode(button, INPUT); // 设置按钮模式为输入模式
  check();                // 开机自检
  attachInterrupt(digitalPinToInterrupt(button), changeButton, CHANGE);
  attachInterrupt(digitalPinToInterrupt(hall), changeHall, CHANGE);
  MsTimer2::set(1000, changeSecond);
  MsTimer2::start();
}

void loop()
{
  select(mode);
}

// 按钮检测事件
void changeButton()
{
  if (!digitalRead(button))
  {
    // 按钮防抖
    btnState = 1;
  }
  if (btnState == 1 && digitalRead(button))
  {
    if (mode < modeLen - 1)
    {
      mode++;
    }
    else
    {
      mode = 0;
    }
    btnState = 0;
  }
}

// 自检函数
void check()
{
  // 初始化将所有引脚设置为输出模式,并进行自检
  for (int i = 0; i < 4; i++)
  {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
    delay(1000);
    digitalWrite(leds[i], HIGH);
  }
}

// 圆圈函数
// order: 1为顺序 0为逆序
void circle(int order)
{
  if (order == 0)
  {
    for (int i = 3; i >= 0; i--)
    {
      digitalWrite(leds[i], LOW);
      delay(300);
      digitalWrite(leds[i], HIGH);
    }
  }
  else
  {
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(leds[i], LOW);
      delay(300);
      digitalWrite(leds[i], HIGH);
    }
  }
}

// 模式选择函数
void select(int mode)
{
  switch (mode)
  {
  // 初始化模式，所有LED熄灭
  case 0:
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(leds[i], HIGH);
    }
    break;
  // 同心圆顺序模式
  case 1:
    circle(1);
    break;
  // 同心圆逆序模式
  case 2:
    circle(0);
    break;
  // 秒针模式
  case 3:
    second();
    break;
  default:
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(leds[i], HIGH);
    }
    break;
  }
}

// 秒针模式函数
void second()
{
  digitalWrite(leds[0], LOW); // 点亮最外围的LED

  if (!digitalRead(hall)) // 检测标志位
  {
    hallState = 1; // 进入标志位
  }
  if (hallState == 1 && digitalRead(hall))
  {
    hallState = 0;
    if (looptime)
    {
      for (int i = 0; i < deg; i++)
      {
        delayMicroseconds((looptime * 1000) / 60);
      }
      showSecond();
      delayMicroseconds((looptime * 1000) / 360);
      hiddenSecond();
    }
  }
}

void showSecond()
{
  digitalWrite(leds[2], LOW);
  digitalWrite(leds[3], LOW);
}
void hiddenSecond()
{
  digitalWrite(leds[2], HIGH);
  digitalWrite(leds[3], HIGH);
}

void changeSecond()
{
  if (mode == 3)
  {
    if (deg >= 60)
    {
      deg = 0;
    }
    else
    {
      deg += 1;
    }
  }
  else
  {
    deg = 0;
  }
}

void changeHall()
{
  if (!digitalRead(hall)) // 检测标志位
  {
    numbers++; // 圈数+1
    if (numbers >= 200)
    {
      if (!starttime)
      {
        starttime = millis();
        stoptime = 0;
      }
      else
      {
        if (starttime && !stoptime)
        {
          stoptime = millis();
          looptime = stoptime - starttime;
          t = looptime / 360;
        }
      }
    }
  }
}