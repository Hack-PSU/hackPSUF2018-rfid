#include "httpwrapper.h"


namespace hackPSU {
	bool HTTPImpl::getAPIKey(){

    //TODO MAKE DAT ONE SECURE BOI
		String url = "http://"+redisHost+"/auth/scanner/register";
		String payload = "{\"pin\":\""+String(MASTER_KEY)+"\"}";
		int headerCount = 1;
		Headers headers [] = { { "Content-Type", "application/json" } };


		Response* response = HTTP::POST(url, payload, headerCount, headers);
    Serial.println(response->payload);

		if (response->responseCode < 0){
			Serial.print("Http request failed: ");
			Serial.println(HTTP::handleError(response->responseCode));
      Serial.println(response->errorMessage);
			//Free up memory since parsing is complete
			delete response;
			return false;
		}

		StaticJsonBuffer<500> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(response->payload);

		//Free up memory since parsing is complete
		delete response;

		//Redis json parse
		String status = root["status"];
    Serial.println(status);
		String message = root["message"];	//Should message also be returned to display why user was not allowed in?
		JsonObject& data = jsonBuffer.parseObject(root["data"].as<String>());
    apiKey = data.get<String>("apikey");
    Serial.println(apiKey);
    //The following is based on assumptions and should be checked
  	return (status == "success");
	}

	redisData* HTTPImpl::getDataFromPin(String pin){
		String url = "http://"+redisHost+"/tabs/getpin";
    Serial.println(url);
		String payload = "{\"pin\":"+pin+"}";
		int headerCount = 1;
		Headers headers [] = { { "Content-Type", "application/json" } };


		Response* response = HTTP::POST(url, payload, headerCount, headers);

		if (response->responseCode < 0){
			Serial.print("Http request failed: ");
			Serial.println(HTTP::handleError(response->responseCode));
			delete response;
			return false;
		}

    Serial.println("RESPONSE payload");
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

		String url = "http://"+redisHost+"/tabs/setup";
		String payload = "{\"pin\":"+pin+", \"apikey\": \""+apiKey+"\"}";
		int headerCount = 1;
		Headers headers [] = { { "Content-Type", "application/json" } };


		Response* response = HTTP::POST(url, payload, headerCount, headers);

		if (response->responseCode < 0){
			Serial.print("Http request failed: ");
			Serial.println(HTTP::handleError(response->responseCode));
			//Free up memory since parsing is complete
			delete response;
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


		String url = "http://"+redisHost+"/tabs/add";
		String payload = "{\"location\":"+locationId+",\"id\":"+rfidTag+"}";
		int headerCount = 1;
		Headers headers [] = { { "Content-Type", "application/json" } };


		Response* response = HTTP::POST(url, payload, headerCount, headers);

		if (response->responseCode < 0){
			Serial.print("Http request failed: ");
			Serial.println(HTTP::handleError(response->responseCode));
			//Free up memory since parsing is complete
			delete response;
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

  Location* HTTPImpl::getLocations(int* len){
    String url = "http://"+redisHost+"/tabs/active-locations";
    Response* response = HTTP::GET(url);

    if (response->responseCode < 0) {
      Serial.println("GET REQUEST FAIL");
      delete response;
      return nullptr;
    }

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(response->payload);

    delete response;
    *len = root["length"];
    Location *locations = new Location[*len];

    for(int i = 0; i < *len; i++){
      locations[i] = {.name = root["locations"][i]["location_name"], .id = root["locations"][i]["uid"]};
    }

    return locations;

  }


}
