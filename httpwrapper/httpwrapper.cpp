#include "httpwrapper.h"


namespace hackPSU {
	redisData* HTTPImpl::getDataFromPin(String pin){	//person inserts pin

		String url = "https://"+this.redisHost+"/tabs/getpin";
		String payload = "{\"pin\":"+pin+"}";
		int count = 1;	//will be used to measure number of headers
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

  		//This is an attempt to read directly from jsonObject
		redisData* pinData = new redisData;
		pinData->uid = root["uid"].asString();
		pinData->pin = root["pin"].asString();
		pinData->name = root["name"].asString();
		pinData->shirtSize = root["shirtSize"].asString();
		pinData->diet = root["diet"].asString();
		pinData->counter = root["counter"].asString();
		pinData->numScans = root["numScans"].asString();

  		return pinData;
	}

	bool HTTPImpl::assignRfidToUser(String userId, String userBandId){ //userId sent in from band, locationId sent in from device

		String url = "https://"+this.redisHost+"/tabs/setup";
		String payload = "{\"userID\":"+userId+",\"userBandId\":"+userBandId+"}";
		int count = 1;	//will be used to measure number of headers
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

  		return root["status"].asString() == "success";
	}

	bool HTTPImpl::entryScan(String userBandId, String locationId){

		String url = "https://"+this.redisHost+"/tabs/add";
		String payload = "{\"userBandID\":"+userBandId+",\"locationId\":"+locationId+"}";
		int count = 1;	//will be used to measure number of headers
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

		//I'm pulling this from pj's documentation and this might be incorrect
		String name = root["name"].asString();
		String description = root["description"].asString();	//I'm assuming this will be either True/False?
		
		//If description is false should the code return why they cannot get in (i.e they are not regsitered for the hackathon)

  		//The following is based on assumptions and should be checked
  		return (description == "success");
	}

}