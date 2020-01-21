//WiFi settings
#define SECRET_SSID "4arduino";
#define SECRET_PASS "12345678900";
//influxdb settings
char influxdbServer[] = "10.0.0.1";
int influxdbPort = 9999;
const int influxdbBufferSize = 2048;
const String influxdbOrgId = "0rg1d";
const String influxdbBucketName = "arduino";
const String influxdbAuthToken = "AB12AuthToken==";
