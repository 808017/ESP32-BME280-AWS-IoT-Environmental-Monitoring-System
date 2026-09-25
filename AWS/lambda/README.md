# AWS Lambda

AWS Lambda is used as the backend layer for retrieving sensor data from DynamoDB and providing it to the web dashboard.

## Data Flow

DynamoDB
↓
Lambda
↓
API
↓
Web Dashboard