# IoT_20232
MQTT    _server: test.mosquitto.org  
        _topic MQTT address:  
             /mandevices/response: Response data from Node  
                /mandevices/receive: Receive .conf data from user  
                /mandevices/gateway: Upload data with TLS enabled  
        _SSL: two-way authentic  
CoAP    _server : theThings.io  
        include:    _token_ID  
                    _path  
