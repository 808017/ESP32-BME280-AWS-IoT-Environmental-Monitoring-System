# Amazon DynamoDB

DynamoDB is used to store the BME280 sensor readings received from AWS IoT Core.

## Stored Parameters

- Device ID
- Timestamp
- Temperature
- Humidity
- Pressure

## Data Flow

AWS IoT Core
↓
IoT Rule
↓
DynamoDB