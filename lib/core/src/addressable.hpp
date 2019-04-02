#pragma once

namespace hackPSU{

template<class Address>
class Addressable{
public:
    Addressable() {}
    ~Addressable() {}
    void setAddress(Address position) { 
        address = position; 
        post_set();
    }

    Address getAddress(){
        return address; 
    }

private:
    Address address;

    virtual void post_set(){
        return;
    }
};

}