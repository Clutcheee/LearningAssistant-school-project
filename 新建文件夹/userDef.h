#define voice 100	//噪声阈值
#define micPin A0	//麦克风引脚
#define buzzerPin 10	//蜂鸣器引脚
#define keyPin 8	//按键引脚


long timer;		//按键定时器
int numNoise=0;		//记录噪音频率
boolean isAlaram=false;		//标记时间是否超时
int i = 200;		//蜂鸣器频率初始值
unsigned long Time,SetTime=2700;//时间变量和设定的计时时间
