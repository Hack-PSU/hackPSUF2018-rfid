#include "hackPSUhttp.h"

namespace hackPSU {
	static jawn getDataFromPin(int pin){	//person inserts pin
		/*
		post request sent to redis with pin 	redis url: tabs/getpin
		parse through above information and assign to variable(s)
		return variables
		*/

	}

	static jawn assignRfidToUser(String userId, String userBandId){ //userId sent in from band, locationId sent in from device
		/*
		post request sent to redis with userId and locationId     redis url: tabs/setup
		redis returns if they are good or not
		return ok or not
		*/
	}

	static bool entryScan(String userBandId, String locationId){
		/*
		post request sent to redis with userId and locationId      redis url: tabs/getpin
		parse through above information and assign to variable(s)
		return variables (whether they are good to go or not)
		*/
	}

}