#include <hackPSUhttp.h>

namespace hackPSU {
  	Response::GET(String url, String payload, struct headers){
  		HTTPClient http;
   		http.begin(url); //http Begin call
    		int httpCode = http.GET(); //GET call

    		http.addHeader(headerInfo.header);

  		//httpCode will be negative (or zero) if there is an error
  		if (httpCode > 0){ 	//If the GET call was successful
      			responseInfo.payload = http.getString(); 		//getting the repsonse from the get
      			responseInfo.responseCode = httpCode;
  		}else{
     			responseInfo.payload = http.errorToString(httpCode).c_str(); //I think this is the error information??
     			responseInfo.responseCode = httpCode; //should be negative or zero?
  			}
    		http.end();
  	}
	
  	Response::GET(String url, String payload){
    		HTTPClient http;
    		http.begin(url); //http Begin call
   		int httpCode = http.GET(); //GET call
    		http.addHeader(headerInfo.header);

  		//httpCode will be negative (or zero) if there is an error
  		if (httpCode > 0){ 	//If the GET call was successful
     			responseInfo.payload = http.getString(); 		//getting the repsonse from the get
      			responseInfo.responseCode = httpCode;
  		}else{
      			responseInfo.payload = http.errorToString(httpCode).c_str(); //I think this is the error information??
      			responseInfo.responseCode = httpCode; //should be negative or zero?
  		}
    		http.end();
  	}

  	Response::POST(String url, String payload, struct headers){
    		HTTPClient http;
    		http.begin(url); //http Begin call
    		int httpCode = http.GET(); //GET call
    		http.addHeader(headerInfo.header);

  		//httpCode will be negative (or zero) if there is an error
  		if (httpCode > 0){ 	//If the GET call was successful
      			responseInfo.payload = http.getString(); 		//getting the repsonse from the get
      			responseInfo.responseCode = httpCode;
  		}else{
      			responseInfo.payload = http.errorToString(httpCode).c_str(); //I think this is the error information??
      			responseInfo.responseCode = httpCode; //should be negative or zero?
  		}
    		http.end();
  	}

  	Response::POST(String url, String payload){
    		HTTPClient http;
    		http.begin(url); //http Begin call
    		int httpCode = http.GET(); //GET call
    		http.addHeader(headerInfo.header);

  		//httpCode will be negative (or zero) if there is an error
  		if (httpCode > 0){ 	//If the GET call was successful
      			responseInfo.payload = http.getString(); 		//getting the repsonse from the get
      			responseInfo.responseCode = httpCode;
  		}else{
      			responseInfo.payload = http.errorToString(httpCode).c_str(); //I think this is the error information??
      			responseInfo.responseCode = httpCode; //should be negative or zero?
  		}
    		http.end();
  	}	
}