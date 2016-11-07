//require('safe');

var enable_debug = false;

debug("1");
Pebble.addEventListener('appmessage',
    function(e) {
        debug('messag received');
        debug('Received message: ' + JSON.stringify(e.payload));

        var req = new XMLHttpRequest();
        req.open("POST", "http://vassdoki.imind.hu/darts/store.php"); // not ready for multiplayer
        req.responseType = "html";
        req.setRequestHeader("Content-type", "application/x-www-form-urlencoded");


        req.onload = function(e) {
            if (req.readyState == 4 && req.status == 200) {
                if(req.status == 200) {
                    debug("request sent xxxx");
                } else { debug('Error'); }
            }
        };
        req.send("pebble=" + JSON.stringify(e.payload));
        debug("request sent: " + JSON.stringify(e.payload));
    }
);

debug("2");

Pebble.addEventListener('ready', function(e) {
    debug("ready event");
/*
    var dict = {
        '0':'Hello from PebbleKit JS!'
    };

    // Send a string to Pebble
    Pebble.sendAppMessage(dict, function(e) {
        debug('Send successful.');
    }, function(e) {
        debug('Send failed!');
    });

    debug("sendAppMessage utan");
    */

    //Pebble.showSimpleNotificationOnPebble("Cím", "Szöveg Szöveg Szöveg Szöveg");
    //debug("sime notification utan");
});

debug("3");

function debug(s) {
    if (enable_debug) {
        console.log(s);
    }
}