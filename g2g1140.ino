#include "cpu.h"
#include "bist.h"

enum State 
{
	PROMPT,
	RUNNING
};

mmu_t *mmu;
cpu_t *cpu;
State state;

void setup()
{
	mmu = mmu_create(131072);
	cpu = cpu_create(mmu);
	Serial.begin(9600);
	Serial.setTimeout(-1);

	delay(5000);
	bist(cpu, mmu);

	Serial.println("the world's worst boot monitor v1.00");
	state = PROMPT;
}

void loop()
{
	switch (state) 
	{
	case PROMPT:
		char cmd[80];

		Serial.print("> ");
		Serial.flush();
		Serial.readBytesUntil('\r', cmd, 80);

		state = RUNNING;

		Serial.println();

		break;

	case RUNNING:
		// STUB: actually run things
		Serial.print(cpu_dump(cpu));
		delay(1000);

		int ch;

		do 
		{
			ch = Serial.read();

			if (ch == 3) // ctrl-c
			{
				state = PROMPT;
			}
		}
		while (ch != -1);

		break;
	}
}
