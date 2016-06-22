//require('safe');

console.log("1");
Pebble.addEventListener('appmessage',
    function(e) {
        console.log('messag received');
        console.log('Received message: ' + JSON.stringify(e.payload));

        var req = new XMLHttpRequest();
        req.open("POST", "http://vassdoki.imind.hu/darts/store.php");
        //req.setRequestHeader('Content-Type', 'application/json');
        req.responseType = "html";
        //req.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
        req.setRequestHeader("Content-type", "application/x-www-form-urlencoded");


        req.onload = function(e) {
            if (req.readyState == 4 && req.status == 200) {
                if(req.status == 200) {
                    console.log("request sent xxxx");
                } else { console.log('Error'); }
            }
        };
        //req.send(JSON.stringify(e.payload));
        //req.send('{"pebble":"yes"}');
        req.send("pebble=" + JSON.stringify(e.payload));
        console.log("request sent: " + JSON.stringify(e.payload));
    }
);

console.log("2");

Pebble.addEventListener('ready', function(e) {
    console.log("ready event");
/*
    var dict = {
        '0':'Hello from PebbleKit JS!'
    };

    // Send a string to Pebble
    Pebble.sendAppMessage(dict, function(e) {
        console.log('Send successful.');
    }, function(e) {
        console.log('Send failed!');
    });

    console.log("sendAppMessage utan");
    */

    //Pebble.showSimpleNotificationOnPebble("Cím", "Szöveg Szöveg Szöveg Szöveg");
    //console.log("sime notification utan");
});

console.log("3");
