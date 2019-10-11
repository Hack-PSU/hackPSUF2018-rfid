#pragma once

namespace hackPSU{

template<class Address>
class Addressable{
public:
    Addressable() {}
    ~Addressable() {}
    void setAddress(Address position) { 
        address = position;
    }

    Address getAddress(){
        return address; 
    }

private:
    Address address;

};

}