# c_ml

A C++ webserver that calculates linear regression from JSON input containing x/y coordinates.

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running

```bash
./server [port]
```

Default port is 8080 if not specified.

## API Usage

### Calculate Linear Regression

**Endpoint:** `POST /linear-regression`

**Request Body:**
```json
{
  "points": [
    {"x": 1, "y": 2},
    {"x": 2, "y": 4},
    {"x": 3, "y": 6}
  ]
}
```

**Response:**
```json
{
  "slope": 2.0,
  "intercept": 0.0,
  "r_squared": 1.0
}
```

### Example with curl

```bash
curl -X POST http://localhost:8080/linear-regression \
  -H "Content-Type: application/json" \
  -d '{"points": [{"x": 1, "y": 2}, {"x": 2, "y": 4}, {"x": 3, "y": 6}]}'
```

## Features

- Simple HTTP server implementation
- Linear regression calculation (slope, intercept, R²)
- JSON request/response format
- Minimal dependencies (C++ standard library only)