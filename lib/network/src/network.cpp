#include "network.hpp"

namespace hackPSU {


  Network::Network(char* name, String host): 
    Network(name) 
  {
    // Addressable method
    setAddress(host);
  }

  Network::Network(char* name): 
    Readable<Response*>::Readable(name),
    Writable<Request*>::Writable(name),
    Addressable<String>::Addressable()
  {
    return;
  }

  Request* Network::createRequest(API::Method method, String route){
    Request* req = createRequest(method);
    req->setAddress(route);
    return req;
  }

  Request* Network::createRequest(API::Method method){
    Request* req = new Request(method, getAddress());
    return req;
  }

  void Network::pre_send(Request* request) {
    return;
  }

  Response* Network::sendRequest(Request* req, int retry) {
    if(!req) { return nullptr; }

    pre_send(req);
    write(req);
    
    Response* res = read();
    post_send(req, res);

    return res;
  }

}
