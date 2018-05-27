/*

接口定义：
	进红外检1测接口--》port c【17】
	出红外检2测接口--》port c【16】      ？？？？？？
	放行信号接收接口  --》port A【16】
	出信号产生接口--》port A【17】？？？？？？
	舵机驱动板接口--》iic0--》port c【18、19】

功能描述:
	车辆进入：红外检测传感器1检测到车辆-》输出一个高电平脉冲，port c【17】检测到这个脉冲后。
	启动舵机1，舵机抬杆让车辆进入。port c【17】同时连接到树莓派1，树莓派检测到高电平后启动拍照。

	车辆放行：红外检测传感器2检测到车辆-》输出一个高电平脉冲，与之连接的树莓派1检测到高电平后启
	动车辆识别，同时检测用户是否支付，如果支付完成则树莓派向port A【16】发送一个高电平脉冲，当
	开发板检测到检测到这个脉冲后。启动舵机2抬杆放行车辆。

问题：
	红外检测2貌似用不到，可以省去。
	
*/
#include "embARC.h"
#include "embARC_debug.h"
#include "iic.h"					//引入iic头文件

static volatile int t0 = 0;
static DEV_GPIO_PTR port_led;		//led显示端口
static DEV_GPIO_PTR port_btn;
static DEV_GPIO_PTR port_irdetect;	//红外检测端口
static DEV_GPIO_PTR	port_payed;  	//放行信号接收端口


#define BUTTON_MASK	0x7

#define IR_Detcet_1_PinNum 17
#define IR_Detcet_2_PinNum 16

#define SERVO_1_IIC_ChannelNum 0
#define SERVO_2_IIC_ChannelNum 1

#define SERVO_180_Degree_LowLevel_PulseLength 0
#define SERVO_0_Degree_LowLevel_PulseLength 0

#define SERVO_180_Degree_HighLevel_PulseLength 375
#define SERVO_0_Degree_HighLevel_PulseLength 150

#define PAYED_Signal_pinPort DW_GPIO_PORT_A
#define PAYED_Signal_pinNum 16


uint8_t  pinLevelChangeDetect(uint8_t pinNum)
{
	uint32_t data;
	uint8_t out;
	port_irdetect->gpio_read(&data,(1<<pinNum));  //17pin
	//EMBARC_PRINTF("DIP switch :0x%x\r\n", data);
	if(data == (1<<pinNum))
		{
		EMBARC_PRINTF("###ARC###:pin%d changed to High!\r\n",pinNum);
		out = 1;
	}
	else
		out = 0;
	return out;
}



/**
 * \brief	gpio key1 interrupt service routine.
 * \details	Output 'key 1' in the shell when press key 1.
 * \param[in]	*ptr	interrupt handler param
 */
static void gpio_key1_isr(void *ptr)
{
	EMBARC_PRINTF("key 1: L\r\n");
}

/**
 * \brief	gpio key2 interrupt service routine.
 * \details	Output 'key 2' in the shell when press key 2.
 * \param[in]	*ptr	interrupt handler param
 */
static void gpio_key2_isr(void *ptr)
{
	EMBARC_PRINTF("key 2: R\r\n");
}

/**
 * \brief	gpio key3 interrupt service routine.
 * \details	Output 'key 3' in the shell when press key 3.
 * \param[in]	*ptr	interrupt handler param
 */
static void gpio_key3_isr(void *ptr)
{
	EMBARC_PRINTF("key 3: A\r\n");
}

/**
 * \brief	call gpio API and configure interrupt
 */
int main(void)
{
	int32_t ercd = 0;
	DEV_GPIO_BIT_ISR bit_isr;
	DEV_GPIO_INT_CFG int_cfg;

	cpu_lock();
	board_init(); /* board init */

	port_irdetect = gpio_get_dev(DW_GPIO_PORT_C);
	port_irdetect->gpio_open(0x0);
	port_led = gpio_get_dev(DW_GPIO_PORT_B);
	port_led->gpio_open(0x1ff);
	port_payed = gpio_get_dev(PAYED_Signal_pinPort);
	port_payed->gpio_open((1<<17)|0x0); //gpioA17 was selected as signal out pin,16 was served as input

	port_btn = gpio_get_dev(0xFF);
	if (port_btn != NULL) {
		EMBARC_PRINTF("gpio_get_dev function error\r\n");
	} else {
		EMBARC_PRINTF("gpio_get_dev function ok\r\n");
	}

	port_btn = gpio_get_dev(DW_GPIO_PORT_A);

	while (port_btn->gpio_close() != E_OK);  /* close gpio device which opened before */
	if (port_btn->gpio_open(0x7) != E_OK) {
		EMBARC_PRINTF("gpio_open function error 1\r\n");
	} else {
		EMBARC_PRINTF("gpio_open function ok 1\r\n");
	}

	ercd = port_btn->gpio_open(0x0);
	if (ercd != E_OPNED) {
		EMBARC_PRINTF("gpio_open function error 2\r\n");
	} else {
		EMBARC_PRINTF("gpio_open function ok 2\r\n");
	}
	port_btn->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)0x7);

	int_cfg.int_bit_mask = BUTTON_MASK;
	int_cfg.int_bit_type = GPIO_INT_BITS_EDGE_TRIG(BUTTON_MASK);
	int_cfg.int_bit_polarity = GPIO_INT_BITS_POL_FALL_EDGE(BUTTON_MASK);
	int_cfg.int_bit_debounce = GPIO_INT_BITS_ENA_DEBOUNCE(BUTTON_MASK);
	port_btn->gpio_control(GPIO_CMD_SET_BIT_INT_CFG, &int_cfg);
	bit_isr.int_bit_ofs = 0;
	bit_isr.int_bit_handler = gpio_key1_isr;
	port_btn->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&bit_isr));
	bit_isr.int_bit_ofs = 1;
	bit_isr.int_bit_handler = gpio_key2_isr;
	port_btn->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&bit_isr));
	bit_isr.int_bit_ofs = 2;
	bit_isr.int_bit_handler = gpio_key3_isr;
	port_btn->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&bit_isr));

	port_btn->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)(0x7));



	//user user program
	iic0_init();

	//timer0 init : 1-ms timer 
	emsk_timer_init();
	
	cpu_unlock(); /* unlock system */

	uint32_t lastmillis = millis();
	uint32_t lastmillis_Outsignal_HighPulseLength = millis(); 
	uint32_t lastmillis_openGate_time = millis();
		
	EMBARC_PRINTF("timer0 :%d\r\n", lastmillis);
	uint8_t led_index = 0;
	
	while (1){
		
		uint32_t data;
		uint8_t out;
		
		if(pinLevelChangeDetect(IR_Detcet_1_PinNum)){
			setPWM(SERVO_1_IIC_ChannelNum, SERVO_180_Degree_LowLevel_PulseLength, SERVO_180_Degree_HighLevel_PulseLength);
		}
		else{
			setPWM(SERVO_1_IIC_ChannelNum, SERVO_0_Degree_LowLevel_PulseLength, SERVO_0_Degree_HighLevel_PulseLength);
		}


		if(pinLevelChangeDetect(IR_Detcet_2_PinNum)){
			port_payed->gpio_write(1,(1<<17));
			if(millis() - lastmillis_Outsignal_HighPulseLength>1000){
				lastmillis_Outsignal_HighPulseLength = millis();
				port_payed->gpio_write(0,(1<<17));
			}
		}
		if(pinLevelChangeDetect(PAYED_Signal_pinNum)){
			setPWM(SERVO_2_IIC_ChannelNum, SERVO_180_Degree_LowLevel_PulseLength, SERVO_180_Degree_HighLevel_PulseLength);
			lastmillis_openGate_time  = millis();
		}
		if(millis() -lastmillis_openGate_time > 1000){
			lastmillis_openGate_time = millis();
			setPWM(SERVO_2_IIC_ChannelNum, SERVO_0_Degree_LowLevel_PulseLength, SERVO_0_Degree_HighLevel_PulseLength);
		}

		if (millis() - lastmillis > 1000){
			EMBARC_PRINTF("+1s....\r\n");
			lastmillis = millis();
			port_led->gpio_write(~(1 << (led_index % 8)), 0x1FF);
			//port_led->gpio_read(&data, 0x1FF);
			led_index ++;
		}	

		
	}
	return E_SYS;
}

/** @} */