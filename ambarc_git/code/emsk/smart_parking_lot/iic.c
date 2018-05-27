#include "iic.h"
#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

#define IIC_MASTER_ID		0

#define IIC_SLAVE_ADDRESS	0x34

#define SLVDEV_REG_ID		0x0
#define SLVDEV_REG_CFG		0x1
#define SLVDEV_REG_DATA		0x2
#define SLVDEV_REG_RESET	0x3

DEV_IIC *iic_master;

/**
 * \brief	IIC device initialization
 * \param[in]	slv_addr	slave address
 */
void slvdev_init(uint32_t slv_addr)
{
	iic_master = iic_get_dev(IIC_MASTER_ID); /* get IIC device */

	iic_master->iic_open(DEV_MASTER_MODE, IIC_SPEED_FAST); /* open IIC device in master mode */

	iic_master->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(slv_addr)); /* Set target slave device address */
}
int32_t slvdev_write_reg(uint8_t regaddr, uint8_t regvalue)
{
	uint8_t buf[2];

	iic_master->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP)); /* Set next condition for following transmit operation */
	buf[0] = regaddr;
	buf[1] = regvalue;
	return iic_master->iic_write(buf, 2);
}
int32_t slvdev_read_reg(uint8_t regaddr, uint8_t *data)
{
	uint8_t buf[1];
	int32_t ercd;

	iic_master->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART)); /* Set next condition for following operation */
	buf[0] = regaddr;
	iic_master->iic_write(buf, 1); /* Write data */
	iic_master->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP)); /* Set next condition for following operation */
	ercd = iic_master->iic_read(buf, 1); /* Read data */
	*data = buf[0];

	return ercd;
}
void iic0_init()
{
	EMBARC_PRINTF("iic 0 initing!\r\n");
	slvdev_init(0x40);
    {	//
		double T = 20000;	//
		uint8_t prescale;
		double osc_clock = 25000000;
		uint8_t oldmode, newmode;
		T /= 0.915;	//
		T /= 1000000;	//
		prescale = (uint8_t)(osc_clock/4096*T - 1);
		slvdev_read_reg( PCA9685_MODE1,&oldmode);
		EMBARC_PRINTF("pca9685: oldmode = %d\r\n",oldmode);
		newmode = (oldmode&0x7f) | 0x10;	//
		slvdev_write_reg(PCA9685_MODE1, newmode);
		
		slvdev_read_reg( PCA9685_MODE1,&oldmode);
		EMBARC_PRINTF("pca9685: oldmode = %d\r\n",oldmode);
		
		slvdev_write_reg( PCA9685_PRESCALE, prescale);
		EMBARC_PRINTF("pca9685: prescale = %d\r\n",prescale);
		oldmode &= 0xef;	//
		slvdev_write_reg( PCA9685_MODE1, oldmode);
		uint32_t ms = 0xffffffff;
		while(ms--){
			for (uint32_t j = 0xffff;j>0;j--);
			}
		slvdev_write_reg( PCA9685_MODE1, oldmode | 0xa1);
    }
}
void setPWM(uint8_t num, uint8_t on, uint16_t off)
{
if (off > 601 || off < 149)
{
	EMBARC_PRINTF("pulse out of range:150-600\r\n");
	//return ;
}
slvdev_write_reg( LED0_ON_L+4*num,on);
slvdev_write_reg( LED0_ON_H+4*num,on>>8);
slvdev_write_reg( LED0_OFF_L+4*num,off);
slvdev_write_reg( LED0_OFF_H+4*num,off>>8);
}