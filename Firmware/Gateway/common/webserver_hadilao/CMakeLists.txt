file(GLOB app_src "*.c")
set(pri_req esp_event esp_http_server esp_http_client esp_https_ota esp_wifi esp_partition mqtt bt 
freertos spiffs nvs_flash driver vfs json log cfg_hadilao)

idf_component_register(SRCS ${app_src}                          
                       INCLUDE_DIRS "."
                    REQUIRES ${pri_req})
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")