#ifndef WEATHER_INFO
#define WEATHER_INFO

typedef struct{
    float temp;
    float high;
    float low;
    float feels;
    int humidity;
    float dew_point;
    float wind_speed;
    int wind_direction;
    float precipitation;
    char description[100];
}weather_data_t;

weather_data_t* get_weather_data();

void fetch_weather_data(void);

#endif