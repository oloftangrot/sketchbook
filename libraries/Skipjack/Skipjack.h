/*
	Skipjack.h - Library for Skipjack encryption and decryption
	Ported to the Arduino platform 2014 by Olof Tångrot
   
	Original version taken from the AVR-Crypto-Lib
	(http://www.das-labor.org/wiki/AVR-Crypto-Lib)
	Copyright (C) 2006-2010  Daniel Otte (daniel.otte@rub.de)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef Skipjack_h
#define Skipjack_h

#include "Arduino.h"

class Skipjack {
public:
	void encrypt(void * data , void  *  key);
	void decrypt(void * data, void  *  key);

private:
	uint16_t skipjack_sub_g(uint8_t g, uint8_t k, uint8_t *key);
	uint16_t skipjack_g(uint16_t g, uint8_t k, uint8_t *key);
	uint16_t skipjack_g_inv(uint16_t g, uint8_t k, uint8_t *key);
	void skipjack_a(uint16_t *w, uint8_t k, uint8_t *key);
	void skipjack_a_inv(uint16_t *w, uint8_t k, uint8_t *key);
	void skipjack_b(uint16_t *w, uint8_t k, uint8_t *key);
	void skipjack_b_inv(uint16_t *w, uint8_t k, uint8_t *key);

};

#endif
