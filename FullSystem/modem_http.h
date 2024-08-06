// modem_http.h

#ifndef MODEM_HTTP_H
#define MODEM_HTTP_H

void initialiseModem();
void sendHttpPost(const char* url, const char* token, const char* contentType, const char* jsonData);
bool waitForResponse(const char* expectedResponse);
void checkThresholds();

#endif // MODEM_HTTP_H
