file(GLOB app_src "*.c")
set(pri_req esp_event esp_http_server esp_http_client esp-tls esp_https_ota esp_wifi esp_partition mqtt bt 
freertos spiffs nvs_flash driver vfs json log cfg_hadilao webserver_hadilao)

idf_component_register(SRCS ${app_src}                          
                       INCLUDE_DIRS "."
                    REQUIRES ${pri_req}
                    EMBED_TXTFILES ${project_dir}/certs/client.crt
                                 ${project_dir}/certs/client.key
                                 ${project_dir}/certs/mosquitto.org.crt)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")