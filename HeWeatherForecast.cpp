#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "HeWeatherForecast.h"

HeWeatherForecast::HeWeatherForecast() {

}

uint8_t HeWeatherForecast::updateForecast(HeWeatherForecastData *data, String appId, String location, String language) {
	return doUpdate(data, "/s6/weather/forecast?location=" + location + "&key=" + appId + "&lang=" + language);
	// https://free-api.heweather.com/s6/weather/forecast?location=CN101210202&key=d72b42bcfc994cfe9099eddc9647c6f2&lang=zh
}

uint8_t HeWeatherForecast::doUpdate(HeWeatherForecastData *data, String url) {
	Serial.println("doUpdate");
	Serial.println(url);
	if (WiFi.status() != WL_CONNECTED) return 0;
	JsonStreamingParser parser;
	parser.setListener(this);
	WiFiClientSecure client;
	client.setFingerprint("07d3c71441672357ef98fbfe9a67f22940377103");
	unsigned long lostTest = 30000UL;
	unsigned long lost_do = millis();
	this->data = data;
	this->currentForecast = 0;

	const char host[] = "free-api.heweather.com";
	Serial.println(host);

	const int httpsPort = 443;
	if (!client.connect(host, httpsPort)) {
		Serial.println("connection failed");
		this->data = nullptr;
		return 0;
	}

	Serial.print("Requesting Host: ");
	Serial.println(host);
	Serial.print("Requesting URL: ");
	Serial.println(url);

	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
		"Host: " + host + "\r\n" +
		"User-Agent: IBEDevices-ESP8266\r\n" +
		"Connection: close\r\n\r\n");

	Serial.println("request sent");

	int retryCounter = 0;
	while (!client.available()) {
		Serial.println(".");
		delay(1000);
		retryCounter++;
		if (retryCounter > 10) {
			this->data = nullptr;
			return 0;
		}
	}

	int pos = 0;
	boolean isBody = false;
	char c;

	int size = 0;
	client.setNoDelay(false);
	while (client.connected()) {
		while ((size = client.available()) > 0) {
			if ((millis() - lost_do) > lostTest) {
				Serial.println("lost in client with a timeout");
				client.stop();
				this->data = nullptr;
				return 0;
//				ESP.restart();
			}
			c = client.read();
			if (c == '{' || c == '[') {
				isBody = true;
			}
			if (isBody) {
				parser.parse(c);
			}
		}
	}
	this->data = nullptr;
	return currentForecast;
}


void HeWeatherForecast::whitespace(char c) {
	Serial.println("whitespace");
}

void HeWeatherForecast::startDocument() {
	Serial.println("start document");
}

void HeWeatherForecast::key(String key) {
	currentKey = String(key);
}

void HeWeatherForecast::value(String value) {
	int intMaxForecasts = 5;
	if (currentForecast < intMaxForecasts && currentKey == "cond_code_d") {
		data[currentForecast].cond_code_d = value.toInt();
		data[currentForecast].iconMeteoCon = getMeteoconIcon(value);
	}
	if (currentForecast < intMaxForecasts && currentKey == "cond_txt_d") {
		data[currentForecast].cond_txt_d = value;
	}
	if (currentForecast < intMaxForecasts && currentKey == "cond_txt_n") {
		data[currentForecast].cond_txt_n = value;
	}
	if (currentForecast < intMaxForecasts && currentKey == "date") {
		data[currentForecast].date = value;
	}
	if (currentForecast < intMaxForecasts && currentKey == "hum") {
		data[currentForecast].hum = value.toInt();
	}
	if (currentForecast < intMaxForecasts && currentKey == "pcpn") {
		data[currentForecast].pcpn = value.toFloat();
	}
	if (currentForecast < intMaxForecasts && currentKey == "pop") {
		data[currentForecast].pop = value.toInt();
	}
	if (currentForecast < intMaxForecasts && currentKey == "tmp_max") {
		data[currentForecast].tmp_max = value.toInt();
	}
	if (currentForecast < intMaxForecasts && currentKey == "tmp_min") {
		data[currentForecast].tmp_min = value.toInt();
	}
	if (currentForecast < intMaxForecasts && currentKey == "wind_dir") {
		data[currentForecast].wind_dir = value;
	}
	if (currentForecast < intMaxForecasts && currentKey == "wind_sc") {
		data[currentForecast].wind_sc = value;
	}
	if (currentForecast < intMaxForecasts && currentKey == "wind_spd") {
		data[currentForecast].wind_spd = value.toInt();
		currentForecast++;
	}
}

void HeWeatherForecast::endArray() {

}


void HeWeatherForecast::startObject() {
	currentParent = currentKey;
}

void HeWeatherForecast::endObject() {
}

void HeWeatherForecast::endDocument() {

}

void HeWeatherForecast::startArray() {

}


String HeWeatherForecast::getMeteoconIcon(String code) {
	if (code == "100") { return "B2"; }      //   Sunnyt           ;        Clear
	if (code == "101") { return "N5"; }      //5   Cloudy-day;        Cloudy-night. Code: 119
	if (code == "102") { return "HI"; }      //I   Partly cloudy-day;        Partly cloudy-night. Code: 116
	if (code == "103") { return "HI"; }      //I   Partly cloudy-day;        Partly cloudy-night. Code: 116
	if (code == "104") { return "Y%"; }      //%   Overcast-day;        Overcast-night. Code: 122
/*
200	有风	Windy
201	平静	Calm
202	微风	Light Breeze
203	和风	Moderate/Gentle Breeze
204	清风	Fresh Breeze
205	强风/劲风	Strong Breeze
206	疾风	High Wind, Near Gale
207	大风	Gale
208	烈风	Strong Gale
209	风暴	Storm
210	狂爆风	Violent Storm
211	飓风	Hurricane
212	龙卷风	Tornado
213	热带风暴	Tropical Storm
*/
	if (code == "200") { return "D9"; }
	if (code == "201") { return "D9"; }
	if (code == "202") { return "D9"; }
	if (code == "203") { return "D9"; }
	if (code == "204") { return "D9"; }
	if (code == "205") { return "EE"; }
	if (code == "206") { return "EE"; }
	if (code == "207") { return "FF"; }
	if (code == "208") { return "FF"; }
	if (code == "209") { return "FF"; }
	if (code == "210") { return "FF"; }
	if (code == "211") { return "FF"; }
	if (code == "212") { return "FF"; }
	if (code == "213") { return "FF"; }
/*
300	阵雨	Shower Rain
301	强阵雨	Heavy Shower Rain
302	雷阵雨	Thundershower
303	强雷阵雨	Heavy Thunderstorm
304	雷阵雨伴有冰雹	Thundershower with hail
305	小雨	Light Rain
306	中雨	Moderate Rain
307	大雨	Heavy Rain
308	极端降雨	Extreme Rain
309	毛毛雨/细雨	Drizzle Rain
310	暴雨	Storm
311	大暴雨	Heavy Storm
312	特大暴雨	Severe Storm
313	冻雨	Freezing Rain
314	小到中雨	Light to moderate rain
315	中到大雨	Moderate to heavy rain
316	大到暴雨	Heavy rain to storm
317	暴雨到大暴雨	Storm to heavy storm
318	大暴雨到特大暴雨	Heavy to severe storm
399	雨	Rain
*/
	if (code == "300") { return "Q7"; }      //7   Light rain shower-day;        Light rain shower-night. Code: 353
	if (code == "301") { return "R8"; }      //8   Moderate or heavy rain shower-day;        Moderate or heavy rain shower-night. Code: 356
	if (code == "302") { return "Z&"; }      //7   Torrential rain shower-day;        Torrential rain shower-night. Code: 359
	if (code == "303") { return "Z&"; }      //7   Torrential rain shower-day;        Torrential rain shower-night. Code: 359
	if (code == "304") { return "Z&"; }      //7   Torrential rain shower-day;        Torrential rain shower-night. Code: 359
	if (code == "305") { return "Q7"; }      //7   Light rain-day;        Light rain-night. Code: 296
	if (code == "306") { return "R8"; }      //8   Moderate rain-day;        Moderate rain-night. Code: 302
	if (code == "307") { return "X$"; }      //$   Heavy rain at times-day;        Heavy rain at times-night. Code: 305
	if (code == "308") { return "X$"; }      //$   Heavy rain-day;        Heavy rain-night. Code: 308
	if (code == "309") { return "Q7"; }      //7   Light rain shower-day;        Light rain shower-night. Code: 353
	if (code == "310") { return "X$"; }      //$   Heavy rain-day;        Heavy rain-night. Code: 308
	if (code == "311") { return "X$"; }      //$   Heavy rain-day;        Heavy rain-night. Code: 308
	if (code == "312") { return "X$"; }      //$   Heavy rain-day;        Heavy rain-night. Code: 308
	if (code == "313") { return "R8"; }      //8   Moderate or heavy freezing rain-day;        Moderate or heavy freezing rain-night. Code: 314
	if (code == "314") { return "Q7"; }      //7   Light rain-day;        Light rain-night. Code: 296
	if (code == "315") { return "R8"; }      //8   Moderate rain-day;        Moderate rain-night. Code: 302
	if (code == "316") { return "X$"; }      //$   Heavy rain at times-day;        Heavy rain at times-night. Code: 305
	if (code == "317") { return "X$"; }      //$   Heavy rain-day;        Heavy rain-night. Code: 308
	if (code == "318") { return "X$"; }      //$   Heavy rain-day;        Heavy rain-night. Code: 308
	if (code == "399") { return "X$"; }      //$   Heavy rain-day;        Heavy rain-night. Code: 308
/*
400	小雪	Light Snow
401	中雪	Moderate Snow
402	大雪	Heavy Snow
403	暴雪	Snowstorm
404	雨夹雪	Sleet
405	雨雪天气	Rain And Snow
406	阵雨夹雪	Shower Snow
407	阵雪	Snow Flurry
408	小到中雪	Light to moderate snow
409	中到大雪	Moderate to heavy snow
410	大到暴雪	Heavy snow to snowstorm
499	雪	Snow
*/
	if (code == "400") { return "U\""; }      //"   Light snow showers-day;        Light snow showers-night. Code: 368
	if (code == "401") { return "W#"; }      //#   Moderate snow-day;        Moderate snow-night. Code: 332
	if (code == "402") { return "W#"; }      //#   Heavy snow-day;        Heavy snow-night. Code: 338
	if (code == "403") { return "W#"; }      //#   Heavy snow-day;        Heavy snow-night. Code: 338
	if (code == "404") { return "U\""; }      //"   Light snow showers-day;        Light snow showers-night. Code: 368
	if (code == "405") { return "U\""; }      //"   Light snow showers-day;        Light snow showers-night. Code: 368
	if (code == "406") { return "U\""; }      //"   Light snow showers-day;        Light snow showers-night. Code: 368
	if (code == "407") { return "U\""; }      //"   Patchy snow possible-day;        Patchy snow possible-night. Code: 179
	if (code == "408") { return "W#"; }      //#   Heavy snow-day;        Heavy snow-night. Code: 338
	if (code == "409") { return "W#"; }      //#   Heavy snow-day;        Heavy snow-night. Code: 338
	if (code == "410") { return "W#"; }      //#   Heavy snow-day;        Heavy snow-night. Code: 338
	if (code == "499") { return "W#"; }      //#   Heavy snow-day;        Heavy snow-night. Code: 338
/*
500	薄雾	Mist
501	雾	Foggy
502	霾	Haze
503	扬沙	Sand
504	浮尘	Dust
507	沙尘暴	Duststorm
508	强沙尘暴	Sandstorm
509	浓雾	Dense fog
510	强浓雾	Strong fog
511	中度霾	Moderate haze
512	重度霾	Heavy haze
513	严重霾	Severe haze
514	大雾	Heavy fog
515	特强浓雾	Extra heavy fog
*/
	if (code == "500") { return "M9"; }      //9   Mist-day;        Mist-night. Code: 143
	if (code == "501") { return "JK"; }      //K   Fog-day;        Fog-night. Code: 248
	if (code == "502") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "503") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "504") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "505") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "506") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "507") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "508") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "509") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "510") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "511") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "512") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "513") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "514") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "515") { return "JK"; }      //K  Haze Fog-day;        Fog-night. Code: 248
/*
900	热	Hot
901	冷	Cold
999	未知	Unknown
*/
	if (code == "900") { return "''"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	if (code == "901") { return "''"; }      //K  Haze Fog-day;        Fog-night. Code: 248
	return "))";   // Nothing matched: N/A
}
