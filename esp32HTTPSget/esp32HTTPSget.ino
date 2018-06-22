//Dependencies
//NTP library for getting accurate time
//NTPClient.h was edited to parse datetime required for aws signing process
//library found in the github repository
#include <NTPClient.h>
//WiFi libraries needed
#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
//Crypto library for hmac/sha256
#include <Crypto.h>
//Parameters needed to work
//Fill in accordingly
const char *ssid     = "";
const char *password = "";
String aws_access = "";
String aws_secret = "";
String aws_region = "ap-southeast-1";
String canonicalURL = "/things/SIMcom32/shadow";
String aws_service = "iotdata";
String aws_endpoint = "";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 0, 60000);
HTTPClient http;
//AWS endpoint tls cert
const char aws_root_ca_pem[] = \
                           "-----BEGIN CERTIFICATE-----\n" \
                           "MIIEajCCA1KgAwIBAgIQP5KHvp0dpKN6nfYoLndaxDANBgkqhkiG9w0BAQsFADCB\n" \
                           "yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\n" \
                           "ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\n" \
                           "U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\n" \
                           "ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\n" \
                           "aG9yaXR5IC0gRzUwHhcNMTUwNTEyMDAwMDAwWhcNMjUwNTExMjM1OTU5WjCBgDEL\n" \
                           "MAkGA1UEBhMCVVMxHTAbBgNVBAoTFFN5bWFudGVjIENvcnBvcmF0aW9uMR8wHQYD\n" \
                           "VQQLExZTeW1hbnRlYyBUcnVzdCBOZXR3b3JrMTEwLwYDVQQDEyhTeW1hbnRlYyBD\n" \
                           "bGFzcyAzIEVDQyAyNTYgYml0IFNTTCBDQSAtIEcyMFkwEwYHKoZIzj0CAQYIKoZI\n" \
                           "zj0DAQcDQgAEDxukkdfnrOfRTk63ZFvhj39uBNOrONtEt0Bcbb2WljffeYmGZ/ex\n" \
                           "Hwie/WM7RoyfvVPoFdyXPiuBRq2Gfw4BOaOCAV0wggFZMC4GCCsGAQUFBwEBBCIw\n" \
                           "IDAeBggrBgEFBQcwAYYSaHR0cDovL3Muc3ltY2QuY29tMBIGA1UdEwEB/wQIMAYB\n" \
                           "Af8CAQAwZQYDVR0gBF4wXDBaBgpghkgBhvhFAQc2MEwwIwYIKwYBBQUHAgEWF2h0\n" \
                           "dHBzOi8vZC5zeW1jYi5jb20vY3BzMCUGCCsGAQUFBwICMBkaF2h0dHBzOi8vZC5z\n" \
                           "eW1jYi5jb20vcnBhMC8GA1UdHwQoMCYwJKAioCCGHmh0dHA6Ly9zLnN5bWNiLmNv\n" \
                           "bS9wY2EzLWc1LmNybDAOBgNVHQ8BAf8EBAMCAQYwKwYDVR0RBCQwIqQgMB4xHDAa\n" \
                           "BgNVBAMTE1NZTUMtRUNDLUNBLXAyNTYtMjIwHQYDVR0OBBYEFCXwiuFLetkBlQrt\n" \
                           "xlPxjHgf2fP4MB8GA1UdIwQYMBaAFH/TZafC3ey78DAJ80M5+gKvMzEzMA0GCSqG\n" \
                           "SIb3DQEBCwUAA4IBAQAMMGUXBaWTdaLxsTGtcB/naqjIQrLvoV9NG+7MoHpGd/69\n" \
                           "dZ/h2zOy7sGFUHoG/0HGRA9rxT/5w5GkEVIVkxtWyIWWq6rs4CTZt8Bej/KHYRbo\n" \
                           "jtEDUkCTZSTLiCvguPyvinXgxy+LHT+PmdtEfXsvcdbeBSWUYpOsDYvD2hNtz9dw\n" \
                           "Od5nBosMApmdxt+z7LQyZu8wMnfI1U6IMO+RWowxZ8uy0oswdFYd32l9xe+aAE/k\n" \
                           "y9alLu/M9pvxiUKufqHJRgDBKA6uDjHLMPX+/nxXaNCPX3SI4KVZ1stHQ/U5oNlM\n" \
                           "dHN9umAvlU313g0IgJrjsQ2nIdf9dsdP+6lrmP7s\n" \
                           "-----END CERTIFICATE-----\n";
//Function for sha256 hash
String sha256 (String input) {
  SHA256 hasher;
  byte hash[32];
  String output;
  hasher.doUpdate(input.c_str());
  hasher.doFinal(hash);
  for (byte i = 0; i < SHA256_SIZE; i++)
  {
    char str[3];
    sprintf(str, "%02x", (int)hash[i]);
    output += (String) str;
  }
  return output;
}
//Function for hmac sha256 hashing
void hmac_sha256 (byte inputkey[], int inputbyteSize, String inputload, byte outputkey[]) {
  SHA256HMAC hmac(inputkey, inputbyteSize);
  hmac.doUpdate(inputload.c_str());
  hmac.doFinal(outputkey);
}
//Function for creating AWS signing key
//It passes in your aws secret key,region and service specified above
//Date is passed in the getAWSSignature function
//It return the output to the byte array outputkey
void getAWSSigningKey (String secret, String region, String service, String date, byte outputkey[]) {
  String ksecret = "AWS4" + secret;
  byte byteKSecret[45];
  byte sha256output[SHA256HMAC_SIZE];

  ksecret.getBytes(byteKSecret, sizeof(byteKSecret));
  hmac_sha256(byteKSecret, sizeof(byteKSecret), date, sha256output);
  hmac_sha256(sha256output, SHA256HMAC_SIZE, region, sha256output);
  hmac_sha256(sha256output, SHA256HMAC_SIZE, service, sha256output);
  hmac_sha256(sha256output, SHA256HMAC_SIZE, "aws4_request", outputkey);
}
//function for creating canonical request
//url is path to your aws iot thing
//host is your aws endpoint
//payload is the json package you are going to send
//awsdate is passed in getAWSSignature function
String getAWSCanonicalRequest (String url, String host, String payload, String awsdate) {
  String temp = "GET\n";
  temp += url + "\n";
  temp += "\ncontent-type:application/x-www-form-urlencoded\nhost:";
  temp += host + "\nx-amz-date:";
  temp += awsdate  + "\n\n";
  temp += "content-type;host;x-amz-date\n";
  temp += "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

  return temp;
}
//Function to create StringtoSign
//It passes the canonical request from before
//awsdate and date are passed in getAWSSignature function
String getStringtoSign (String canonical, String awsdate, String date) {
  String temp;
  temp = "AWS4-HMAC-SHA256\n";
  temp += awsdate + "\n";
  temp += date + "/";
  temp += aws_region + "/";
  temp += "iotdata/aws4_request\n";
  temp += sha256(canonical);

  return temp;
}
//Function to create the signature at the end of the authorization header
//This function calls all the previous function to create the signature
String getAWSSignature (String secret, String region, String service, String url, String host, String payload , String AwsDate, String Date) {
  byte signCode[SHA256HMAC_SIZE];
  byte signaturebyte[SHA256HMAC_SIZE];
  String signature;

  getAWSSigningKey (secret, region, service, Date, signCode);
  String canonical = getAWSCanonicalRequest(url, host, payload, AwsDate);
  String stringToSign = getStringtoSign(canonical, AwsDate, Date);
  Serial.println(canonical);
  Serial.println();
  Serial.println(stringToSign);

  hmac_sha256 (signCode, SHA256HMAC_SIZE, stringToSign, signaturebyte);

  for (byte i = 0; i < SHA256HMAC_SIZE; i++) {
    char str[3];
    sprintf(str, "%02x", (int)signaturebyte[i]);
    signature += (String) str;
  }
  return signature;
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println();
  Serial.println("Connected to ");
  Serial.println(ssid);
  //start ntp client
  timeClient.begin();
  delay(1000);
  timeClient.update();
}

void loop() {
  //payload is the json package you send to aws iot
  String payload = "";

  //Check the current connection status
  if ((WiFi.status() == WL_CONNECTED)) {
    //the four lines below are the aws signing 4 process
    //getFormattedDate return YYYYMMDD utc time string
    //getAWSFormattedDate return YYYMMDDTHHmmssZ utc time string
    String Date = timeClient.getFormattedDate();
    String AwsDate = timeClient.getAWSFormattedDate();
    String Signature = getAWSSignature (aws_secret, aws_region, aws_service, canonicalURL, aws_endpoint, payload, AwsDate, Date);
    String Authorization = "AWS4-HMAC-SHA256 Credential=" + aws_access + "/" + Date + "/ap-southeast-1/iotdata/aws4_request , SignedHeaders=content-type;host;x-amz-date , Signature=" + Signature;

    http.begin("https://endpoint/things/SIMcom32/shadow", test_root_ca); //Specify the URL and certificate
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Host", "endpoint.iot.ap-southeast-1.amazonaws.com");
    http.addHeader("X-Amz-Date", AwsDate);
    http.addHeader("Authorization", Authorization);
    int httpCode = http.GET();

    //Make the request
    if (httpCode > 0) { //Check for the returning code

      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    }
    else {
      Serial.println("Error on HTTP request");
    }
    http.end(); //Free the resources
  }
  //to prevent loop
  while (true);
  delay(10000);
}
