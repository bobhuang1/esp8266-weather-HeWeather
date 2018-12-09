#pragma once
#include <JsonListener.h>
#include <JsonStreamingParser.h>

typedef struct HeWeatherForecastData {
	uint16_t cond_code_d; // condition code for day
	String cond_txt_d; // Text description for day
	String cond_txt_n; // Text description for night
	String date; // Date 2018-08-09
	uint16_t hum;  // humidity
	float pcpn; // precipitation in mm
	uint16_t pop; // probablity of precipitation
	int16_t tmp_max;  // max temperature
	int16_t tmp_min;  // min temperature
	String wind_dir; // wind direction
	String wind_sc; // wind degree
	uint16_t wind_spd; // wind speed in km/h
   // We will get MeteoCon from code
	String iconMeteoCon;
} HeWeatherForecastData;

class HeWeatherForecast : public JsonListener {
private:
	String currentKey;
	String currentParent;
	HeWeatherForecastData *data;
	uint8_t currentForecast;
	uint8_t doUpdate(HeWeatherForecastData *data, String url);

public:
	HeWeatherForecast();
	uint8_t updateForecast(HeWeatherForecastData *data, String appId, String location, String language);

	String getMeteoconIcon(String icon);
	virtual void whitespace(char c);

	virtual void startDocument();

	virtual void key(String key);

	virtual void value(String value);

	virtual void endArray();

	virtual void endObject();

	virtual void endDocument();

	virtual void startArray();

	virtual void startObject();
};
