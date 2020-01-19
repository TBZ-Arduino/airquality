//WiFi settings
#define SECRET_SSID "4arduino"
#define SECRET_PASS "12345678900"
//influxdb settings
#define INFLUXDB_HOST “influxdb.example.org"
influx.setBucket(“myBucket");
influx.setVersion(2);
influx.setOrg(“myOrg");
influx.setPort(9999);
influx.setToken(“myToken");
