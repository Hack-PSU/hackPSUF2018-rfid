#include "httpwrapper.h"


namespace hackPSU {
	static jawn HTTPImpl::getDataFromPin(String pin){	//person inserts pin
		/*
		post request sent to redis with pin 	redis url: tabs/getpin
		parse through above information and assign to variable(s)
		return variables
		*/

		String url = "https://"+redisHost+"/tabs/getpin";
		String payload = "{\"pin\":"+pin+"}";
		int count = 1;	//will be used to measure number of headers
		Headers headers [] = { { "Content-Type", "application/json" } };
		

		Response* response = HTTP.POST(url, payload, count, headers);

		if (response->responseCode < 0){
			throw "http request failed :(";
		}

		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response->payload);

		// Parse from JSONObject into type jawn
		if (!root.success()) {
			throw "json parsing failed :( lit";
  		}
	}

	static jawn HTTPImpl::assignRfidToUser(String userId, String userBandId){ //userId sent in from band, locationId sent in from device
		/*
		post request sent to redis with userId and locationId     redis url: tabs/setup
		redis returns if they are good or not
		return ok or not
		*/

		String url = "https://"+redisHost+"/tabs/setup";
		String payload = "{\"userID\":"+userId+",\"userBandId\":"+userBandId+"}";
		int count = 1;	//will be used to measure number of headers
		Headers headers [] = { { "Content-Type", "application/json" } };


		Response* response = HTTP.POST(url, payload, count, headers);

		if (response->responseCode < 0){
			throw "http request failed :(";
		}

		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response->payload);

		// Parse from JSONObject into type jawn
		if (!root.success()) {
			throw "json parsing failed :(";
  		}
	}

	static bool HTTPImpl::entryScan(String userBandId, String locationId){
		/*
		post request sent to redis with userId and locationId      redis url: tabs/add
		parse through above information and assign to variable(s)
		return variables (whether they are good to go or not)
		*/

		String url = "https://"+redisHost+"/tabs/add";
		String payload = "{\"userBandID\":"+userBandId+",\"locationId\":"+locationId+"}";
		int count = 1;	//will be used to measure number of headers
		Headers headers [] = { { "Content-Type", "application/json" } };


		Response* response = HTTP.POST(url, payload, count, headers);

		if (response->responseCode < 0){
			throw "http request failed :(";
		}

		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response->payload);
		
		// Parse from JSONObject into type jawn
		if (!root.success()) {
			throw "json parsing failed :(";
  		}

  		//The following is based on assumptions and should be checked

  		return (bool) root["data"][0];
	}

}