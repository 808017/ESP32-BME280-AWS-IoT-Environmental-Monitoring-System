#ifndef AWS_IOT_H
#define AWS_IOT_H

#include <stdbool.h>

void aws_iot_init(void);

bool aws_iot_is_connected(void);

bool aws_iot_publish(float temperature, float humidity);

bool aws_iot_publish_topic(
    const char *topic,
    float temperature,
    float humidity
);

#endif