#pragma once

class CCRC16
{

private:

	CCRC16(void)
	{
	}

	~CCRC16(void)
	{
	}

public:

	static unsigned short CalcCRC16Words(unsigned int count, short *buffer);

	static unsigned short CalcCRC16Bytes(unsigned int count, char *buffer);
};