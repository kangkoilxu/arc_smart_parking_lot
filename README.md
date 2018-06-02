# arc_smart_parking_lot
arc contest : smart_smart_parking_lot

	

项目名称	：	arc智能停车场管理系统
负责人		：  徐康
单位		：  西安电子科技大学


视频地址：http://v.youku.com/v_show/id_XMzYzMTg2MjgwOA==.html?spm=a2h3j.8428770.3416059.1

注意：  ！！！编译的时候一定要用我的arc_smart_parking_lot/ambarc_git/board/emsk/common/文件，因为我在emsk_timer.c文件实现了一个 millis()函数，用来返回系统时间，main.c调用了这个函数。！！！

开发环境：
	开发板版本：emsk 1.1
	固件版本：arcem4
	工具链：GNU
	debug：JTAG-usb

接口定义：
	进红外检1测接口--》port c【17】
	出红外检2测接口--》port c【16】      
	放行信号接收接口  --》port A【16】
	出信号产生接口--》port A【17】
	舵机驱动板接口--》iic0--》port c【18、19】

功能描述:
	车辆进入：红外检测传感器1检测到车辆-》输出一个高电平脉冲，port c【17】检测到这个脉冲后。
	启动舵机1，舵机抬杆让车辆进入。port c【17】同时连接到树莓派1，树莓派检测到高电平后启动拍照。

	车辆放行：红外检测传感器2检测到车辆-》输出一个高电平脉冲，与之连接的树莓派1检测到高电平后启
	动车辆识别，同时检测用户是否支付，如果支付完成则树莓派向port A【16】发送一个高电平脉冲，当
	开发板检测到检测到这个脉冲后。启动舵机2抬杆放行车辆。

舵机控制：
	由于arc开发没有pwm接口，无法直接控制舵机，所有通过i2c接口扩展了一个pwm接口。


android 源码地址：
	  
	https://github.com/freeDevelope/Android_Things_Iot_HyperLPR4Android.git
	https://github.com/freeDevelope/ParkingLot_v2.git
	
	
