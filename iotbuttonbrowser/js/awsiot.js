// Initialize the Amazon Cognito credentials provider
AWS.config.region = 'ap-southeast-1'; // Region
AWS.config.credentials = new AWS.CognitoIdentityCredentials({
    IdentityPoolId: 'ap-southeast-1:971470c8-53fa-4787-83db-02e7aa7a962a',
});

var iotdata = new AWS.IotData({endpoint: 'a1je8kwt0ntrsy.iot.ap-southeast-1.amazonaws.com'});

var updateparamsopen = {
    topic: '$aws/things/IoTDoor/shadow/get', /* required */
    payload: '{"state": {"desired": {"status": "open"}}}' /* Strings will be Base-64 encoded on your behalf */,
    qos: 0
};

var updateparamsclose = {
    topic: '$aws/things/IoTDoor/shadow/update', /* required */
    payload: '{"state":{"reported":{"status":"close"}}}' /* Strings will be Base-64 encoded on your behalf */,
    qos: 0
};

var getparams = {
    thingName: 'IoTDoor' /* required */
};

function updateThing() {
    iotdata.publish(updateparamsopen, function (err, data) {
        if (err) console.log(err, err.stack); // an error occurred
        else console.log(JSON.stringify(data));           // successful response
    });
}

function getThing() {
    iotdata.getThingShadow(getparams, function (err, data) {
        if (err) console.log(err, err.stack); // an error occurred
        else console.log(data);
        obj = JSON.parse(data.payload);
        console.log(obj.state.desired.status)
        if (obj.state.reported.status == "open") {
            iotdata.publish(updateparamsclose, function (err, data) {
                if (err) console.log(err, err.stack); // an error occurred
                else console.log(JSON.stringify(data));           // successful response
            });
        }
    });
}