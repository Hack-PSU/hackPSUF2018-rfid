#include "httpwrapper.h"


namespace hackPSU {
	redisData* HTTPImpl::getDataFromPin(String pin){	

		String url = "https://"+this.redisHost+"/tabs/getpin";
		String payload = "{\"pin\":"+pin+"}";
		int count = 1;	
		Headers headers [] = { { "Content-Type", "application/json" } };
		

		Response* response = HTTP.POST(url, payload, count, headers);

		if (response->responseCode < 0){
			throw "http request failed :(";
		}

		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response->payload);

		//Free up memory since parsing is complete
		delete response;

		if (!root.success()) {
			throw "json parsing failed :( lit";
  		}

  		//Redis json parse
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

	bool HTTPImpl::assignRfidToUser(String userId, String userBandId){ 

		String url = "https://"+this.redisHost+"/tabs/setup";
		String payload = "{\"userID\":"+userId+",\"userBandId\":"+userBandId+"}";
		int count = 1;	
		Headers headers [] = { { "Content-Type", "application/json" } };


		Response* response = HTTP.POST(url, payload, count, headers);

		if (response->responseCode < 0){
			throw "http request failed :(";
		}

		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response->payload);

		//Free up memory since parsing is complete
		delete response;

		if (!root.success()) {
			throw "json parsing failed :(";
  		}

  		//Redis json parse
  		return root["status"].asString() == "success";
	}

	bool HTTPImpl::entryScan(String userBandId, String locationId){


		String url = "https://"+this.redisHost+"/tabs/add";
		String payload = "{\"userBandID\":"+userBandId+",\"locationId\":"+locationId+"}";
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

		//Free up memory since parsing is complete
		delete response;

		//Redis json parse
		String status = root["status"].asString();
		String data = root["data"].asString();
		String message = root["message"].asString();	//Should message also be returned to display why user was not allowed in?
		
  		//The following is based on assumptions and should be checked
  		return (status == "success");
	}

}