This project is a simple AWS IoT update from a ESP32 mircontroller. When I was working on the project, I did not like how many of the similar implementation of this project requires the use of MQTT to connect to AWS IoT. This required a lot for dependencies which was troublesome to troubleshoot. As part of my work, I decided to look for a better way to connect to AWS IoT without MQTT. Using AWS IoT documentation, I found you can connect directly to AWS IoT using HTTPS, however it requires authentication protocol label 'Signature Version 4'. Thus, with a little trial and error, I mananged to connect to AWS IoT without using AWS SDK hrough HTTPS (I don't think a sdk was out yet when i was working on it), using only standard libraries in arduino.

Links to projects

https://sites.google.com/d/1bQ2orMohzJhZuNtTdGGB-_nXD85q9wmp/p/15jjSAgF2nyLn2fagubsBNfcd8CLnjVu7/edit 
(Still using MQTT)

https://sites.google.com/view/smartclubhouseproject/side-projects/project-smart-door (continuation of the project just using ESP32)

Links to AWS IoT documentation on 'Signature Version 4'
https://docs.aws.amazon.com/iot/latest/developerguide/protocols.html#protocol-port-mapping
