#pragma once

class CBlowfish
{

private:	

#pragma pack(push)
#pragma pack(1)

	struct WordByte
	{
		unsigned char	zero;
		unsigned char	one;
		unsigned char	two;
		unsigned char	three;	
	};

	union Word
	{
		unsigned long word;
		WordByte byte;
	};

	struct DWord
	{
		Word word0;
		Word word1;
	};

#pragma pack(pop)	

public:

	CBlowfish(void)
	{
		Reset();
	}

	~CBlowfish(void)
	{
		Reset();
	}	

	void Reset()
	{
		unsigned int i,j;		

		for (i=0;i<NUM_SUBKEYS;i++)
		{
			PA[i] = m_PA_Init[i];
		}

		for (j=0;j<NUM_S_BOXES;j++)
		{
			for (i=0;i<NUM_ENTRIES;i++)
			{
				SB[j][i] = m_SB_Init[j][i];
			}
		}  
	}

	bool Set_Passwd(char * Passwd)
	{		
		unsigned int len;

		len = (int) strlen(Passwd);
		if (len == 0 || len > MAX_STRING)
			return false;		

		Reset();
		Gen_Subkeys(Passwd);		

		Passwd = NULL;
		len = 0;

		return true;
	}

	bool Set_Passwd(const BYTE* Passwd, int len)
	{				
		if (len == 0 || len > MAX_STRING)
			return false;		

		Reset();
		Gen_Subkeys(Passwd, len);		

		Passwd = NULL;
		len = 0;

		return true;
	}

	bool Encrypt(void *Ptr, unsigned int N_Bytes)
	{
		unsigned int i;
		DWord *Work;

		if (N_Bytes%8)
		{			
			return false;
		}

		N_Bytes /= 8;
		Work = (DWord *)Ptr;

		for (i=0;i<N_Bytes;i++)
		{
			BF_En(&Work->word0,&Work->word1);
			Work++;
		}

		Work = NULL;

		return true;		  
	}

	bool Decrypt(void *Ptr, unsigned int N_Bytes)
	{
		unsigned int i;
		DWord *Work;

		if (N_Bytes%8)
		{
			return false;
		}

		N_Bytes /= 8;
		Work = (DWord *)Ptr;
		for (i=0;i<N_Bytes;i++)
		{
			BF_De(&Work->word0,&Work->word1);
			Work++;
		}

		Work = NULL;

		return true;
	}

private:

	const static unsigned NUM_SUBKEYS	= 18;
	const static unsigned NUM_S_BOXES	= 4;
	const static unsigned NUM_ENTRIES	= 256;
	const static unsigned MAX_STRING	= 256;
	const static unsigned MAX_PASSWD	= 56;  // 448bits

	static const unsigned int m_PA_Init[NUM_SUBKEYS];
	static const unsigned int m_SB_Init[NUM_S_BOXES][NUM_ENTRIES];

	unsigned int PA[NUM_SUBKEYS];
	unsigned int SB[NUM_S_BOXES][NUM_ENTRIES];

	void Gen_Subkeys(const BYTE* Passwd, int len)
	{
		unsigned int i, j;
		Word Work, null0, null1;

		if (len > 0)
		{
			j = 0;

			for (i=0;i<NUM_SUBKEYS;i++)
			{
				Work.byte.zero = Passwd[(j++)%len];
				Work.byte.one = Passwd[(j++)%len];
				Work.byte.two = Passwd[(j++)%len];
				Work.byte.three = Passwd[(j++)%len];
				PA[i] ^= Work.word;
			}

			null0.word = null1.word = 0;

			for (i=0;i<NUM_SUBKEYS;i+=2)
			{
				BF_En(&null0, &null1);
				PA[i] = null0.word;
				PA[i+1] = null1.word;
			}

			for (j=0; j<NUM_S_BOXES; j++)
			{
				for (i=0; i<NUM_ENTRIES; i+=2)
				{
					BF_En(&null0,&null1);
					SB[j][i] = null0.word;
					SB[j][i+1] = null1.word;
				}
			}
		}

		Work.word = null0.word = null1.word = 0;
		Passwd = NULL;
		len = 0;
	}

	void Gen_Subkeys(char* Passwd)
	{
		Gen_Subkeys((BYTE*) Passwd, (int) strlen(Passwd));
	}

	inline void BF_En(Word *x1, Word *x2)
	{		
		Word w1=*x1, w2=*x2;

		w1.word ^= PA[0];
		w2.word ^= F(w1)^PA[1];       w1.word ^= F(w2)^PA[2];
		w2.word ^= F(w1)^PA[3];       w1.word ^= F(w2)^PA[4];
		w2.word ^= F(w1)^PA[5];       w1.word ^= F(w2)^PA[6];
		w2.word ^= F(w1)^PA[7];       w1.word ^= F(w2)^PA[8];
		w2.word ^= F(w1)^PA[9];       w1.word ^= F(w2)^PA[10];
		w2.word ^= F(w1)^PA[11];      w1.word ^= F(w2)^PA[12];
		w2.word ^= F(w1)^PA[13];      w1.word ^= F(w2)^PA[14];
		w2.word ^= F(w1)^PA[15];      w1.word ^= F(w2)^PA[16];
		w2.word ^= PA[17];

		*x1 = w2;
		*x2 = w1;
	}

	inline void BF_De(Word *x1, Word *x2)
	{
		Word w1=*x1,w2=*x2;

		w1.word ^= PA[17];
		w2.word ^= F(w1)^PA[16];      w1.word ^= F(w2)^PA[15];
		w2.word ^= F(w1)^PA[14];      w1.word ^= F(w2)^PA[13];
		w2.word ^= F(w1)^PA[12];      w1.word ^= F(w2)^PA[11];
		w2.word ^= F(w1)^PA[10];      w1.word ^= F(w2)^PA[9];
		w2.word ^= F(w1)^PA[8];       w1.word ^= F(w2)^PA[7];
		w2.word ^= F(w1)^PA[6];       w1.word ^= F(w2)^PA[5];
		w2.word ^= F(w1)^PA[4];       w1.word ^= F(w2)^PA[3];
		w2.word ^= F(w1)^PA[2];       w1.word ^= F(w2)^PA[1];
		w2.word ^= PA[0];

		*x1 = w2;
		*x2 = w1;
	}

	inline unsigned int F(const Word& x)
	{
		return (((SB[0][x.byte.zero] + SB[1][x.byte.one]) ^ SB[2][x.byte.two]) + SB[3][x.byte.three]);
	}
};
