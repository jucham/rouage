#ifndef FLOWCONTROL_H_
#define FLOWCONTROL_H_
#include <cstdio>

class FlowControl
{
public:

	FlowControl()
	{
		printf( "Flow control initialized\n" );
		Reset();
	}

	void Reset()
	{
		mode = BAD;
		penalty_time = 4.0f;
		good_conditions_time = 0.0f;
		penalty_reduction_accumulator = 0.0f;
	}

	void Update( float deltaTime, float rtt )
	{
		const float RTT_Threshold = 250.0f;

		if ( mode == GOOD )
		{
			if ( rtt > RTT_Threshold )
			{
				printf( "*** dropping to bad mode ***\n" );
				mode = BAD;
				if ( good_conditions_time < 10.0f && penalty_time < 60.0f )
				{
					penalty_time *= 2.0f;
					if ( penalty_time > 60.0f )
						penalty_time = 60.0f;
					printf( "penalty time increased to %.1f\n", penalty_time );
				}
				good_conditions_time = 0.0f;
				penalty_reduction_accumulator = 0.0f;
				return;
			}

			good_conditions_time += deltaTime;
			penalty_reduction_accumulator += deltaTime;

			if ( penalty_reduction_accumulator > 10.0f && penalty_time > 1.0f )
			{
				penalty_time /= 2.0f;
				if ( penalty_time < 1.0f )
					penalty_time = 1.0f;
				printf( "penalty time reduced to %.1f\n", penalty_time );
				penalty_reduction_accumulator = 0.0f;
			}
		}

		if ( mode == BAD )
		{
			if ( rtt <= RTT_Threshold )
				good_conditions_time += deltaTime;
			else
				good_conditions_time = 0.0f;

			if ( good_conditions_time > penalty_time )
			{
				printf( "*** upgrading to good mode ***\n" );
				good_conditions_time = 0.0f;
				penalty_reduction_accumulator = 0.0f;
				mode = GOOD;
				return;
			}
		}
	}

	float GetSendRate()
	{
		return mode == GOOD ? 30.0f : 10.0f;
	}

private:

	enum Mode
	{
		GOOD,
		BAD
	};

	Mode mode;
	float penalty_time;
	float good_conditions_time;
	float penalty_reduction_accumulator;
};

#endif
