#include "httpwrapper.h"


namespace hackPSU {
	redisData* HTTPImpl::getDataFromPin(String pin){	
		String url = "http://"+redisHost+"/tabs/getpin";
    	Serial.println(url);
		String payload = "{\"pin\":"+pin+"}";
		int count = 1;	
		Headers headers [] = { { "Content-Type", "application/json" } };
		

		Response* response = HTTP::POST(url, payload, count, headers);

		if (response->responseCode < 0){
			Serial.print("Http request failed: ");
			Serial.println(HTTP::handleError(response->responseCode));
			return false;
		}

    	Serial.println(response->payload);

		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response->payload);

		//Free up memory since parsing is complete
		delete response;

//		if (!root.success()) {
//			throw "json parsing failed :( lit";
//  		}

  		//Redis json parse
		redisData* pinData = new redisData;
		pinData->uid = root["uid"];
		pinData->pin = root["pin"];
		pinData->name = root["name"];
		pinData->shirtSize = root["shirtSize"];
		pinData->diet = root["diet"];
		pinData->counter = root["counter"];
		pinData->numScans = root["numScans"];

  		return pinData;
	}

	bool HTTPImpl::assignRfidToUser(String rfidCode, String pin){ 

		String url = "https://"+redisHost+"/tabs/setup";
		String payload = "{\"id\":"+rfidCode+",\"pin\":"+pin+"}";
		int count = 1;	
		Headers headers [] = { { "Content-Type", "application/json" } };


		Response* response = HTTP::POST(url, payload, count, headers);

		if (response->responseCode < 0){
			Serial.print("Http request failed: ");
			Serial.println(HTTP::handleError(response->responseCode));
			return false;
		}

		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response->payload);

		//Free up memory since parsing is complete
		delete response;

//		if (!root.success()) {
//			throw "json parsing failed :(";
//  		}

  		//Redis json parse
  		return root["status"] == "success";
	}

	bool HTTPImpl::entryScan(String locationId, String rfidTag){


		String url = "https://"+redisHost+"/tabs/add";
		String payload = "{\"location\":"+locationId+",\"id\":"+rfidTag+"}";
		int count = 1;
		Headers headers [] = { { "Content-Type", "application/json" } };


		Response* response = HTTP::POST(url, payload, count, headers);

		if (response->responseCode < 0){
			Serial.print("Http request failed: ");
			Serial.println(HTTP::handleError(response->responseCode));
			return false;
		}



		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response->payload);

//		if (!root.success()) {
//			throw "json parsing failed :(";
//  		}

		//Free up memory since parsing is complete
		delete response;

		//Redis json parse
		String status = root["status"];
		String data = root["data"];
		String message = root["message"];	//Should message also be returned to display why user was not allowed in?
		
  		//The following is based on assumptions and should be checked
  		return (status == "success");
	}

}
