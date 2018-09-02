/*#include "hackPSUhttp.h"

namespace hackPSU {
	Response HTTP::GET(String url) {
	    HTTPClient http;
	    Response response;
	    http.begin(url);
	    int httpCode = http.GET();
		
	    //httpCode will be negative (or zero) if there is an error
	    if (httpCode > 0){ 	//If the GET call was successful
		    response.payload = http.getString(); 
		    response.responseCode = httpCode;
	    } else {
		    response.payload = http.errorToString(httpCode).c_str();
		    response.responseCode = httpCode;
	    }
	http.end();
	return responseInfo;
	};
}



*/
