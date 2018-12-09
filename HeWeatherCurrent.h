#pragma once
#include <JsonListener.h>
#include <JsonStreamingParser.h>

typedef struct HeWeatherCurrentData {
	uint16_t cond_code; // condition code
	String cond_txt; // Text description
	uint16_t hum;  // humidity
	int16_t tmp;  // temperature
	String wind_dir; // wind direction
	String wind_sc; // wind degree
	uint16_t wind_spd; // wind speed in km/h
   // We will get MeteoCon from code
	String iconMeteoCon;
} HeWeatherCurrentData;

class HeWeatherCurrent : public JsonListener {
private:
	String currentKey;
	String currentParent;
	HeWeatherCurrentData *data;
	void doUpdate(HeWeatherCurrentData *data, String url);

public:
	HeWeatherCurrent();
	void updateCurrent(HeWeatherCurrentData *data, String appId, String location, String language);

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
