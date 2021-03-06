/*
 * config.cpp
 *
 *  Created on: 2015/3/31
 *      Author: Leo
 */

#include "config.h"
#include "main.h"

Config::Config(I2C *i2c) :
		_eeprom(i2c) {

}

void Config::test() {
	pc.printf("class Config : EEPROM test");
	uint16_t d;
	for (d = 0; d < 0xffff; d++) {
		write(0x00, d, 2);
		uint16_t r = read(0x00, 2);
		pc.printf("class Config : w 0x%x r 0x%x\r\n", d, r);
	}
}

void Config::write(uint16_t addr, uint16_t conf, uint16_t length) {
	char write[4];
	write[0] = (addr >> 8) & 0xFF;
	write[1] = addr & 0xFF;
	write[2] = conf & 0xFF;
	write[3] = (conf >> 8) & 0xFF;
	_eeprom->write(EEPROM_Address, write, 2 + length);
	wait_ms(100);
}

uint16_t Config::read(uint16_t addr, uint16_t length) {
	char write[2];
	char read[2];
	write[0] = (addr >> 8) & 0xFF;
	write[1] = addr & 0xFF;
	memset(read, 0, 2);
	_eeprom->write(EEPROM_Address, write, 2, true);
	_eeprom->read(EEPROM_Address, read, length);
	return (read[1] << 8) | read[0];
}

void Config::set(CONFIG_TYPE type, uint16_t conf) {
	switch (type) {
	case TIMEZONE:
		write(type, conf, 2);
		break;
	case MOTOR_TYPE:
	case MOTOR_SPEC:
	case MOTOR_RPMS:
	case TASK:
		write(type, conf, 1);
		break;
	}
}

uint16_t Config::get(CONFIG_TYPE type) {
	switch (type) {
	case TIMEZONE:
		return read(type, 2);
		break;
	case MOTOR_TYPE:
	case MOTOR_SPEC:
	case MOTOR_RPMS:
	case TASK:
		return read(type, 1);
		break;
	}
	return NULL;
}

