#pragma once

class Utils
{
public: 
	// signed normalization function for scaling input value with a known input range to an output range
	inline float snorm(float value, float in_Min, float in_Max)
	{
		float out_Value = (((1.0f - -1.0f) * ((value - in_Min) / (in_Max - in_Min))) + -1.0f);
		return out_Value;
	}

	// unsigned normalization function
	inline float norm(float value, float in_Min, float in_Max)
	{
		float out_Value = (((1.0f - 0.0f) * ((value - in_Min) / (in_Max - in_Min))) + 0.0f);
		return out_Value;
	}

	inline float set_snorm(float value, float in_Min, float in_Max, float out_Min, float out_Max)
	{
		float out_Value = (((out_Max - out_Min) * ((value - in_Min) / (in_Max - in_Min))) + out_Min);
		return out_Value;
	}
};