#include "httpwrapper.h"


namespace hackPSU {
	static jawn getDataFromPin(int pin){	//person inserts pin
		/*
		post request sent to redis with pin 	redis url: tabs/getpin
		parse through above information and assign to variable(s)
		return variables
		*/

		String url = "https://"+redisHost+"/tabs/getpin";
		String payload = "{\"pin\":"+String(pin)+"}";
		int count = 1;
		Headers headers [] = { { "Content-Type", "application/json" } };
		

		Response* response = HTTP.POST(url, payload, count, headers);

		if (response->responseCode < 0){
			throw "http request failed :(";
		}

		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response->payload);

		if (!root.success()) {
			throw "json parsing failed :( lit";
  		}
	}

	static jawn assignRfidToUser(String userId, String userBandId){ //userId sent in from band, locationId sent in from device
		/*
		post request sent to redis with userId and locationId     redis url: tabs/setup
		redis returns if they are good or not
		return ok or not
		*/

		String url = "https://"+redisHost+"/tabs/setup";
		String payload = "{\"userID\":"+userId+",\"locationId\":"+locationId+"}";
		int count = 1;
		Headers headers [] = { { "Content-Type", "application/json" } };


		Response* response = HTTP.POST(url, payload, count, headers);

		if (response->responseCode < 0){
			throw "http request failed :(";
		}

		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response->payload);

		if (!root.success()) {
			throw "json parsing failed :(";
  		}
	}

	static bool entryScan(String userBandId, String locationId){
		/*
		post request sent to redis with userId and locationId      redis url: tabs/getpin
		parse through above information and assign to variable(s)
		return variables (whether they are good to go or not)
		*/

		String url = "https://"+redisHost+"/tabs/getpin";
		String payload = "{\"userID\":"+userId+",\"locationId\":"+locationId+"}";
		int count = 1;
		Headers headers [] = { { "Content-Type", "application/json" } };


		Response* response = HTTP.POST(url, payload, count, headers);

		if (response->responseCode < 0){
			throw "http request failed :(";
		}

		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response->payload);

		if (!root.success()) {
			throw "json parsing failed :(";
  		}

  		//The following is based on assumptions and should be checked
  		bool entryAllowedOrNot = root["data"][0];

  		return entryAllowedOrNot;
	}

}